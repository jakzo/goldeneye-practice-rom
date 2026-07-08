#!/usr/bin/env node

const fs = require('fs');
const os = require('os');
const path = require('path');
const { spawnSync } = require('child_process');

const PROMPT = 'Print out the three numbers in this image seperated by hyphens. Output only the these numbers and nothing else. Ignore the "P". If they are not discernable output `none`. Example output: 1234 - 567 - 8';

const args = {
  apiUrl: 'http://localhost:1234/v1',
  model: 'gemma-4-e2b-it-4bit',
  crop: '540:180:0:ih-180',
  start: 0,
  limit: null,
  concurrency: 1,
  keepFrames: true,
  videoPath: process.argv[2],
};

function runFfmpeg(outputDir) {
  const outputPattern = path.join(outputDir, 'frame-%06d.png');
  const filters = [];

  if (args.start > 0 || args.limit !== null) {
    const end = args.limit === null ? '1e12' : args.start + args.limit - 1;
    filters.push(`select='between(n\\,${args.start}\\,${end})'`);
  }
  filters.push(`crop=${args.crop}`);

  const ffmpegArgs = [
    '-v', 'error',
    '-i', args.videoPath,
    '-vf', filters.join(','),
    '-vsync', '0',
    outputPattern,
  ];

  const result = spawnSync('ffmpeg', ffmpegArgs, { stdio: 'inherit' });
  if (result.error) {
    throw result.error;
  }
  if (result.status !== 0) {
    throw new Error(`ffmpeg failed with exit code ${result.status}`);
  }
}

function frameFiles(outputDir) {
  return fs.readdirSync(outputDir)
    .filter((name) => /^frame-\d+\.png$/.test(name))
    .sort()
    .map((name) => path.join(outputDir, name));
}

let latestExample = null;

async function ocrFrame(filePath) {
  const image = fs.readFileSync(filePath).toString('base64');
  const exampleImage = latestExample && fs.readFileSync(latestExample.item).toString('base64');

  const response = await fetch(`${args.apiUrl.replace(/\/$/, '')}/chat/completions`, {
    method: 'POST',
    headers: { 'content-type': 'application/json' },
    body: JSON.stringify({
      model: args.model,
      messages: exampleImage
        ? [
            {
              role: 'user',
              content: [
                { type: 'text', text: PROMPT },
                {
                  type: 'image_url',
                  image_url: {
                    url: `data:image/png;base64,${exampleImage}`,
                  },
                },
              ],
            },
            {
              role: 'assistant',
              content: [
                { type: 'text', text: latestExample.value },
              ],
            },
            {
              role: 'user',
              content: [
                { type: 'text', text: 'What do the three numbers in this image say?' },
                {
                  type: 'image_url',
                  image_url: {
                    url: `data:image/png;base64,${image}`,
                  },
                },
              ],
            },
          ]
        : [{
          role: 'user',
          content: [
            { type: 'text', text: PROMPT },
            {
              type: 'image_url',
              image_url: {
                url: `data:image/png;base64,${image}`,
              },
            },
          ],
        }],
      temperature: 0,
      max_tokens: 16,
    }),
  });

  if (!response.ok) {
    const body = await response.text();
    throw new Error(`OCR request failed for ${path.basename(filePath)}: HTTP ${response.status} ${body}`);
  }

  const data = await response.json();
  return String(data.choices?.[0]?.message?.content ?? '').trim() || 'none';
}

async function main() {
  const outputDir = fs.mkdtempSync(path.join(os.tmpdir(), 'xframes-ocr-'));

  try {
    runFfmpeg(outputDir);
    const files = frameFiles(outputDir);

    if (files.length === 0) {
      throw new Error('No frames were extracted');
    }

    if (args.keepFrames) {
      console.error(`Cropped frames kept in ${outputDir}`);
    }

    let frameNum = 0;
    let prevMissionTimerTick = 0;

    for await (const result of mapWithConcurrencyInOrder(files, args.concurrency, ocrFrame)) {
      console.error(`${path.basename(result.item)} = ${result.value}`);

      const nums = result.value.split('-').map(Number);
      const [missionTimerTick, renderedFrameCount, droppedFrames] = nums;
      if (nums.length === 3 && !nums.some((n) => isNaN(n))) {
        if (frameNum === 0 && renderedFrameCount !== 0) {
          frameNum = renderedFrameCount;
        }
        if (frameNum !== 0) latestExample = result;
      }
      if (frameNum === 0) continue;

      frameNum++;
      const suffix = missionTimerTick ? `${missionTimerTick.toString().padStart(4, ' ')} - ${renderedFrameCount.toString().padStart(4, ' ')} - ${droppedFrames}` : '???'
      console.log(`${frameNum.toString().padStart(4, ' ')}: ${missionTimerTick === prevMissionTimerTick ? '' : suffix}`);
      prevMissionTimerTick = missionTimerTick;
    }
  } finally {
    if (!args.keepFrames) {
      fs.rmSync(outputDir, { recursive: true, force: true });
    }
  }
}

main().catch((error) => {
  console.error(error.message);
  process.exit(1);
});

async function* mapWithConcurrencyInOrder(items, concurrency, fn) {
  const results = new Map();
  let nextToStart = 0;
  let nextToYield = 0;
  let inFlight = 0;
  let notifyResult = null;

  function notify() {
    if (notifyResult) {
      const resolve = notifyResult;
      notifyResult = null;
      resolve();
    }
  }

  function waitForResult() {
    return new Promise((resolve) => {
      notifyResult = resolve;
    });
  }

  function launchMore() {
    while (inFlight < concurrency && nextToStart < items.length) {
      const index = nextToStart++;
      inFlight++;

      Promise.resolve()
        .then(() => fn(items[index], index))
        .then(
          (value) => results.set(index, { value }),
          (error) => results.set(index, { error }),
        )
        .finally(() => {
          inFlight--;
          launchMore();
          notify();
        });
    }
  }

  launchMore();

  while (nextToYield < items.length) {
    while (!results.has(nextToYield)) {
      await waitForResult();
    }

    const result = results.get(nextToYield);
    results.delete(nextToYield);

    if (result.error) {
      throw result.error;
    }

    yield {
      index: nextToYield,
      item: items[nextToYield],
      value: result.value,
    };
    nextToYield++;
  }
}
