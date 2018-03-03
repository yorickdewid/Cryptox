/**
 * Copyright (C) 2017 Cryptox, Blub Corp.
 * All Rights Reserved
 *
 * This file is part of the Cryptox project.
 *
 */

#include "D:\Projects\Cryptox\src\crylib\srcctrl\main.cil.h"

#define CONSTANT    1
#define RETURN_OK   0

int main() {
    
    /* My first program in C. */
    printf("Hello, %s!\n", "world");
    
    int i = CONSTANT;
    if (i >= 1) {
        puts("statement true");
    } else {
        puts("statement false");
    }

    return RETURN_OK;
}
