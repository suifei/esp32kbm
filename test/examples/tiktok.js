var REG0 = 0x0;
var REG1 = 0x1;
var STAGE_TOCENTER = 0x1;
var STAGE_TOUP = 0x2;
var STAGE_DCLICK = 0x3;
var STAGE_WAIT = 0x4;

if (ble_check()) {
    var stage = rread(REG0);
    switch (stage) {
        default: //default 0
            print('INIT');
            rwrite(REG0, STAGE_TOCENTER);
            delay(100);
            break;
        case STAGE_TOCENTER:
            print('TO CENTER');
            rwrite(REG0, STAGE_DCLICK);
            mouse_move_to(-10000, -10000);//move to left top
            delay(10);
            mouse_move_to(800, 800);//move to screen center
            delay(10);
            break;
        case STAGE_TOUP:
            print('TO UP');
            rwrite(REG0, STAGE_DCLICK);
            mouse_down(1);
            delay(10);
            mouse_move_to(0, -700, 10, 15);
            delay(10);
            mouse_up(1);
            delay(100);
            mouse_move_to(0, 700, 10, 15);
            delay(100);
            break;
        case STAGE_DCLICK:
            print('DCLICK');
            rwrite(REG0, STAGE_WAIT);
            //double click
            mouse_click(1);
            delay(100);
            mouse_click(1);
            delay(100);
            break;
        case STAGE_WAIT:
            print('WAIT');
            rwrite(REG0, STAGE_TOUP);
            //wait 1~5 sec.
            delay(randRange(1000, 5000));
            if (rread(REG1) > 100) {
                reclear(); //reset
            } else {
                rwrite(REG1, rread(REG1) + 1);//inc reg1
            }
            break;
    }
}
//random int (n~m)
function randRange(n, m) {
    return parseInt(Math.floor(Math.random() * (m - n)) + n);
}