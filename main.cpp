#include <stdio.h>

#include "stack.h"

int main(){
    Stack st = {INIT(st)};
    stackCtor(&st);
    int x = 0;
    stackPush(&st, 100);
    stackPush(&st, 200);
    stackPush(&st, 300);
    stackPush(&st, 400);
    stackPush(&st, 500);
    printf("%d\n", x);

}
