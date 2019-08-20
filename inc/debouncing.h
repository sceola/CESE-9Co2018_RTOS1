/*=============================================================================
 * Copyright (c) 2019, Sebastian Ceola <sebastian.ceola@gmail.com>
 * All rights reserved.
 * License: mit (see LICENSE.txt)
 * Date: 2019/06/06
 * Version: 0.0.1
 *===========================================================================*/
#ifndef __DEBOUNCING_H__
#define __DEBOUNCING_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef enum _debounce_state
{
    ST_UNDEF,
    ST_HIGH,
    ST_HI_TO_LO,
    ST_LOW,
    ST_LO_TO_HI,
}
debounce_state;

typedef struct _debouncer_type
{
    unsigned        minpulse;
    debounce_state  state;
    int             pin;
    unsigned        elapsed;
    bool            edge;
}
debouncer_type;


void debouncer_init( debouncer_type* db, unsigned minpulse, int pin );

void debouncer_update( debouncer_type* db );

int  debouncer_is_hi( const debouncer_type* db );

bool debouncer_is_edge( const debouncer_type* db );


#ifdef __cplusplus
}
#endif

#endif
