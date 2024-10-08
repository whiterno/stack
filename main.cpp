#include <stdio.h>
#include <time.h>

#include "./stack_hcpp/stack.h"

const int STACK_SHIFT_DATA = 56;

int main(){
    int code_err = 0;

    size_t i_know_your_time = time(NULL);
    Stack* st = stackCtor(INIT(st) 2, 1, 2);
    Stack* real_stk_ptr = (Stack*) ((size_t) st ^ i_know_your_time);

    for (StackElem i = 1; i < 100; i++)
        if ((code_err = stackPush(st, i)) != 0)
            return code_err;

    (*((StackElem**) ((char*) real_stk_ptr + STACK_SHIFT_DATA)))[90] = 11111111;

    StackElem var_for_print = 0;
    for (int i = 1; i < 101; i++)
    {
        if ((code_err = stackPop(st, &var_for_print)) != 0)
            return code_err;
        printf("%d ", var_for_print);
    }
    putchar('\n');
}
