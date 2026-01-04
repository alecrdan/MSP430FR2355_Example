#include <driverlib.h>
#include <stdint.h>

// Interrupt configuration
const uint8_t adcIntrConfig = ADCIE0 | ADCOVIE | ADCTOVIE;

static void initPins(void) {
    // 1. Configure P1.2 for ADC functionality (A2 is on P1.2)
    GPIO_setAsPeripheralModuleFunctionInputPin(
        GPIO_PORT_P1, 
        GPIO_PIN2, // Changed to PIN2 for Channel A2
        GPIO_PRIMARY_MODULE_FUNCTION);
}

void initAdc(void) {
    initPins();

    ADC_init(ADC_BASE, 
            ADC_SAMPLEHOLDSOURCE_SC,
            ADC_CLOCKSOURCE_ADCOSC, 
            ADC_CLOCKDIVIDER_1);

    ADC_setupSamplingTimer(ADC_BASE, 
                           ADC_CYCLEHOLD_16_CYCLES, 
                           ADC_MULTIPLESAMPLESDISABLE);

    ADC_configureMemory(ADC_BASE,
                        ADC_INPUT_A2, 
                        ADC_VREFPOS_AVCC, 
                        ADC_VREFNEG_AVSS);

    ADC_enable(ADC_BASE);

    ADC_startConversion(ADC_BASE, 
                        ADC_REPEATED_SINGLECHANNEL);

    ADC_enableInterrupt(ADC_BASE, 
                        adcIntrConfig);
}


#pragma vector = ADC_VECTOR
__interrupt void ADC_ISR(void)
{
    switch (__even_in_range(ADCIV, ADCIV__ADCIFG0))
    {
        case ADCIV__ADCIFG0:  // Conversion complete
        {
            break;
        }

        case ADCIV__ADCOVIFG: // ADCMEM overflow
        {
            break;
        }

        case ADCIV__ADCTOVIFG: // Conversion-time overflow
        {
            break;
        }

        default:
            break;
    }
}
