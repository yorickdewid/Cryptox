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
 * @purpose     Testcase for booleans
 * @author      Blub Corp.
 * @modified    Yorick de Wid
 * @created     2018-05-27
 * @copyright   (C) Copyright 2018, Blub Corp, All Rights Reserved.
 * @license     GPL, Version 3
 */

#include <stdio.h>

int main(int argc, char *argv[]) {
    int a = 10;
    int b = 10;
    int c = 20;
    int z = 0;

    if (0)
        printf("0: True\n");
    else
        printf("0: False\n");

    if (1)
        printf("1: True\n");
    else
        printf("1: False\n");

    if (a == b)
        printf("a == b: True\n");
    else
        printf("a == b: False\n");

    if (a = 10)
        printf("a = c: True\n");
    else
        printf("a = c: False\n");

    if (z == 0)
        printf("z == 0: True\n");
    else
        printf("z == 0: False\n");

    if (z = 0)
        printf("z = 0: True\n");
    else
        printf("z = 0: False\n");

    return 0;
}
