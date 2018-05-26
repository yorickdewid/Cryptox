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
 * @purpose     Testcase for Print
 * @author      Blub Corp.
 * @modified    Yorick de Wid
 * @created     2018-05-27
 * @copyright   (C) Copyright 2018, Blub Corp, All Rights Reserved.
 * @license     GPL, Version 3
 */

#include <stdio.h>

int main(int argc, char *argv[]) {
    int anInt = 10;
    char aChar = 'Y';
    double aDouble = 123.459;

    printf("anInt = %d\n", anInt);
    printf("aChar = %c\n", aChar);
    printf("aDouble = %f\n", aDouble);
    printf("aDouble = %.2f\n", aDouble);

    return 0;
}
