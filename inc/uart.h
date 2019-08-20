/*=============================================================================
 * Copyright (c) 2019, Sebastian Ceola <sebastian.ceola@gmail.com>
 * All rights reserved.
 * License: mit (see LICENSE.txt)
 * Date: 2019/06/17
 * Version: 0.0.1
 *===========================================================================*/

#ifndef __UART_H__
#define __UART_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Inicializa la UART USB con la siguiente configuracion:
 *   * Baudrate indicado con 'brate'
 *   * 8 bits de datos
 *   * 1 bit de stop
 *   * 0 bits de paridad
 */
void uart_init( uint32_t brate );

/**
 * Escribe un byte por la UART USB.
 */
void uart_write( uint8_t d );


/**
 * Recibe un byte por la UART USB, devuelve false si no habia nada por recibir.
 */
bool uart_read( uint8_t* d );


#ifdef __cplusplus
}
#endif
#endif
