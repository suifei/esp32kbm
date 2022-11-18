const { readFile } = require('fs')
const { SerialPort, ReadlineParser } = require('serialport')

var args = process.argv; //2=file 3=dev 4=baudRate
if (args.length < 4) {
    console.log(`node app.js ./examples/tiktok.js /dev/cu.wchusbserial14142130 115200`);
    return;
}
const file = args[2]
const dev = args[3]
const baudRate = parseInt(args[4])

const port = new SerialPort({ path: dev, baudRate })
const parser = port.pipe(new ReadlineParser())

parser.on('data', console.log)
port.write('#CLS\n', 'ascii')
setTimeout(() => {
    readFile(file, { encoding: 'ascii' }, (err, data) => {
        if (err) throw err
        data.split('\n').forEach(v => { port.write(v + '\n', 'ascii'); console.log(v); })
        port.write('#RUN\n', 'ascii')
    })
}, 100)