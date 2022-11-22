var R0 = 0x0;
var R1 = 0x1;
var T_TC = 0x1;
var T_TU = 0x2;
var T_TDC = 0x3;
var T_TW = 0x4;
var T_M = 900;

if (ble_check()) {
    var stage = rread(R0);
    var mx = rand(-150, 150);
    switch (stage) {
        default: //default 0
            print('INIT');
            rwrite(R0, T_TC);
            delay(100);
            break;
        case T_TC:
            print('TO CENTER');
            rwrite(R0, T_TDC);
            mouse_move_to(-10000, -10000);//move to left top
            delay(100);
            mouse_move_to(800, T_M);//move to screen center
            delay(100);
            break;
        case T_TU:
            print('TO UP');
            rwrite(R0, T_TDC);
            mouse_down(1);
            delay(10);
            mouse_move_to(-mx, -T_M, 50, 15);
            delay(10);
            mouse_up(1);
            delay(10);
            mouse_move_to(mx, T_M, 100, 10);
            delay(10);
            break;
        case T_TDC:
            print('DCLICK');
            var my = rand(0, 300);
            rwrite(R0, T_TW);
            //double click
            mouse_move_to(-mx, -my, 50, 15);//random position
            delay(10);
            mouse_click(1);
            delay(100);
            mouse_click(1);
            delay(100);
            mouse_move_to(mx, my, 50, 15);//restore
            delay(10);
            break;
        case T_TW:
            print('WAIT');
            rwrite(R0, T_TU);
            //wait 1~10 sec.
            delay(rand(1000, 10000));
            if (rread(R1) > 20) {
                rclear(); //reset
            } else {
                rwrite(R1, rread(R1) + 1);//inc reg1
            }
            break;
    }
}
//random int (n~m)
function rand(n, m) {
    return parseInt(Math.floor(Math.random() * (m - n)) + n);
}