const { readFile } = require('fs')
const { SerialPort, ReadlineParser } = require('serialport')

var args = process.argv; //2=file 3=dev 4=baudRate
if (args.length < 4) {
    console.log(`node app.js ./dist/tiktok.min.js /dev/cu.wchusbserial14142130 115200`);
    return;
}
const file = args[2]
const dev = args[3]
const baudRate = parseInt(args[4])

const port = new SerialPort({ path: dev, baudRate })
const parser = port.pipe(new ReadlineParser())

parser.on('data', console.log)

port.write('#RESETALL\0')

setTimeout(() => {
    port.write('#CLS\0')

    setTimeout(() => {
        readFile(file, { encoding: 'ascii' }, (err, data) => {
            if (err) throw err
            data.split(";").forEach(buf => {
                console.log('SEND[0x' + (buf.length + 2).toString(16) + ']', buf + ";")
                port.write(buf + ";\0")
            })
            port.write('#RUN\0')
        })
    }, 1000)

}, 1000);


