/**
 * Copyright (C) 2017 Quenza Inc. All Rights Reserved.
 * Copyright (C) 2018 Blub Corp. All Rights Reserved.
 *
 * This file is part of the Cryptox project.
 *
 * Content can not be copied and/or distributed without the express
 * permission of the author.
 */

/**
 * @name        10-hello.c
 * @purpose     Testcase for function calls
 * @author      Blub Corp.
 * @modified    Yorick de Wid
 * @created     2018-05-27
 * @copyright   (C) Copyright 2018, Blub Corp, All Rights Reserved.
 * @license     GPL, Version 3
 */

#include <stdio.h>

void function1(int);
void function2(int*);

int main(int argc, char *argv[]) {
    int anInt = 10;
    int *intPtr = &anInt;

    printf("Calling functions using anInt\n");
    function1( anInt );
    printf("Back from function 1 - anInt: %d\n", anInt);
    function2( &anInt );
    printf("Back from function 2 - anInt: %d\n", anInt);

    printf("Calling functions using intPtr\n" );
    function1( *intPtr );
    printf("Back from function 1 - anInt: %d\n", anInt);
    function2( intPtr );
    printf("Back from function 2 - anInt: %d\n", anInt);

    return 0;
}

void function1( int n ) {
    n = n * 2;
    printf("Function 1: n = %d\n", n);
}

void function2( int *n ) {
    *n = *n * 2;
    printf("Function 2: n = %d\n", *n);
}
