#define NDEBUG

#ifndef STACK_HEADER
#define STACK_HEADER

// #include "config.h"

#ifndef NDEBUG

#define INIT(st) 0, #st, __FILE__, __func__, __LINE__,
#define INIT_ARGS size_t left_canary, const char* name, const char* filename, const char* funcname, size_t line, size_t count
#define PUT_INIT_ARGS(st) st->name = name; st->filename = filename; st->funcname = funcname; st->line = line;
#define DUMB_ARGS(st) st, __FILE__, __func__, __LINE__
#define STACK_ASSERT(st)                                                        \
{                                                                               \
    if (stackErr(st)){                                                          \
        stackDump(DUMB_ARGS(st));                                               \
    }                                                                           \
}
#define DBG(...) __VA_ARGS__

#ifndef NDEBUG_CANARY

#define DBG_CANARY(...) __VA_ARGS__
#define INIT_STRUCT_CANARY                                                      \
if (counter_struct_canary == 0){                                                \
    counter_struct_canary++;                                                    \
    STRUCT_CANARY *= (((size_t)stk) << 8) + (((size_t)stk) >> 8);               \
}
#define INIT_DATA_CANARY                                                        \
if (counter_data_canary == 0){                                                  \
    counter_data_canary++;                                                      \
    DATA_CANARY *= (((size_t)&stk->data) << 8) + (((size_t)&stk->data) >> 8);   \
}

#else

#define DBG_CANARY(...)
#define INIT_STRUCT_CANARY
#define INIT_DATA_CANARY

#endif

#ifndef NDEBUG_SECURE_PTR

#define DBG_SECURE_PTR(...) __VA_ARGS__
#define INIT_XOR_KEY                                \
if (counter_key == 0){                              \
    counter_key++;                                  \
    KEY = (size_t)stk;                              \
}

#else

#define DBG_SECURE_PTR(...)
#define INIT_XOR_KEY

#endif

#ifndef NDEBUG_HASH

#define DBG_HASH(...) __VA_ARGS__

#else

#define DBG_HASH(...)

#endif

#else

#define INIT(st)
#define INIT_ARGS size_t count
#define PUT_INIT_ARGS(st)
#define DUMB_ARGS(st)
#define STACK_ASSERT(st)
#define DBG(...) __VA_ARGS__
#define DBG_CANARY(...)
#define DBG_SECURE_PTR(...)
#define DBG_HASH(...)
#define INIT_STRUCT_CANARY
#define INIT_DATA_CANARY
#define INIT_XOR_KEY
#define NOT_ENCAPSULATION

#endif



enum Errors{
    NO_ERROR,
    STACK_NOT_FOUND,
    STACK_OVERFLOW,
    LOST_DATA,
    STACK_UNDERFLOW,
    POISON_TOUCHED,
    LEFT_CANARY_TOUCHED,
    RIGHT_CANARY_TOUCHED,
    LEFT_DATA_CANARY_TOUCHED,
    RIGHT_DATA_CANARY_TOUCHED,
    HASH_DOES_NOT_MATCH
};

const char DUMP_FILENAME[] = "./dump.txt";
const int POISON = -42;
const int PUSH_RESIZE_MULTIPLIER = 2;
const int POP_RESIZE_DIVIDER = 2;
const int POP_MAX_CAPACITY_DIVIDE_SIZE_VALUE = 4;

typedef int StackElem;

#ifndef NOT_ENCAPSULATION

struct Stack;

#else

struct Stack{
    DBG_CANARY(size_t left_canary);
    DBG(const char* name);
    DBG(const char* filename);
    DBG(const char* funcname);
    DBG(size_t line);

    size_t size;
    size_t capacity;
    StackElem* data;

    DBG_CANARY(size_t right_canary);
    DBG_HASH(unsigned int hash);
};

#endif

Stack* stackCtor(INIT_ARGS, ...);
int stackPush(Stack* stk, StackElem value);
int stackPop(Stack* stk, StackElem* value);
int stackDtor(Stack** stk);

#endif
