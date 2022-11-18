#include <Arduino.h>
#include <BleCombo.h>
#include <duktape.h>

#include "config.h"

duk_context *ctx;
String script;

int ledState = LOW;
unsigned long previousMillis = 0;
bool canRun = false;
int32_t runtime_register[0x10] = {0, 0, 0, 0, 0, 0, 0, 0,
                                  0, 0, 0, 0, 0, 0, 0, 0};

//清空寄存器
static duk_ret_t js_native_rclear(duk_context *ctx) {
  int index = -1;
  int n = duk_get_top(ctx);

  if (n > 0) {
    index = duk_to_number(ctx, 0);
  }
  // Serial.printf("rclear(%d);\n", index);
  if (index == -1) {
    memset(runtime_register, 0, 0x10);
    duk_push_boolean(ctx, true);
  } else if (index > 0xf) {
    duk_push_boolean(ctx, false);
  } else {
    runtime_register[index] = 0;
    duk_push_boolean(ctx, true);
  }
  return 1;
}
//读寄存器
static duk_ret_t js_native_rread(duk_context *ctx) {
  int index = duk_to_number(ctx, 0);
  if (index < 0 || index > 0xf) {
    duk_push_number(ctx, 0);
    return 1;
  }
  int32_t val = runtime_register[index];
  // Serial.printf("%d= rread(%d);\n", val, index);
  duk_push_number(ctx, val);
  return 1;
}
//写寄存器
static duk_ret_t js_native_rwrite(duk_context *ctx) {
  int index = duk_to_number(ctx, 0);
  int32_t val = duk_to_number(ctx, 1);
  // Serial.printf("rwrite(%d,%d);\n", index, val);
  if (index < 0 || index > 0xf) {
    duk_push_boolean(ctx, false);
    return 1;
  }
  runtime_register[index] = val;
  duk_push_boolean(ctx, true);
  return 1;
}

//打印到串口
static duk_ret_t js_native_print(duk_context *ctx) {
  duk_push_string(ctx, " ");
  duk_insert(ctx, 0);
  duk_join(ctx, duk_get_top(ctx) - 1);
  Serial.printf("%s\n", duk_safe_to_string(ctx, -1));
  return 0;
}
//检测蓝牙是否已连接
static duk_ret_t js_native_ble_check(duk_context *ctx) {
  duk_push_boolean(ctx, Keyboard.isConnected());
  return 1;
}
//启动蓝牙服务
static duk_ret_t js_native_ble_start(duk_context *ctx) {
  Keyboard.begin();
  Mouse.begin();
  return 0;
}
//移动鼠标
static duk_ret_t js_native_mouse_move(duk_context *ctx) {
  int x = duk_to_number(ctx, 0);
  int y = duk_to_number(ctx, 1);
  Mouse.move(x, y);
  return 0; /* one return value */
}
//移动鼠标到
static duk_ret_t js_native_mouse_move_to(duk_context *ctx) {
  int x = duk_to_number(ctx, 0);
  int y = duk_to_number(ctx, 1);
  int step = 127;
  int wait = 20;

  int n = duk_get_top(ctx);
  if (n > 2) {
    step = duk_to_number(ctx, 2);
    if (step < -127 || step > 127) {
      step = step < 0 ? -127 : 127;
    }
  }

  if (n > 3) {
    wait = duk_to_number(ctx, 3);
    if (wait < 1) {
      wait = 1;
    }
  }

  int tx1 = 0;
  int tx2 = x;
  if (x != 0) {
    tx1 = int(abs(x) / step);
    tx2 = int(abs(x) % step);
  }
  int ty1 = 0;
  int ty2 = y;
  if (y != 0) {
    ty1 = int(abs(y) / step);
    ty2 = int(abs(y) % step);
  }

  // Serial.printf("tx1:%d tx2:%d ty1:%d ty2:%d\n", tx1, tx2, ty1, ty2);

  int tx3 = 0;
  int ty3 = 0;
  while (tx3 != tx1 || ty3 != ty1) {
    int mx = 0;
    int my = 0;
    if (tx3 < tx1) {
      mx = x > 0 ? step : -step;
      tx3++;
    }
    if (ty3 < ty1) {
      my = y > 0 ? step : -step;
      ty3++;
    }

    Mouse.move(mx, my);
    delay(wait);
  }
  tx3 = 0;
  ty3 = 0;

  while (tx3 != tx2 || ty3 != ty2) {
    int mx = 0;
    int my = 0;
    if (tx3 < tx2) {
      mx = x > 0 ? 1 : -1;
      tx3++;
    }
    if (ty3 < ty2) {
      my = y > 0 ? 1 : -1;
      ty3++;
    }
    Mouse.move(mx, my);
    delay(wait);
  }

  return 0;
}
//滚动鼠标
static duk_ret_t js_native_mouse_scroll(duk_context *ctx) {
  int w = duk_to_number(ctx, 0);
  Mouse.move(0, 0, w);
  return 0; /* one return value */
}
//单击鼠标
static duk_ret_t js_native_mouse_click(duk_context *ctx) {
  int b = duk_to_number(ctx, 0);
  Mouse.click(b);
  return 0; /* one return value */
}
//鼠标按下
static duk_ret_t js_native_mouse_down(duk_context *ctx) {
  int b = duk_to_number(ctx, 0);
  Mouse.press(b);
  return 0; /* one return value */
}
//鼠标抬起
static duk_ret_t js_native_mouse_up(duk_context *ctx) {
  int b = duk_to_number(ctx, 0);
  Mouse.release(b);
  return 0; /* one return value */
}

//键盘输入字符串
static duk_ret_t js_native_keyboard_print(duk_context *ctx) {
  duk_push_string(ctx, " ");
  duk_insert(ctx, 0);
  duk_join(ctx, duk_get_top(ctx) - 1);
  Keyboard.print(duk_safe_to_string(ctx, -1));
  return 0;
}
//键盘输入字符串带换行
static duk_ret_t js_native_keyboard_println(duk_context *ctx) {
  duk_push_string(ctx, " ");
  duk_insert(ctx, 0);
  duk_join(ctx, duk_get_top(ctx) - 1);
  Keyboard.println(duk_safe_to_string(ctx, -1));
  return 0;
}

//键盘按键
static duk_ret_t js_native_keyboard_write(duk_context *ctx) {
  int k = duk_to_number(ctx, 0);
  Keyboard.write(k);
  return 0;
}
//键盘按下
static duk_ret_t js_native_keyboard_press(duk_context *ctx) {
  int k = duk_to_number(ctx, 0);
  Keyboard.press(k);
  return 0;
}
//键盘释放全部按下
static duk_ret_t js_native_keyboard_releaseAll(duk_context *ctx) {
  Keyboard.releaseAll();
  return 0;
}
//延迟
static duk_ret_t js_native_delay(duk_context *ctx) {
  int x = duk_to_number(ctx, 0);
  delay(x);
  return 0;
}
//写io
static duk_ret_t js_native_write(duk_context *ctx) {
  int p = duk_to_number(ctx, 0);
  int v = duk_to_number(ctx, 1);
  digitalWrite(p, v);
  return 0;
}
//读io
static duk_ret_t js_native_read(duk_context *ctx) {
  int p = duk_to_number(ctx, 0);
  int v = digitalRead(p);
  duk_push_number(ctx, v);
  return 1;
}
//设置io模式
static duk_ret_t js_native_pinMode(duk_context *ctx) {
  int p = duk_to_number(ctx, 0);
  int m = duk_to_number(ctx, 1);
  pinMode(p, m);
  return 0;
}

//注册js函数
void js_native_bind() {

  duk_push_c_function(ctx, js_native_rclear, DUK_VARARGS);
  duk_put_global_string(ctx, "rclear");

  duk_push_c_function(ctx, js_native_rwrite, DUK_VARARGS);
  duk_put_global_string(ctx, "rwrite");

  duk_push_c_function(ctx, js_native_rread, DUK_VARARGS);
  duk_put_global_string(ctx, "rread");

  duk_push_c_function(ctx, js_native_print, DUK_VARARGS);
  duk_put_global_string(ctx, "print");

  duk_push_c_function(ctx, js_native_delay, DUK_VARARGS);
  duk_put_global_string(ctx, "delay");

  duk_push_c_function(ctx, js_native_write, DUK_VARARGS);
  duk_put_global_string(ctx, "write");

  duk_push_c_function(ctx, js_native_read, DUK_VARARGS);
  duk_put_global_string(ctx, "read");

  duk_push_c_function(ctx, js_native_pinMode, DUK_VARARGS);
  duk_put_global_string(ctx, "pin_mode");

  duk_push_c_function(ctx, js_native_ble_check, DUK_VARARGS);
  duk_put_global_string(ctx, "ble_check");

  duk_push_c_function(ctx, js_native_ble_start, DUK_VARARGS);
  duk_put_global_string(ctx, "ble_start");

  duk_push_c_function(ctx, js_native_mouse_move, DUK_VARARGS);
  duk_put_global_string(ctx, "mouse_move");

  duk_push_c_function(ctx, js_native_mouse_move_to, DUK_VARARGS);
  duk_put_global_string(ctx, "mouse_move_to");

  duk_push_c_function(ctx, js_native_mouse_scroll, DUK_VARARGS);
  duk_put_global_string(ctx, "mouse_scroll");

  duk_push_c_function(ctx, js_native_mouse_click, DUK_VARARGS);
  duk_put_global_string(ctx, "mouse_click");

  duk_push_c_function(ctx, js_native_mouse_down, DUK_VARARGS);
  duk_put_global_string(ctx, "mouse_down");

  duk_push_c_function(ctx, js_native_mouse_up, DUK_VARARGS);
  duk_put_global_string(ctx, "mouse_up");

  duk_push_c_function(ctx, js_native_keyboard_print, DUK_VARARGS);
  duk_put_global_string(ctx, "keyboard_print");

  duk_push_c_function(ctx, js_native_keyboard_println, DUK_VARARGS);
  duk_put_global_string(ctx, "keyboard_println");

  duk_push_c_function(ctx, js_native_keyboard_write, DUK_VARARGS);
  duk_put_global_string(ctx, "keyboard_write");

  duk_push_c_function(ctx, js_native_keyboard_press, DUK_VARARGS);
  duk_put_global_string(ctx, "keyboard_press");

  duk_push_c_function(ctx, js_native_keyboard_releaseAll, DUK_VARARGS);
  duk_put_global_string(ctx, "keyboard_releaseAll");
}
//执行js代码
void js_eval(const char *code) {

  ctx = duk_create_heap_default();
  js_native_bind();
  duk_push_string(ctx, code);
  duk_int_t rc = duk_peval(ctx);

  if (rc != DUK_EXEC_SUCCESS) {

    Serial.printf("%s\n", duk_safe_to_stacktrace(ctx, -1));
    canRun = false;

  } else {
    duk_safe_to_string(ctx, -1);
    String res = duk_get_string(ctx, -1);
    if (!res.isEmpty() && strcmp(res.c_str(), "undefined") != 0) {
      Serial.printf("Result: %s\n", res);
    }
  }

  duk_pop(ctx);
  duk_destroy_heap(ctx);

  delay(1);
}

void setup() {
  Serial.begin(115200);
  js_eval("print('Starting work!');");
  js_eval("pin_mode(2, 3);");
  js_eval("ble_start();");
}

void flashLED(){
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 500) {
    ledState = (ledState == LOW) ? HIGH : LOW;
    digitalWrite(LED, ledState);
    previousMillis = currentMillis;
  }
}

void handleMessage() {

  if (canRun && !script.isEmpty()) {
    digitalWrite(LED, HIGH);
    js_eval(script.c_str());
    Serial.print(".");
    digitalWrite(LED, LOW);

  } else {
    canRun = false;

    flashLED();
  }
}

void loop() {
  if (Serial.available()) {
    digitalWrite(LED, HIGH);
    String cmd = Serial.readStringUntil('\n');
    if (cmd.equalsIgnoreCase("#CAT")) {
      Serial.println("SCRIPT:");
      Serial.println(script);
    } else if (cmd.equalsIgnoreCase("#RESETALL")) {
      Serial.println("RESET,clear all register and script code.");
      memset(runtime_register, 0, 0x10);
      script = "";
      canRun = false;
    } else if (cmd.equalsIgnoreCase("#RESET")) {
      Serial.println("RESET,clear all register.");
      memset(runtime_register, 0, 0x10);
    } else if (cmd.equalsIgnoreCase("#CLS") || cmd.equalsIgnoreCase("#CLEAR")) {
      Serial.println("SCRIPT CLEAR.");
      script = "";
      canRun = false;
    } else if (cmd.equalsIgnoreCase("#RUN")) {
      Serial.println("SCRIPT RUN.");
      Serial.println(script);
      canRun = true;
    } else if (cmd.equalsIgnoreCase("#STOP")) {
      Serial.println("SCRIPT STOP.");
      canRun = false;
    } else {
      script += cmd + "\n";
      Serial.println("OK");
    }
    digitalWrite(LED, LOW);
  }

  handleMessage();

  if (!Keyboard.isConnected()) {
    flashLED();
  } else if (ledState == HIGH) {
    digitalWrite(LED, LOW);
  }
}
