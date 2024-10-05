#include <stdio.h>

#include "./stack_hcpp/stack.h"

int main(){
    Stack* st = stackCtor(INIT(st), 2, 1, 2);
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

    Stack* st2 = stackCtor(INIT(st2), 2, 1, 2);
    stackPush(&st2, 1);
    stackPush(&st2, 2);
    stackPush(&st2, 3);
    stackPush(&st2, 4);
    stackPush(&st2, 3);
    stackPush(&st2, 4);
    stackPop(&st2, &x);
    stackPop(&st2, &x);
    stackPop(&st2, &x);

    stackDtor(&st);
    stackDtor(&st2);

    Stack* st3 = stackCtor(INIT(st3), 2, 1, 2);
    stackPush(&st3, 1);
    stackPush(&st3, 2);
    stackPush(&st3, 3);
    stackPush(&st3, 4);
    stackPush(&st3, 3);
    stackPush(&st3, 4);
    stackPop(&st3, &x);
    stackPop(&st3, &x);
    stackPop(&st3, &x);

    stackDtor(&st3);

}
