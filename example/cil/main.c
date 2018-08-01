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

int putchar(int c);

int puts(const char *str);

int main(int args, char *argv[]) {
    
    const char qq[] = "TEEESRHDEBNAHHEMRE";

    for (int i=0;i<sizeof(qq);++i) {
        putchar(qq[i]);
    }
    puts("");
    for (int i=0;i<sizeof(qq);++i) {
        putchar(qq[i]-5);
    }

    return 0;
}
