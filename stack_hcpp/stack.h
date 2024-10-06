#ifndef STACK_HEADER
#define STACK_HEADER

#define ON_DEBUG(...) __VA_ARGS__;
#define DBG(...) __VA_ARGS__
#define INIT_STRUCT_CANARY                                              \
if (counter_struct_canary == 0){                                        \
    counter_struct_canary++;                                            \
    STRUCT_CANARY *= (((size_t)stk) << 8) + (((size_t)stk) >> 8);       \
}

#define INIT_DATA_CANARY                                                        \
if (counter_data_canary == 0){                                                  \
    counter_data_canary++;                                                      \
    DATA_CANARY *= (((size_t)&stk->data) << 8) + (((size_t)&stk->data) >> 8);   \
}

#define INIT_XOR_KEY                                \
if (counter_key == 0){                              \
    counter_key++;                                  \
    KEY = (size_t)stk;                              \
}

#define INIT(st) 0, #st, __FILE__, __func__, __LINE__
#define INIT_ARGS size_t left_canary, const char* name, const char* filename, const char* funcname, size_t line
#define PUT_INIT_ARGS(st) st->left_canary = left_canary; st->name = name; st->filename = filename; st->funcname = funcname; st->line = line;

#define DUMB_ARGS(st) st, __FILE__, __func__, __LINE__
#define STACK_ASSERT(st)                                                \
{                                                                       \
    if (stackErr(st)){                                                  \
        printf("shit");                                                 \
        stackDump(DUMB_ARGS(st));                                       \
    }                                                                   \
}

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

enum Function{
    PUSH,
    POP
};

const char DUMP_FILENAME[] = "./dump.txt";
const int POISON = -42;
const int PUSH_RESIZE_MULTIPLIER = 2;
const int POP_RESIZE_DIVIDER = 2;
const int POP_MAX_CAPACITY_DIVIDE_SIZE_VALUE = 4;

typedef int StackElem;

struct Stack;

Stack* stackCtor(INIT_ARGS, size_t count, ...);
int stackPush(Stack* stk, StackElem value);
int stackPop(Stack* stk, StackElem* value);
int stackDtor(Stack** stk);

#endif
