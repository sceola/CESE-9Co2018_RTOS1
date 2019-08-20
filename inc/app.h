/*=============================================================================
 * Copyright (c) 2019, Sebastian Ceola <sebastian.ceola@gmail.com>
 * All rights reserved.
 * License: mit (see LICENSE.txt)
 * Version: 0.0.1
 *===========================================================================*/

#ifndef __APP_H__
#define __APP_H__

#include <FreeRTOS.h>
#include <semphr.h>
#include <stdint.h>
#include <stddef.h>

#include "buffer_queue.h"
#include "debouncing.h"

#ifdef __cplusplus
extern "C" {
#endif


#define APP_UART_TIMEOUT    	250 // En MS
#define APP_ERROR_ONTIME    	500 // En MS

#define APP_ADC_CHANNEL     	ADC_CH2
#define APP_ADC_MIN_RATE		 0
#define APP_ADC_MAX_RATE		10

#define APP_BUTTON_PIN_LEFT 	BOARD_TEC_1
#define APP_BUTTON_PIN_RIGHT	BOARD_TEC_4
#define APP_BUTTON_PIN_UP   	BOARD_TEC_3
#define APP_BUTTON_PIN_DOWN 	BOARD_TEC_2

#define APP_DATA_BUF_SIZE   	16
#define APP_DATA_BUF_NMBR   	 8


typedef struct _app_type
{
    unsigned        	sample_period; // En ticks, se utiliza este valor +1. O sea, para muestrear cada 1 tick ponerlo en 0
    buffer_queue    	data_queue;

	// Teclas
    debouncer_type 		button_left;
    debouncer_type 		button_right;
    debouncer_type 		button_up;
    debouncer_type 		button_down;
	SemaphoreHandle_t	semaphore_config;

	// Indicacion de error para LED
	SemaphoreHandle_t	semaphore_error;

	// Indicacion de espera de respuesta por UART
	SemaphoreHandle_t	semaphore_reply;

    // Para la tarea del ADC
    unsigned        	samples_in_buffer;
    uint8_t*        	current_buffer;
}
app_type;


void app_init( app_type* app );


#ifdef __cplusplus
}
#endif
#endif
