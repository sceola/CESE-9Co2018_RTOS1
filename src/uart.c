#include "sapi.h"
#include "uart.h"

void uart_init( uint32_t brate )
{
	//uartInit2(UART_USB, brate, 8, 0, 1);
	uartInit(UART_USB, brate);
}

void uart_write( uint8_t s )
{
    uartWriteByte(UART_USB, s);
}

bool uart_read( uint8_t* d )
{
	bool ret = false;
	if (uartRxReady(UART_USB))
		ret = true;
	*d = uartRxRead(UART_USB);
	return ret;
}
