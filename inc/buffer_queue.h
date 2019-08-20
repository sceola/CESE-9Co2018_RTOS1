/*=============================================================================
 * Copyright (c) 2019, Sebastian Ceola <sebastian.ceola@gmail.com>
 * All rights reserved.
 * License: mit (see LICENSE.txt)
 * Version: 0.0.1
 *===========================================================================*/

#ifndef __BUFFER_QUEUE_H__
#define __BUFFER_QUEUE_H__

#include <FreeRTOS.h>
#include <queue.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _buffer_queue
{
    QueueHandle_t   avail;
    QueueHandle_t   inuse;
    uint8_t*        mem; // Of size * n_elems
    unsigned        size;
    unsigned        n_elems;
}
buffer_queue;


int      buffer_queue_init     ( buffer_queue* bq, uint8_t* mem, unsigned size, unsigned n );
uint8_t* buffer_queue_get_avail( buffer_queue* bq, TickType_t xTicksToWait );
void     buffer_queue_push     ( buffer_queue* bq, uint8_t* buf );
uint8_t* buffer_queue_get_inuse( buffer_queue* bq, TickType_t xTicksToWait );
void     buffer_queue_return   ( buffer_queue* bq, uint8_t* buf );


#ifdef __cplusplus
}
#endif
#endif
