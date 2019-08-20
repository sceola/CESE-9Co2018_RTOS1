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

/**
 * Este modulo define un tipo de dato para intercambio de buffers de datos entre
 * tareas.  Funciona de la siguiente manera:
 *   1. Se crean 2 FIFOs para intercambio de punteros entre tareas.
 *   2. Una contiene punteros a buffers disponibles y la otra a buffers en uso.
 *   3. La memoria se inicializa externamente (podria ser estatica).
 *   4. En la inicializacion se divide la memoria en buffers y se colocan todos
 *      en la fila de buffers libres.
 *   5. Para obtener un buffers sobre el cual escribir se utiliza
 *      buffer_queue_get_avail.  Una vez escrito se lo envia utilizando
 *      buffer_queue_push y automaticamente se coloca en la lista de buffers en
 *      uso.  Si no habia ninguno disponible, retorna con NULL.
 *   6. Para obtener un buffer lleno se utiliza buffer_queue_get_inuse.  Una vez
 *      procesado se lo devuelve con buffer_queue_return para indicar que esta
 *      nuevamente disponible.  Si no habia ningun buffer lleno, returna NULL.
 *   7. Si siempre se utiliza la interfaz provista y no se hace nada raro, los
 *      metodos de _push y _return no pueden fallar, porque como la cantidad de
 *      buffers es acotada (determinada en el arranque) cuando se lo saca de los
 *      en uso sabemos que tiene que haber lugar en los vacios y viceversa.
 */

typedef struct _buffer_queue
{
    QueueHandle_t   avail;
    QueueHandle_t   inuse;
    uint8_t*        mem; // Of size * n_elems
    unsigned        size;
    unsigned        n_elems;
}
buffer_queue;


/**
 * Inicializa la lista de buffers.  Utiliza la memoria especificada en 'mem' 
 * y la divide en 'n' buffers de 'size' tamano cada uno.
 * Todos los buffers se indican como disponibles.
 * La memoria 'mem' debe persistir luego de la llamada a esta funcion, pues esta
 * estructura solo apunta a la memoria y nunca se aduena de ella.
 */
int      buffer_queue_init     ( buffer_queue* bq, uint8_t* mem, unsigned size, unsigned n );

/**
 * Obtener un buffer disponible.  NULL de no ser posible.
 */
uint8_t* buffer_queue_get_avail( buffer_queue* bq, TickType_t xTicksToWait );

/**
 * Enviar un buffer a la lista de buffers llenos.
 */
void     buffer_queue_push     ( buffer_queue* bq, uint8_t* buf );

/**
 * Obtener un buffer lleno.  NULL de no ser posible.
 */
uint8_t* buffer_queue_get_inuse( buffer_queue* bq, TickType_t xTicksToWait );

/**
 * Devolver un buffer a la lista de disponibles.
 */
void     buffer_queue_return   ( buffer_queue* bq, uint8_t* buf );


#ifdef __cplusplus
}
#endif
#endif
