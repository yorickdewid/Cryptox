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
 * @name        oddeven.c
 * @purpose     Demonstration of conditional operator
 * @author      Blub Corp.
 * @modified    Yorick de Wid
 * @created     2018-04-29
 * @copyright   (C) Copyright 2018, Blub Corp, All Rights Reserved.
 * @license     GPL, Version 3
 */

#include <stdio.h>

int main() {
    int number;

    printf("Enter an integer: ");
    scanf("%d", &number);

    (number % 2 == 0)
        ? printf("%d is even.", number)
        : printf("%d is odd.", number);

    return 0;
}
