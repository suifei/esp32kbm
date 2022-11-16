#include <duktape.h>
#include <Arduino.h>

duk_context *ctx;

static duk_ret_t js_native_print(duk_context *ctx) {
  duk_push_string(ctx, " ");
  duk_insert(ctx, 0);
  duk_join(ctx, duk_get_top(ctx) - 1);
  Serial.printf("%s", duk_to_string(ctx, -1));

  return 0;
}

void js_setup ()
{
  ctx = duk_create_heap_default();

  duk_push_c_function(ctx, js_native_print, DUK_VARARGS);
  duk_put_global_string(ctx, "print");
}

void js_eval (char *code)
{
  Serial.printf("\n>>> Eval Code: %s\n", code);
  duk_push_string(ctx, code);
  duk_int_t rc = duk_peval(ctx);

  if (rc != 0) {
    duk_safe_to_stacktrace(ctx, -1);
  } else {
    duk_safe_to_string(ctx, -1);
  }

  String res = duk_get_string(ctx, -1);

  Serial.printf("\n>>> Reslt: %s\n", res ? res : "null");

  duk_pop(ctx);
//  duk_destroy_heap(ctx);
}

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(100);

  Serial.println("Loading JS Example...");

  // put your setup code here, to run once:
  js_setup();
  js_eval("print('Hello, world!');");
}

void loop() {
  // put your main code here, to run repeatedly:

}
