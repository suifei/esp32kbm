const { SerialPort, ReadlineParser } = require('serialport')
const path = "/dev/cu.wchusbserial14143130"
const baudRate = 115200
const port = new SerialPort({ path, baudRate })
const parser = port.pipe(new ReadlineParser())

parser.on('data', console.log)

console.log("发送重新启动 LCTRL 29 + LALT 56 + LSHIFT 42 + CMD 125 + R 19")


async function keyDownUp( ...keycodes) {
    //keydown
    for (let n = 0; n < keycodes.length; n++){
        port.write(`D;${keycodes[n]}\n`)
    }
    await sleep(1000)
    //keyup
    keycodes = keycodes.reverse();
    for (let n = 0; n < keycodes.length; n++) {
        port.write(`U;${keycodes[n]}\n`)
    }
}

async function keyPress(...keycodes) {
    //keydown
    for (let n = 0; n < keycodes.length; n++) {
        port.write(`P;${keycodes[n]}\n`)
    }
}


// keyPress(29, 56, 42, 125, 19)
keyDownUp(42, 30)

keyPress(42, 30)


function sleep(time) {
    return new Promise((resolve) => {
        setTimeout(() => {
            resolve();
        }, time);
    });
}