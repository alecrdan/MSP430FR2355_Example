#include <driverlib.h>
#include <math.h>

// #define USE_SHARED_REG

volatile uint32_t tickCount = 0;

// Timer_B0 in up mode: SMCLK = 1 MHz -> Divider = 64 -> Timer clock = 1 MHz / 64 = 15625 Hz
void configureTimer(void) {
    const uint16_t ticksPerSecond   = 15625;
    const uint16_t ticksPerHalfSec  = floor(ticksPerSecond / 2);

    // Up mode CCR0 sets the period 
    Timer_B_initUpModeParam upModeConfig = {
        .clockSource                          = TIMER_B_CLOCKSOURCE_SMCLK,
        .clockSourceDivider                   = TIMER_B_CLOCKSOURCE_DIVIDER_64,
        .timerPeriod                          = ticksPerHalfSec,
        .timerInterruptEnable_TBIE            = TIMER_B_TBIE_INTERRUPT_DISABLE,
#ifdef USE_SHARED_REG
        .captureCompareInterruptEnable_CCR0_CCIE = TIMER_B_CCIE_CCR0_INTERRUPT_ENABLE,
#else
        .captureCompareInterruptEnable_CCR0_CCIE = TIMER_B_CCIE_CCR0_INTERRUPT_DISABLE,
#endif
        .timerClear                           = TIMER_B_DO_CLEAR,
        .startTimer                           = false
    };
    Timer_B_initUpMode(TIMER_B0_BASE, &upModeConfig);

#ifndef USE_SHARED_REG
    // CCR1 compare
    Timer_B_initCompareModeParam ccr1Config = {
        .compareRegister        = TIMER_B_CAPTURECOMPARE_REGISTER_1,
        .compareInterruptEnable = TIMER_B_CAPTURECOMPARE_INTERRUPT_ENABLE,
        .compareOutputMode      = TIMER_B_OUTPUTMODE_OUTBITVALUE,
        .compareValue           = ticksPerHalfSec
    };
    Timer_B_initCompareMode(TIMER_B0_BASE, &ccr1Config);

    // CCR2 compare
    Timer_B_initCompareModeParam ccr2Config = {
        .compareRegister        = TIMER_B_CAPTURECOMPARE_REGISTER_2,
        .compareInterruptEnable = TIMER_B_CAPTURECOMPARE_INTERRUPT_ENABLE,
        .compareOutputMode      = TIMER_B_OUTPUTMODE_OUTBITVALUE,
        .compareValue           = ticksPerHalfSec
    };
    Timer_B_initCompareMode(TIMER_B0_BASE, &ccr2Config);
#endif
    // Start timer in up mode
    Timer_B_startCounter(TIMER_B0_BASE, TIMER_B_UP_MODE);
}


#ifdef USE_SHARED_REG
// TIMER0_B0_VECTOR: CCR0 interrupt
#pragma vector = TIMER0_B0_VECTOR
__interrupt void TIMER0_B0_ISR(void) {
    tickCount++;

    // LED on P1.0: blink every 0.5 s (every CCR0 interrupt)
    GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);

    // LED on P1.1: blink every 1.0 s using a software divider
    if ((tickCount % 2) == 0) {
        GPIO_toggleOutputOnPin(GPIO_PORT_P6, GPIO_PIN6);
    }
}

#else
// TIMER0_B1_VECTOR: shared for CCR1 / CCR2 / TBIFG
#pragma vector = TIMER0_B1_VECTOR
__interrupt void TIMER0_B1_ISR(void) {
    // Reading TB0IV clears the corresponding CCIFG/TBIFG.
    switch (__even_in_range(TB0IV, TB0IV_TBIFG)) {
    case TB0IV_NONE:
        break;

    case TB0IV_TBCCR1:
        tickCount++;
        // LED on P1.0: toggles once per 1 s period
        if ((tickCount % 2) == 0) {
            GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
        }
        break;

    case TB0IV_TBCCR2:
        // LED on P16.6: toggles once per 0.5 s period
        GPIO_toggleOutputOnPin(GPIO_PORT_P6, GPIO_PIN6);
        break;

    case TB0IV_TBCCR3:
    case TB0IV_TBCCR4:
    case TB0IV_TBCCR5:
    case TB0IV_TBCCR6:
    case TB0IV_TBIFG:
    default:
        break;
    }
}
#endif
