#include <driverlib.h>


void initUart() {
    // Configure the UART
    EUSCI_A_UART_initParam uartConfig = {
        .selectClockSource = EUSCI_A_UART_CLOCKSOURCE_SMCLK,
        .clockPrescalar = 6,             // BRDIV
        .firstModReg = 8,                // UCBRFx
        .secondModReg = 0x20,            // UCBRSx
        .parity = EUSCI_A_UART_NO_PARITY,
        .msborLsbFirst = EUSCI_A_UART_LSB_FIRST,
        .numberofStopBits = EUSCI_A_UART_ONE_STOP_BIT,
        .uartMode = EUSCI_A_UART_MODE,
        .overSampling = EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION
    };
    EUSCI_A_UART_init(EUSCI_A0_BASE, &uartConfig);

    // Enable RX and TX interrupts
    EUSCI_A_UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT | EUSCI_A_UART_TRANSMIT_INTERRUPT);

    // Enable the UART
    EUSCI_A_UART_enable(EUSCI_A0_BASE);
}


#pragma vector = USCI_A0_VECTOR 
__interrupt void USCI_A0_ISR(void) {
    switch(__even_in_range(UCA0IV,18)) {
        case 0x00: // Vector 0: No interrupts
            break;
        case 0x02: // Vector 2: UCRXIFG
            break;
        case 0x04: // Vector 4: UCTXIFG
            break;
        case 0x06: // Vector 6: UCSTTIFG
            break;
        case 0x08: // Vector 8: UCTXCPTIFG
            break;
        default: break;
    }
}
