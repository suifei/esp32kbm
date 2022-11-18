# esp32kbm

> suifei at gmail dot com
> 
> 注意事项：指令和代码注解不支持中文

基于ESP32开发的蓝牙无线鼠标、键盘，支持下发指令并编程（javascript）。

## 一、如何工作？

接通5V电源，启动设备后，将模拟蓝牙键盘和蓝牙鼠标。在 `iPhone` 内选择连接 `Smart Keyboard/Mouse`。
当没有连接任何设备时，LED 会闪烁。连接后，能够通过串行端口将命令发送到设备，设备将映射 `keyCodes` 并将其发送到蓝牙客户端 `iPhone`。

发送控制 LED 命令示例：

```js
pin_mode(2,3); //Sets the digital pin 2 as output
```

```js
#CLS
write(2,1); //Write a HIGH value to a digital pin.
delay(1000); //waits for a second
write(2,0); //Write a LOW value to a digital pin.
delay(1000); //waits for a second
#RUN
```

说明：基于`javascript`语法，当通过串行端口将此命令发送到设备。

## 二、命令

命令的结构很简单。区分为系统命令和脚本代码指令。回车符`\n`结束命令。注意，系统命令不支持行内注解。

### 1. 系统命令

```javascript
//查看当前运行的脚本代码
#CAT
//清空当前运行的脚本代码
#CLS 、#CLEAR
//清空所有寄存器和脚本代码
#RESETALL
//清空寄存器
#RESET
//运行脚本代码
#RUN
//停止脚本运行
#STOP
```

### 2. 脚本代码指令

```javascript
//全局寄存器16个，每个存储32位值，从0-15读写
rclear([i]); //清空寄存器，i<0或不传则清空全部，返回 true, false
rread(i); //读取第i个寄存器，返回值或0
rwrite(i,v); //写入v到第i个寄存器,返回 true , false
//硬件函数
print(); //打印输出到串口
delay(n); //延迟，n 毫秒
write(p,v); //IO写入，p 为IO位号 uint8，v 值int8，参考[电平信号]部分
v=read(p); //读取IO，p 为IO位号 uint8，v 返回值 int
pin_mode(p,m); //设置IO，p 为IO位号 uint8，m 模式，参考[模式]部分
//蓝牙函数
ble_check(); //检查蓝牙是否连接，bool == true 表示连接正常
ble_start(); //启动蓝牙监听，设备开机会被启动，代码不直接调用
//键盘函数，参考[按键码]定义
keyboard_print("aB123#$");//发送文本，自动映射ASCII按键 string
keyboard_println("atest");//发送文本，自动映射ASCII按键 string，带回车按键
keyboard_write(k); //发送按键消息，包含了(KeyDown,KeyUp)，k 按键码 uint8
keyboard_press(k); //发送按键消息，只包含(KeyDown)，组合键使用，k 按键码 uint8
keyboard_releaseAll(); //释放按键，多次 press 一起释放，组合键使用，无参数
//鼠标函数，参考[鼠标]定义，取值范围是-127 ~ 127
mouse_move(x,y); //发送鼠标移动消息，x,y = -1,0,1 ，分别表示 xy 负坐标,原点,正坐标，uint8
mouse_move_to(x,y[,step][,delay]); //移动鼠标到目标 x，y坐标，每次移动的步长，默认，-127~127，延时，默认 20,不可小于1
mouse_scroll(w); //发送鼠标滚动消息，w = -1,0,1，分别表示 y 负坐标,原点,正坐标，uint8，水平滚动未开放
mouse_click(b); //发送鼠标按键消息，b = 1/2/4/8/16，表示不同按键，包含(MouseDown，MouseUp)，uint8
mouse_down(b); //发送鼠标按键按下消息
mouse_up(b); //发送鼠标按键松开消息
```

#### 2.1 write 电平信号

```c
#define LOW               0x0
#define HIGH              0x1
```

#### 2.2 pin_mod 模式

```c
//GPIO FUNCTIONS
#define INPUT             0x01
// Changed OUTPUT from 0x02 to behave the same as Arduino pinMode(pin,OUTPUT) 
// where you can read the state of pin even when it is set as OUTPUT
#define OUTPUT            0x03 
#define PULLUP            0x04
#define INPUT_PULLUP      0x05
#define PULLDOWN          0x08
#define INPUT_PULLDOWN    0x09
#define OPEN_DRAIN        0x10
#define OUTPUT_OPEN_DRAIN 0x12
#define ANALOG            0xC0
//Interrupt Modes
#define DISABLED  0x00
#define RISING    0x01
#define FALLING   0x02
#define CHANGE    0x03
#define ONLOW     0x04
#define ONHIGH    0x05
#define ONLOW_WE  0x0C
#define ONHIGH_WE 0x0D
```

#### 2.3 键盘按键码定义

```c
const uint8_t KEY_LEFT_CTRL = 0x80;
const uint8_t KEY_LEFT_SHIFT = 0x81;
const uint8_t KEY_LEFT_ALT = 0x82;
const uint8_t KEY_LEFT_GUI = 0x83;
const uint8_t KEY_RIGHT_CTRL = 0x84;
const uint8_t KEY_RIGHT_SHIFT = 0x85;
const uint8_t KEY_RIGHT_ALT = 0x86;
const uint8_t KEY_RIGHT_GUI = 0x87;

const uint8_t KEY_UP_ARROW = 0xDA;
const uint8_t KEY_DOWN_ARROW = 0xD9;
const uint8_t KEY_LEFT_ARROW = 0xD8;
const uint8_t KEY_RIGHT_ARROW = 0xD7;
const uint8_t KEY_BACKSPACE = 0xB2;
const uint8_t KEY_TAB = 0xB3;
const uint8_t KEY_RETURN = 0xB0;
const uint8_t KEY_ESC = 0xB1;
const uint8_t KEY_INSERT = 0xD1;
const uint8_t KEY_DELETE = 0xD4;
const uint8_t KEY_PAGE_UP = 0xD3;
const uint8_t KEY_PAGE_DOWN = 0xD6;
const uint8_t KEY_HOME = 0xD2;
const uint8_t KEY_END = 0xD5;
const uint8_t KEY_CAPS_LOCK = 0xC1;
const uint8_t KEY_F1 = 0xC2;
const uint8_t KEY_F2 = 0xC3;
const uint8_t KEY_F3 = 0xC4;
const uint8_t KEY_F4 = 0xC5;
const uint8_t KEY_F5 = 0xC6;
const uint8_t KEY_F6 = 0xC7;
const uint8_t KEY_F7 = 0xC8;
const uint8_t KEY_F8 = 0xC9;
const uint8_t KEY_F9 = 0xCA;
const uint8_t KEY_F10 = 0xCB;
const uint8_t KEY_F11 = 0xCC;
const uint8_t KEY_F12 = 0xCD;
const uint8_t KEY_F13 = 0xF0;
const uint8_t KEY_F14 = 0xF1;
const uint8_t KEY_F15 = 0xF2;
const uint8_t KEY_F16 = 0xF3;
const uint8_t KEY_F17 = 0xF4;
const uint8_t KEY_F18 = 0xF5;
const uint8_t KEY_F19 = 0xF6;
const uint8_t KEY_F20 = 0xF7;
const uint8_t KEY_F21 = 0xF8;
const uint8_t KEY_F22 = 0xF9;
const uint8_t KEY_F23 = 0xFA;
const uint8_t KEY_F24 = 0xFB;

typedef uint8_t MediaKeyReport[2];

const MediaKeyReport KEY_MEDIA_NEXT_TRACK = {1, 0};
const MediaKeyReport KEY_MEDIA_PREVIOUS_TRACK = {2, 0};
const MediaKeyReport KEY_MEDIA_STOP = {4, 0};
const MediaKeyReport KEY_MEDIA_PLAY_PAUSE = {8, 0};
const MediaKeyReport KEY_MEDIA_MUTE = {16, 0};
const MediaKeyReport KEY_MEDIA_VOLUME_UP = {32, 0};
const MediaKeyReport KEY_MEDIA_VOLUME_DOWN = {64, 0};
const MediaKeyReport KEY_MEDIA_WWW_HOME = {128, 0};
const MediaKeyReport KEY_MEDIA_LOCAL_MACHINE_BROWSER = {0, 1}; // Opens "My Computer" on Windows
const MediaKeyReport KEY_MEDIA_CALCULATOR = {0, 2};
const MediaKeyReport KEY_MEDIA_WWW_BOOKMARKS = {0, 4};
const MediaKeyReport KEY_MEDIA_WWW_SEARCH = {0, 8};
const MediaKeyReport KEY_MEDIA_WWW_STOP = {0, 16};
const MediaKeyReport KEY_MEDIA_WWW_BACK = {0, 32};
const MediaKeyReport KEY_MEDIA_CONSUMER_CONTROL_CONFIGURATION = {0, 64}; // Media Selection
const MediaKeyReport KEY_MEDIA_EMAIL_READER = {0, 128};
```

#### 2.4 鼠标按键定义

```c
#define MOUSE_LEFT 1
#define MOUSE_RIGHT 2
#define MOUSE_MIDDLE 4
#define MOUSE_BACK 8
#define MOUSE_FORWARD 16
#define MOUSE_ALL (MOUSE_LEFT | MOUSE_RIGHT | MOUSE_MIDDLE) # For compatibility with the Mouse library
```

## 三、示例

### 1. 使用寄存器

```js
#CLS
rclear();   //clear all register
print(rread(0));    //read register0
rwrite(0,100);  //write register0 ,value 100
print(rread(0));    //print register0 value(100)
delay(1000);    //delay 1 sec
#RUN
```

### 2. 使用组合键

```js
#CLS

//Sending Ctrl+Alt+Delete
keyboard_press(0x80);//KEY_LEFT_CTRL
keyboard_press(0x82);//KEY_LEFT_ALT
keyboard_press(0xd4);//KEY_DELETE
delay(1000); //delay
keyboard_releaseAll();//release All
delay(500);

keyboard_print("aB123#$");//Sending Text
delay(500);
keyboard_println("atest");//Sending Test and 'Enter'
delay(1000);

#RUN
```

### 3. 鼠标移动坐标

```js
if(ble_check()){
    var count = 100;
    //x=0,y-1,Move mouse pointer up
    for (var i=0;i<count;i++){
        mouse_move(0,-1);
        delay(10);
    }
    //x-1,y=0,Move mouse pointer left
    for (var i=0;i<count;i++){
        mouse_move(-1,0);
        delay(10);
    }
    //x=0,y+1,Move mouse pointer down
    for (var i=0;i<count;i++){
        mouse_move(0,1);
        delay(10);
    }
    //x+1,y=0,Move mouse pointer right
    for (var i=0;i<count;i++){
        mouse_move(1,0);
        delay(10);
    }

    delay(1000);
}
```

### 4. 模拟拖动画面

状态机模式，区分每个动作，根据状态执行。

- 打开 TikTok App 后运行，自动切视频并点♥️。
- 命令行启动，进入项目`test`子目录
```bash
#查看串口，Windows 请到设备管理器查看 COM 号
ls /dev/*usb*
#node app.js 范例 串口路径 波特率
node app.js ./examples/tiktok.js /dev/cu.wchusbserial14142130 115200 
```

普通流程，注意初始化寄存器第一次修改后，重新发布脚本前，请主动调用 `rclear()` 重置，否则不会执行光标居中逻辑。

```js
#CLS
if (ble_check()) {
    var REG0 = 0x0;
    var INIT_MOUSE = 0x1;
    //read register 0
    if (rread(REG0) != INIT_MOUSE) {
        print("Init Mouse.");
        //write register 0, value 1
        rwrite(REG0, INIT_MOUSE);
        //move to left top
        mouse_move_to(-10000, -10000);
        delay(1000);
        //move to screen center
        mouse_move_to(800, 700);
        delay(1000);
    }
    mouse_down(1);
    delay(10);
    mouse_move_to(0, -700, 10, 15);
    delay(10);
    mouse_up(1);
    delay(600);
    mouse_move_to(0, 700, 10, 15);
    delay(100);
    mouse_click(1);
    delay(100);
    mouse_click(1);
    //wait 1~5 sec.
    delay(randRange(1000, 5000));
} else {
    delay(1000);
}
//random int (n~m)
function randRange(n, m) {
    return parseInt(Math.floor(Math.random() * (m - n)) + n);
}
#RUN
```

### 5. 鼠标滚动和按钮

```js
#CLS
mouse_scroll(-1); //w-1,Scroll Down
delay(500);
mouse_click(1); //MOUSE_LEFT click
delay(500);
mouse_click(2); //MOUSE_RIGHT click
delay(500);
mouse_click(4); //MOUSE_MIDDLE click
delay(500);
mouse_click(8); //MOUSE_BACK click
delay(500);
mouse_click(16); //MOUSE_FORWARD click
delay(500);
mouse_click(1 | 2); //Click left+right mouse button at the same time
delay(500);
mouse_click(1 | 2 | 4);//Click left+right mouse button and scroll wheel at the same time
delay(500);
#RUN
```

## 四、开发范例

设备与电脑之间的连接通过串口完成。足够满足调试需求。
串口波特率:`115200`

`test`文件夹为控制端测试例子，`test/examples/`存放的是`tikto`刷视频的实例。

```bash
cd test
node app.js ./examples/tiktok.js /dev/cu.wchusbserial14142130 115200
```

## 五、三方库

- duktape 2.7.0 http://duktape.org/
- ESP32-BLE-Combo 0.1.0 https://github.com/blackketter/ESP32-BLE-Combo

非常感谢所有这些库的作者和贡献者。

## 六、授权

The source code is licensed under GPL v3. License is available [here](/LICENSE).

ESP32KBM was originally developed by suifei.
