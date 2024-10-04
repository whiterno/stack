#include <stdio.h>

#include "./stack_hcpp/stack.h"

int main(){
    Stack st = {INIT(st)};
    stackCtor(&st, 2, 1, 2);
    int x = 0;
    stackPush(&st, 1);
    stackPush(&st, 2);
    stackPush(&st, 3);
    stackPush(&st, 4);
    stackPush(&st, 3);
    stackPush(&st, 4);
    stackPop(&st, &x);
    stackPop(&st, &x);
    stackPop(&st, &x);

    stackDtor(&st);
}
