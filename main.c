#include <driverlib.h>
#include "led_timer.h"

// ---------------- Clock setup ----------------

static void clockInit(void) {
    // MCLK = 1 MHz, SMCLK = 1 MHz, ACLK ≈ 32.768 kHz (REFO)
    CS_initClockSignal(CS_ACLK,  CS_REFOCLK_SELECT,   CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_SMCLK, CS_DCOCLKDIV_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_MCLK,  CS_DCOCLKDIV_SELECT, CS_CLOCK_DIVIDER_1);
}

int main(void) {
    // Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);

    clockInit();
    configureTimer();

    // Configure pins as outputs so you can see the different behaviors
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0); // toggles every 0.5 s (CCR0)
    GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN6); // CCR1 event (~0.25 s into each period)

    // Disable the GPIO power-on default high-impedance mode
    PMM_unlockLPM5();

    __enable_interrupt();

    // Do not exit program
    while (1) {
        __no_operation();
    }
}
