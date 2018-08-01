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
 * @name        main.c
 * @purpose     Original source test
 * @author      Blub Corp.
 * @modified    Yorick de Wid
 * @created     2017-08-02
 * @copyright   (C) Copyright 2018, Blub Corp, All Rights Reserved.
 * @license     GPL, Version 3
 */

#include "main.h"

#define CONSTANT    1
#define RETURN_OK   0

int main(int args, char *argv[]) {
    
    /* My first program in C. */
    printf("Hello, %d!\n", 12);
    
    int i = CONSTANT;
    if (i >= 1) {
        puts("statement true");                                 
    } else {
        puts("statement false");
    }

    return RETURN_OK;
}