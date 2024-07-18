// this is a C sample for point to function usage

#include <stdio.h>


void sum_int(void* a, void* b, void* result)
{
    *(int*) result = *(int*) a + *(int*) b;
}

void sum_double(void* a, void* b, void* result)
{
    *(double*) result = *(double*) a + *(double*)b;
}

int main(int argc, char *argv[])
{
    void (*sum)(void*, void*,void*);

    sum = &sum_int;
    int arg1i = 1;
    int arg2i = 2;
    int resulti;
    sum(&arg1i, &arg2i, &resulti);
    printf("result 1 = %d\n", resulti);

    sum = &sum_double;
    double arg1f = 1.0;
    double arg2f = 2.0;
    double resultf;
    sum(&arg1f, &arg2f, &resultf);
    printf("result 2 = %f\n", resultf);

    return 0;
}
