#include "sapi.h"
#include "adc.h"

void adc_init( void )
{
    // Copié lo de abajo de sapi_adc, para poder cambiar la resolución

    ADC_CLOCK_SETUP_T ADCSetup = {
       ADC_MAX_SAMPLE_RATE,
       ADC_8BITS,
       DISABLE
    };
    
    Chip_ADC_Init( LPC_ADC0, &ADCSetup );
    Chip_ADC_SetBurstCmd( LPC_ADC0, DISABLE );
    Chip_ADC_SetSampleRate( LPC_ADC0, &ADCSetup, ADC_MAX_SAMPLE_RATE/2 );
    Chip_ADC_EnableChannel( LPC_ADC0, ADC_CH1, DISABLE );
    Chip_ADC_Int_SetChannelCmd( LPC_ADC0, ADC_CH1, DISABLE );
    Chip_ADC_EnableChannel( LPC_ADC0, ADC_CH2, DISABLE );
    Chip_ADC_Int_SetChannelCmd( LPC_ADC0, ADC_CH2, DISABLE );
    Chip_ADC_EnableChannel( LPC_ADC0, ADC_CH3, DISABLE );
    Chip_ADC_Int_SetChannelCmd( LPC_ADC0, ADC_CH3, DISABLE );
    Chip_ADC_EnableChannel( LPC_ADC0, ADC_CH4, DISABLE );
    Chip_ADC_Int_SetChannelCmd( LPC_ADC0, ADC_CH4, DISABLE );
}

uint8_t adc_read( int chn )
{
    return adcRead(chn);
}
