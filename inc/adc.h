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


void adc_init( void );
uint8_t adc_read( int chn );


#ifdef __cplusplus
}
#endif
#endif
