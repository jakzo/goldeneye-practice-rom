Scripts and tools for calculating mission timer based only on visual analysis of the recording.

## Findings

It appears that if you can accurately detect the frame when the mission timer starts and stops (plus any pauses) you just multiply by the real-world elapsed time by ~0.997270 and you get the mission time (see drift-analysis.txt).
