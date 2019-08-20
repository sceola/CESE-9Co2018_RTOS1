/*=============================================================================
 * Copyright (c) 2019, Sebastian Ceola <sebastian.ceola@gmail.com>
 * All rights reserved.
 * License: mit (see LICENSE.txt)
 * Date: 2019/06/17
 * Version: 0.0.1
 *===========================================================================*/

#ifndef __ADC_H__
#define __ADC_H__

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Inicializa el ADC con la siguiente configuracion:
 *     * Tasa de muestreo maxima, es mayor que las una muestra por tick de 1ms
 *       que vamos a usar.
 *     * 8 bits de ancho de palabra.
 *     * Todos los canales deshabilitados.
 */
void adc_init( void );

/**
 * Habilita el canal especificado y lee una muestra.
 */
uint8_t adc_read( int chn );


#ifdef __cplusplus
}
#endif
#endif
