const async = require("async");
const sleep = async (delay) => new Promise((resolve) => setTimeout(resolve, delay));

const { readFileSync } = require('fs');
const { SerialPort, ReadlineParser } = require('serialport');

var args = process.argv; //2=file 3=dev 4=baudRate
if (args.length < 4) {
    console.log(`node app.js ./dist/tiktok.min.js /dev/cu.wchusbserial320 115200`);
    return;
}
const file = args[2];
const dev = args[3];
const baudRate = parseInt(args[4]);

const port = new SerialPort({ path: dev, baudRate });
const parser = port.pipe(new ReadlineParser());

parser.on('data', console.log);

(async () => {
    await port.write('#RESETALL\0');
    await sleep(5000);
    const data = readFileSync(file, { encoding: 'ascii' }).split(";");
    for (let i = 0; i < data.length; i++) {
        const buf = data[i] + ";\0";
        await port.write(buf);
        console.log('SEND[0x' + buf.length.toString(16) + ']', buf);
        await sleep(100);
    }
    await port.write('#RUN\0');
})()
