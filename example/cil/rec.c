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
 * @name        rec.c
 * @purpose     Test of record declaration
 * @author      Blub Corp.
 * @modified    Yorick de Wid
 * @created     2018-08-01
 * @copyright   (C) Copyright 2018, Blub Corp, All Rights Reserved.
 * @license     GPL, Version 3
 */

int main()  {
    struct location
    {
        int x;
        int y;
    };

    struct location loc;
    loc.x = 7364;
    loc.y = 8613;
 
    printf("loc %d:%d\n", loc.x, loc.y);

    return 0;
}
