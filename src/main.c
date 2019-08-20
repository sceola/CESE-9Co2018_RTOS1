/*=============================================================================
 * Copyright (c) 2019, Sebastian Ceola <sebastian.ceola@gmail.com>
 * All rights reserved.
 * License: mit (see LICENSE.txt)
 * Date: 2019/06/17
 * Version: 0.0.1
 *===========================================================================*/

#include "FreeRTOS.h"
#include "task.h"
#include "app.h"


int main( void )
{
    static app_type app;

    // ----- Setup Application -----------------------
    app_init(&app);

    vTaskStartScheduler();
    
    // We should never reach this point.
    while (1)
    {
    }
    return 0;
}
