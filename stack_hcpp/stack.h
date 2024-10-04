#ifndef STACK_HEADER
#define STACK_HEADER

#define ON_DEBUG(...) __VA_ARGS__;
#define INIT(st) st.left_canary = 0, st.name = #st, st.filename = __FILE__, st.funcname = __func__, st.line = __LINE__
#define DUMB_ARGS(st) st, __FILE__, __func__, __LINE__
#define STACK_ASSERT(st)                                                \
{                                                                       \
    if (!stackErr(stk)){                                                \
        stackDump(DUMB_ARGS(st));                                       \
    }                                                                   \
}
#define DBG(...) __VA_ARGS__

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

typedef int StackElem;

struct Stack{
    ON_DEBUG(size_t left_canary)
    ON_DEBUG(const char* name)
    ON_DEBUG(const char* filename)
    ON_DEBUG(const char* funcname)
    ON_DEBUG(size_t line)

    size_t size;
    size_t capacity;
    StackElem* data;

    ON_DEBUG(size_t right_canary)
    ON_DEBUG(size_t hash)
};

int stackCtor(Stack* stk, size_t count, ...);
int stackPush(Stack* stk, StackElem value);
int stackPop(Stack* stk, StackElem* value);
int stackDtor(Stack* stk);

#endif
