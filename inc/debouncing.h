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


/**
 * Estado de debouncing de una tecla.
 * Tambien se utiliza para detectar el flanco.
 */
typedef enum _debounce_state
{
    ST_UNDEF,
    ST_HIGH,
    ST_HI_TO_LO,
    ST_LOW,
    ST_LO_TO_HI,
}
debounce_state;

/**
 * Objeto que representa una tecla con debouncing.
 */
typedef struct _debouncer_type
{
    unsigned        minpulse;
    debounce_state  state;
    int             pin;
    unsigned        elapsed;
    bool            edge;
}
debouncer_type;


/**
 * Inicializa la estructura de debouncing de una tecla.  Se configura el pin al
 * que corresponde y cuantos ticks (en llamadas a la funcion de _update) deben
 * pasar para considerar que una tecla se presiono y no fue un rebote.
 */
void debouncer_init( debouncer_type* db, unsigned minpulse, int pin );

/**
 * Actualiza el estado de debouncing de una tecla.
 */
void debouncer_update( debouncer_type* db );

/**
 * Indica si la tecla esta actualmente presionada ya habiendo descartado los
 * rebotes.
 */
int  debouncer_is_hi( const debouncer_type* db );

/**
 * Indica se la tecla acaba de cambiar de estado.
 * Utilizar en conjunto con debouncer_is_hi() para determinar si es un flanco
 * ascendente o descendente.
 */
bool debouncer_is_edge( const debouncer_type* db );


#ifdef __cplusplus
}
#endif

#endif
