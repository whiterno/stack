#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "stack.h"

#define STACK_ASSERT(st)                                                \
{                                                                       \
    if (!stackErr(st)){                                                 \
        stackDump(st, __FILE__, __func__, __LINE__, stackErr(st));      \
    }                                                                   \
}                                                                       \

#define DBG(...) __VA_ARGS__

inline static int resize(Stack* stk, int func);
static Errors stackErr(Stack* stk);
static int stackDump(const Stack* stk, const char* filename, const char* funcname, const size_t line, Errors err);
static void putDataCanaries(Stack* stk);
static const char* errorToString(Errors err);


int stackCtor(Stack* stk, ...){
    STACK_ASSERT(stk)

    va_list argptr;
    va_start(argptr, stk);
    size_t count = va_arg(argptr, size_t);

    stk->size = 0;
    stk->capacity = 0;
    stk->data = NULL;
    ON_DEBUG(stk->left_canary = 0xDEADDEAD)
    ON_DEBUG(stk->right_canary = 0xDEADDEAD)

    for (size_t elem = 0; elem < count; elem++){
        stackPush(stk, va_arg(argptr, StackElem));
    }

    va_start(argptr, stk);
    memset(argptr, 0, sizeof(size_t));
    va_end(argptr);

    STACK_ASSERT(stk)
    return 0;
}

int stackPush(Stack* stk, StackElem value){
    STACK_ASSERT(stk)

    if (stk->data != NULL){
        printf("left_canary - %#lX\n", *(((size_t*)stk->data) - 1));
        printf("right_canary - %#lX\n", *(size_t*)(stk->data + stk->capacity));
    }

    resize(stk, PUSH);
    stk->data[stk->size++] = value;

    STACK_ASSERT(stk)
    return NO_ERROR;
}

int stackPop(Stack* stk, StackElem* value){
    STACK_ASSERT(stk)

    if (resize(stk, POP) == STACK_UNDERFLOW){
        return STACK_UNDERFLOW;
    }

    stk->size--;
    stk->data[stk->size] = POISON;
    *value = stk->data[stk->size];

    STACK_ASSERT(stk)
    return NO_ERROR;
}

int stackDtor(Stack* stk){

    free((char*)stk->data DBG(- sizeof(size_t)));
    stk->size = 0;
    stk->capacity = 0;

    ON_DEBUG(fclose(stk->fp));

    return 0;
}

static Errors stackErr(Stack* stk){
    if (stk == NULL){
        return STACK_NOT_FOUND;
    }
    if (stk->size > stk->capacity){
        return STACK_OVERFLOW;
    }
    if (stk->data == NULL && stk->size != 0){
        return LOST_DATA;
    }
    for (size_t elem = stk->size; elem < stk->capacity; elem++){
        if (stk->data[elem] != POISON){
            return  POISON_TOUCHED;
        }
    }
    if (stk->left_canary != 0xDEADDEAD){
        return LEFT_CANARY_TOUCHED;
    }
    if (stk->right_canary != 0xDEADDEAD){
        return RIGHT_CANARY_TOUCHED;
    }
    if (stk->data != NULL && *((size_t*)stk->data - 1) != 0xDEADDEAD){
        return LEFT_DATA_CANARY_TOUCHED;
    }
    if (stk->data != NULL && *(size_t*)(stk->data + stk->capacity)!= 0xDEADDEAD){
        return RIGHT_DATA_CANARY_TOUCHED;
    }

    return NO_ERROR;
}

static int stackDump(const Stack* stk, const char* filename, const char* funcname, const size_t line, Errors err){
    fprintf(stk->fp, "-----------------------------------------------------------\n");
    fprintf(stk->fp, "CODE ERROR: %s\n", errorToString(err));
    if (err == STACK_NOT_FOUND){
        fprintf(stk->fp, "Stack not found.\n");
        return err;
    }
    fprintf(stk->fp, "From %s tack [%p] name = %s at %s:%lu born at %s:%lu (%s)\n", funcname, stk, stk->name, filename, line, stk->filename, stk->line, stk->funcname);
    fprintf(stk->fp, "{\n");
    fprintf(stk->fp, "\t%s.size     = %lu\n", stk->name, stk->size);
    fprintf(stk->fp, "\t%s.capacity = %lu\n", stk->name, stk->capacity);
    if (err == STACK_OVERFLOW){
        fprintf(stk->fp, "\tSize is bigger than capacity.\n");
        return err;
    }
    if (err == LOST_DATA){
        fprintf(stk->fp, "\tSize is not 0, but data has nothing in it.\n");
        return err;
    }
    fprintf(stk->fp, "\t%s.data[%p] =\n", stk->name, stk->data);
    fprintf(stk->fp, "\t{\n");
    for (size_t elem = 0; elem < stk->capacity; elem++){
        if (elem >= stk->size){
            fprintf(stk->fp, "\t\t [%lu]: %d (POISON)\n", elem, stk->data[elem]);
            continue;
        }
        fprintf(stk->fp, "\t\t*[%lu]: %d\n", elem, stk->data[elem]);
    }
    fprintf(stk->fp, "\t}\n");
    fprintf(stk->fp, "}\n");

    return err;
}

inline static int resize(Stack* stk, int func){
    STACK_ASSERT(stk)

    switch(func){
        case PUSH:{
            if (stk->capacity == 0){
                stk->data = (StackElem*)calloc(1, sizeof(StackElem) DBG(+ 2 * sizeof(size_t)));
                stk->capacity = 1;
                ON_DEBUG(putDataCanaries(stk))
                stk->data[0] = POISON;
            }
            if (stk->size == stk->capacity){
                stk->capacity *= 2;
                stk->data = (StackElem*)realloc((char*)stk->data - sizeof(size_t), sizeof(StackElem) * stk->capacity DBG(+ 2 * sizeof(size_t)));
                ON_DEBUG(putDataCanaries(stk))
                for (size_t elem = stk->size; elem < stk->capacity; elem++){
                    stk->data[elem] = POISON;
                }
            }
            STACK_ASSERT(stk)
            return 0;
        }
        case POP:{
            if (stk->size == 0){
                STACK_ASSERT(stk)
                return 1;
            }
            if (stk->size == stk->capacity / 4){
                stk->capacity /= 2;
                stk->data = (StackElem*)realloc((char*)stk->data - sizeof(size_t), sizeof(StackElem) * stk->capacity DBG(+ 2 * sizeof(size_t)));
                ON_DEBUG(putDataCanaries(stk))
            }
            STACK_ASSERT(stk)
            return 0;
        }
        default:{
            STACK_ASSERT(stk)
            return NO_ERROR;
        }
    }
}

static void putDataCanaries(Stack* stk){
    *(size_t*)stk->data = 0xDEADDEAD;
    stk->data = (StackElem*)((size_t*)stk->data  + 1);
    *(size_t*)(stk->data + stk->capacity) = 0xDEADDEAD;
}

#define _DESCR(str) case str: return #str;
static const char* errorToString(Errors err){
    switch(err){
        _DESCR(NO_ERROR)
        _DESCR(STACK_NOT_FOUND)
        _DESCR(STACK_OVERFLOW)
        _DESCR(LOST_DATA)
        _DESCR(STACK_UNDERFLOW)
        _DESCR(POISON_TOUCHED)
        _DESCR(LEFT_CANARY_TOUCHED)
        _DESCR(RIGHT_CANARY_TOUCHED)
        _DESCR(LEFT_DATA_CANARY_TOUCHED)
        _DESCR(RIGHT_DATA_CANARY_TOUCHED)
    }
}
#undef _DESCR
