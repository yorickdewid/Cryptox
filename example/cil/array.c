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
 * @name        array.c
 * @purpose     Demonstration of arrays and elements
 * @author      Blub Corp.
 * @modified    Yorick de Wid
 * @created     2018-04-29
 * @copyright   (C) Copyright 2018, Blub Corp, All Rights Reserved.
 * @license     GPL, Version 3
 */

#include <stdio.h>

int main()  {
    int array[100], n, c;
 
    printf("Enter number of elements in array\n");
    scanf("%d", &n);
 
    printf("Enter %d elements\n", n);
 
    for (c = 0; c < n; c++) 
        scanf("%d", &array[c]);
 
    printf("The array elements are:\n");
 
    for (c = 0; c < n; c++) {
        printf("%d\n", array[c]);
    }
 
    return 0;
}
