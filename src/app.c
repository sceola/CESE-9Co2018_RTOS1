#include <FreeRTOS.h>
#include <task.h>
#include <board.h>
#include <string.h>

#include "app.h"
#include "adc.h"
#include "uart.h"


// DEBUG
// Esta constante es para aumentar el intervalo de las tareas, en caso de
// querer que se disparen mas lento para debugging.
#define DBG_PERIOD_MULTIPLIER    1


/// Memoria estatica de la aplicacion, para no ponerla en el stack.
uint8_t buffer_queue_mem[APP_DATA_BUF_SIZE * APP_DATA_BUF_NMBR];


/**
 * Tarea principal, espera que haya muestras del ADC y las envia por la UART,
 * luego espera la respuesta por UART o enciende el LED de error.
 */
void vTaskApp( void *pParam );

/**
 * Tarea del ADC, simplemente toma una muestra por iteracion y la coloca en un
 * buffer.  En caso de que se cambie el periodo de muestreo aqui adentro se
 * cambia el delay entre casa iteracion.
 * Se comunica con vTaskApp a traves de un buffer_queue.
 */
void vTaskADC( void *pParam );

/**
 * Trea de recepcion UART.  Esta escuchando la UART en caso de recibir algun
 * mensaje, para simplificar las cosas aceptamos cualquier mensaje como
 * Acknowledge de que todo esta bien y disparamos el semaforo que le notifica
 * a vTaskApp que los datos se enviaron correctamente.
 */
void vTaskUART( void *pParam );

/**
 * Actualiza el estado de las teclas para eliminar los rebotes.  En caso de
 * algun evento indica con un semaforo que hay una nueva configuracion por
 * aplicar.  Por ahora lo unico que hace es cambiar la frecuencia de muestreo.
 */
void vTaskButtons( void *pParam );

/**
 * Solo se dispara cuando hubo un error de recepcion UART y queda encendida por
 * APP_ERROR_ONTIME.  La tarea queda trabada indefinidamente mientras no se de
 * esa condicion.
 */
void vTaskError( void *pParam );


void app_update( app_type* app )
{
	// Pedimos un buffer lleno con muestras del ADC.
	// El timeout esta por si las dudas, si las cosas andan bien y no le paso
	// nada raro a la tarea del ADC siempre vamos a tener datos para procesar.

    const TickType_t timeout = pdMS_TO_TICKS(1000UL * DBG_PERIOD_MULTIPLIER);
    uint8_t* buf = buffer_queue_get_inuse(&app->data_queue, timeout);

    if (buf != NULL)
    {
        for (unsigned i = 0; i < APP_DATA_BUF_SIZE; ++i)
            uart_write(buf[i]);
        buffer_queue_return(&app->data_queue, buf);

        const TickType_t uart_timeout = pdMS_TO_TICKS(APP_UART_TIMEOUT);
        if (xSemaphoreTake(app->semaphore_reply, uart_timeout) != pdTRUE)
        {
            // Timeout
            xSemaphoreGive(app->semaphore_error);
        }
    }
    else
    {
        // ERROR
    }
}

void adc_update( app_type* app )
{
    uint8_t* buf = app->current_buffer;
    if (buf == NULL)
    {
        // Tenemos que pedir un buffer nuevo.  Puede que no haya ninguno
        // disponible si nadie los vacio todavia, en tal caso obtenemos el
        // proximo en uso y lo descartamos, seria como hacer una especia de
        // buffer circular.
        buf = buffer_queue_get_avail(&app->data_queue, 0);
        if (buf == NULL)
        {
            buf = buffer_queue_get_inuse(&app->data_queue, 0);
            if (buf != NULL)
            {
                buffer_queue_return(&app->data_queue, buf);
                buf = NULL;
            }
            else
            {
                // ERROR
            }
        }
        app->samples_in_buffer = 0;
        app->current_buffer = buf;
    }

    if (buf != NULL) // Solo leemos el ADC si tenemos un buffer disponible
    {
        buf[app->samples_in_buffer++] = adc_read(APP_ADC_CHANNEL);

        if (app->samples_in_buffer == APP_DATA_BUF_SIZE)
        {
            // Se lleno el buffer actual, enviarlo y marcarlo para pedir uno
            // nuevo en la proxima iteracion.
            buffer_queue_push(&app->data_queue, buf);
            app->current_buffer = NULL;
        }
    }
}

void uart_update( app_type* app )
{
    uint8_t data;
    if (uart_read(&data))
    {
		// Indicamos a vTaskApp que esta todo bien.
        xSemaphoreGive(app->semaphore_reply);
    }
}

void buttons_update( app_type* app )
{
    debouncer_update(&app->button_left );
    debouncer_update(&app->button_right);
    debouncer_update(&app->button_up   );
    debouncer_update(&app->button_down );

	// Bajar la frecuencia de muestreo.
    if (debouncer_is_edge(&app->button_left))
    {
        if (debouncer_is_hi(&app->button_left))
        {
            Board_LED_Set(LED_3, 0);
            if (app->sample_period < APP_ADC_MAX_RATE)
            {
                app->sample_period++;
                xSemaphoreGive(app->semaphore_config);
            }
        }
        else
        {
            Board_LED_Set(LED_3, 1);
        }
    }

	// Aumentar la frecuencia de muestreo.
    if (debouncer_is_edge(&app->button_right))
    {
        if (debouncer_is_hi(&app->button_right))
        {
            Board_LED_Set(LED_3, 0);
            if (app->sample_period > APP_ADC_MIN_RATE)
            {
                app->sample_period--;
                xSemaphoreGive(app->semaphore_config);
            }
        }
        else
        {
            Board_LED_Set(LED_3, 1);
        }
    }
}

void app_init( app_type* app )
{
    Board_Init();
    
	// Periodo de muestreo al maximo.
    app->sample_period = 0;

	// Inicializamos los semaforos.
    app->semaphore_config = xSemaphoreCreateBinary();
    app->semaphore_error  = xSemaphoreCreateBinary();
    app->semaphore_reply  = xSemaphoreCreateBinary();

	// Inicializamos la lista de buffers.
    buffer_queue_init( &app->data_queue,
                       buffer_queue_mem,
                       APP_DATA_BUF_SIZE,
                       APP_DATA_BUF_NMBR );

	// Iniciamos todas las tareas, estan ordenadas por prioridad.
    xTaskCreate( vTaskADC,
                 (const char*) "Task ADC",
                 configMINIMAL_STACK_SIZE,
                 app,
                 tskIDLE_PRIORITY+1,
                 NULL );

    xTaskCreate( vTaskApp,
                 (const char*) "Task APP",
                 configMINIMAL_STACK_SIZE,
                 app,
                 tskIDLE_PRIORITY+2,
                 NULL );

    xTaskCreate( vTaskUART,
                 (const char*) "Task UART",
                 configMINIMAL_STACK_SIZE,
                 app,
                 tskIDLE_PRIORITY+2,
                 NULL );

    xTaskCreate( vTaskButtons,
                 (const char*) "Task Buttons",
                 configMINIMAL_STACK_SIZE,
                 app,
                 tskIDLE_PRIORITY+3,
                 NULL );

    xTaskCreate( vTaskError,
                 (const char*) "Task Error",
                 configMINIMAL_STACK_SIZE,
                 app,
                 tskIDLE_PRIORITY+3,
                 NULL );
}


void vTaskApp( void *pParam )
{
    app_type* pApp = pParam;
    
    while (1)
    {
        app_update(pApp);  // Adentro espera por un buffer con datos.
    }
}

void vTaskADC( void *pParam )
{
    app_type* pApp = pParam;
    TickType_t xTaskDelay = pdMS_TO_TICKS((pApp->sample_period+1) * DBG_PERIOD_MULTIPLIER);
    TickType_t xLastWakeTime = xTaskGetTickCount();

    adc_init();
    pApp->current_buffer = NULL;
    
    while (1)
    {
        adc_update(pApp);

        if (xSemaphoreTake(pApp->semaphore_config, 0))
        {
            // Nueva configuracion
            xTaskDelay = pdMS_TO_TICKS((pApp->sample_period+1) * DBG_PERIOD_MULTIPLIER);
        }

        vTaskDelayUntil(&xLastWakeTime, xTaskDelay);
    }
}

void vTaskUART( void *pParam )
{
    app_type* pApp = pParam;
    const TickType_t xTaskDelay = pdMS_TO_TICKS(10UL * DBG_PERIOD_MULTIPLIER);
    TickType_t xLastWakeTime = xTaskGetTickCount();
    
    uart_init(115200);
    
    while (1)
    {
        uart_update(pApp);

        vTaskDelayUntil(&xLastWakeTime, xTaskDelay);
    }
}

void vTaskButtons( void *pParam )
{
    app_type* pApp = pParam;
    const TickType_t xTaskDelay = pdMS_TO_TICKS(40UL * DBG_PERIOD_MULTIPLIER);
    TickType_t xLastWakeTime = xTaskGetTickCount();

    debouncer_init(&pApp->button_left,  2, APP_BUTTON_PIN_LEFT );
    debouncer_init(&pApp->button_right, 2, APP_BUTTON_PIN_RIGHT);
    debouncer_init(&pApp->button_up,    2, APP_BUTTON_PIN_UP   );
    debouncer_init(&pApp->button_down,  2, APP_BUTTON_PIN_DOWN );
    
    while (1)
    {
        buttons_update(pApp);

        vTaskDelayUntil(&xLastWakeTime, xTaskDelay);
    }
}

void vTaskError( void *pParam )
{
    app_type* pApp = pParam;
    const TickType_t xTaskDelay = pdMS_TO_TICKS(APP_ERROR_ONTIME);
    
    while (1)
    {
        Board_LED_Set(LED_1, 0);
        xSemaphoreTake(pApp->semaphore_error, portMAX_DELAY);
        Board_LED_Set(LED_1, 1);
        vTaskDelay(xTaskDelay);
    }
}
