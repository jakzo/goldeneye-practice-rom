const lines=[]
for(let tick=0; ; tick++){
    const secs=Math.floor(tick/60)
    const huns=Math.floor((tick%60)*100/60)
    lines.push(`${tick.toString().padStart(4, ' ')} - ${secs.toString().padStart(2, '0')}.${huns.toString().padStart(2, '0')}`)
    if(secs>=30)break
}
require('fs').writeFileSync('frame-times.txt', lines.join('\n'))