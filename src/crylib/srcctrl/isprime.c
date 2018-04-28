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
 * @name        isprime.c
 * @purpose     Demonstration of loops and conditions
 * @author      Blub Corp.
 * @modified    Yorick de Wid
 * @created     2018-04-29
 * @copyright   (C) Copyright 2018, Blub Corp, All Rights Reserved.
 * @license     GPL, Version 3
 */

#include <stdio.h>

int main() {
    int n, c;

    printf("Enter a number\n");
    scanf("%d", &n);

    if (n == 2)
        printf("Prime number.\n");
    else
    {
        for (c = 2; c <= n - 1; c++) {
            if (n % c == 0) break;
        }

        if (c != n) puts("Not prime.");
        else puts("Prime number.");
    }

    return 0;
}
