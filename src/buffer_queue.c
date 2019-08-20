#include "buffer_queue.h"


uint8_t* s__get_buffer( QueueHandle_t handle, TickType_t xTicksToWait )
{
    uint8_t* ret;
    BaseType_t xSts = xQueueReceive(handle, &ret, xTicksToWait);
    return (xSts == pdPASS) ? ret : NULL;
}


int buffer_queue_init( buffer_queue* bq, uint8_t* mem, unsigned size, unsigned n )
{
    bq->avail    = xQueueCreate(n, sizeof(mem));
    bq->inuse    = xQueueCreate(n, sizeof(mem));
    bq->mem      = mem;
    bq->size     = size;
    bq->n_elems  = n;

    BaseType_t xSts = pdPASS;
    for (unsigned i = 0; i < n && xSts == pdPASS; ++i)
    {
        xSts = xQueueSendToBack(bq->avail, &mem, 0);
        mem += size;
    }

    return (xSts == pdPASS) ? 0 : -1;
}

uint8_t* buffer_queue_get_avail( buffer_queue* bq, TickType_t xTicksToWait )
{
    return s__get_buffer(bq->avail, xTicksToWait);
}

void buffer_queue_push( buffer_queue* bq, uint8_t* buf )
{
    xQueueSendToBack(bq->inuse, &buf, 0);
}

uint8_t* buffer_queue_get_inuse( buffer_queue* bq, TickType_t xTicksToWait )
{
    return s__get_buffer(bq->inuse, xTicksToWait);
}

void buffer_queue_return( buffer_queue* bq, uint8_t* buf )
{
    xQueueSendToBack(bq->avail, &buf, 0);
}
