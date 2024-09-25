#ifndef STACK_HEADER
#define STACK_HEADER

#define ON_DEBUG(...) __VA_ARGS__
#define INIT(st) st.name = #st, st.filename = __FILE__, st.funcname = __func__, st.line = __LINE__

typedef int StackElem;

struct Stack{
    ON_DEBUG(const char* name);
    ON_DEBUG(const char* filename);
    ON_DEBUG(const char* funcname);
    ON_DEBUG(size_t line);

    StackElem* data;
    size_t size;
    size_t capacity;
};

enum Errors{
    NO_ERROR,
    STACK_NOT_FOUND,
    SIZE_BIGGER_CAPACITY,
    LOST_DATA
};

int stackCtor(Stack* stk);
int stackPush(Stack* stk, StackElem value);
int stackPop(Stack* stk, StackElem* value);
int stackDtor(Stack* stk);
int stackErr(Stack* stk);
int stackDump(const Stack* stk, const char* filename, const char* funcname, const size_t line);
int stackAssert(Stack stk, const char* filename, const char* funcname, const int line);

#endif
