#ifndef M_CONFIG_H
#define M_CONFIG_H

//使用以下一个或多个来减少错误处理占用空间：
#undef DUK_USE_AUGMENT_ERRORS
#undef DUK_USE_TRACEBACKS
#undef DUK_USE_VERBOSE_ERRORS
#undef DUK_USE_VERBOSE_EXECUTOR_ERRORS
#undef DUK_USE_PC2LINE
//使用更慢但更紧凑的莱克算法（节省代码占用）：
#undef DUK_USE_LEXER_SLIDING_WINDOW
//禁用JSON快速路径（节省代码占用）：
#undef DUK_USE_JSON_STRINGIFY_FASTPATH
#undef DUK_USE_JSON_QUOTESTRING_FASTPATH
#undef DUK_USE_JSON_DECSTRING_FASTPATH
#undef DUK_USE_JSON_DECNUMBER_FASTPATH
#undef DUK_USE_JSON_EATWHITE_FASTPATH
//如果您不需要Node.js Buffer和ES2015类型数组支持，请使用：
#undef DUK_USE_BUFFEROBJECT_SUPPORT
//如果您不需要特定于Duktape的其他JX/JC格式，请使用：
#undef DUK_USE_JX
#undef DUK_USE_JC
//从ECMAScript ES2015借用的功能通常可以禁用（并非详尽无遗）：
#undef DUK_USE_ES6_OBJECT_SETPROTOTYPEOF
#undef DUK_USE_ES6_OBJECT_PROTO_PROPERTY
#undef DUK_USE_ES6_PROXY
//如果您不需要regexp支持，请使用：
#undef DUK_USE_REGEXP_SUPPORT
//禁用C API的不必要的部分：
#undef DUK_USE_BYTECODE_DUMP_SUPPORT
// Duktape调试代码使用大型静态临时缓冲区来格式化调试日志行。如果您在启用调试的情况下运行，请使用以下操作来减少此开销：
#undef DUK_USE_DEBUG_BUFSIZE
#define DUK_USE_DEBUG_BUFSIZE 2048

// LED
#define LED 2
#endif