const { readFile } = require('fs')
const file = "dist/tiktok.min.js"


readFile(file, { encoding: 'ascii' }, (err, data) => {
    if (err) throw err
    data.split(";").forEach(v => {
        port.write(v + "\0")
    })
})
