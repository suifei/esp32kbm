#include <Arduino.h>
#include <BleCombo.h>
#include <duktape.h>
#define LED 2
duk_context *ctx;
String script;
int ledState = LOW;
unsigned long previousMillis = 0;
bool canRun = false;

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
void js_setup() {
  ctx = duk_create_heap_default();

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

  duk_push_c_function(ctx, js_native_mouse_scroll, DUK_VARARGS);
  duk_put_global_string(ctx, "mouse_scroll");

  duk_push_c_function(ctx, js_native_mouse_click, DUK_VARARGS);
  duk_put_global_string(ctx, "mouse_click");

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
  duk_push_string(ctx, code);
  duk_int_t rc = duk_peval(ctx);

  if (rc != 0) {
    duk_safe_to_stacktrace(ctx, -1);
  } else {
    duk_safe_to_string(ctx, -1);
  }
  // String res = duk_get_string(ctx, -1);
  // Serial.printf("\n>>> Reslt: %s\n", res ? res : "null");
  duk_pop(ctx);
  //  duk_destroy_heap(ctx);
}

void setup() {
  Serial.begin(115200);
  js_setup();

  js_eval("print('Starting work!');");
  js_eval("pin_mode(2, 3);");
  js_eval("ble_start();");
}

void handleMessage() {
  digitalWrite(LED, HIGH);
  if (canRun && !script.isEmpty()) {
    js_eval(script.c_str());
    Serial.print(".");
  } else {
    canRun = false;
  }
  digitalWrite(LED, LOW);

  delay(1);
}

void loop() {
  if (Serial.available()) {
    digitalWrite(LED, HIGH);
    String cmd = Serial.readStringUntil('\n');
    if (strcmp(cmd.c_str(), "$CAT") == 0) {
      Serial.println("SCRIPT:");
      Serial.println(script);
    } else if (strcmp(cmd.c_str(), "$CLS") == 0) {
      Serial.println("SCRIPT CLEAR.");
      script = "";
      canRun = false;
    } else if (strcmp(cmd.c_str(), "$RUN") == 0) {
      Serial.println("SCRIPT RUN.");
      Serial.println(script);
      canRun = true;
    } else if (strcmp(cmd.c_str(), "$STOP") == 0) {
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
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= 500) {
      ledState = (ledState == LOW) ? HIGH : LOW;
      digitalWrite(LED, ledState);
      previousMillis = currentMillis;
    }
  } else if (ledState == HIGH) {
    digitalWrite(LED, LOW);
  }

}
