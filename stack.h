#ifndef STACK_HEADER
#define STACK_HEADER

#define ON_DEBUG(...) __VA_ARGS__;
#define INIT(st) st.left_canary = 0, st.name = #st, st.filename = __FILE__, st.funcname = __func__, st.line = __LINE__, st.fp = fopen(DUMP_FILENAME, "w")

typedef int StackElem;

struct Stack{
    ON_DEBUG(size_t left_canary)
    ON_DEBUG(const char* name)
    ON_DEBUG(const char* filename)
    ON_DEBUG(const char* funcname)
    ON_DEBUG(size_t line)
    ON_DEBUG(FILE* fp)

    StackElem* data;
    size_t size;
    size_t capacity;

    ON_DEBUG(size_t right_canary)
};

enum Function{
    PUSH,
    POP
};

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
    RIGHT_DATA_CANARY_TOUCHED
};

const char DUMP_FILENAME[] = "./dump.txt";
const int POISON = -42;

int stackCtor(Stack* stk, ...);
int stackPush(Stack* stk, StackElem value);
int stackPop(Stack* stk, StackElem* value);
int stackDtor(Stack* stk);

#endif
