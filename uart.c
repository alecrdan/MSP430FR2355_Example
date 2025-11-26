#include <driverlib.h>
#include <stdint.h>


static void configureUartPins(void) {
    // Route P1.4 (RX) and P1.5 (TX) to the eUSCI_A0 peripheral
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN4 | GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION);
}


void InitUart(void) {
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

    configureUartPins();

    // Enable RX interrupt only - TX is handled by polling when needed
    EUSCI_A_UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);

    // Enable the UART
    EUSCI_A_UART_enable(EUSCI_A0_BASE);
}


void UartSendByte(uint8_t byte)
{
    // Wait until TX buffer is ready
    while (!EUSCI_A_UART_getInterruptStatus(EUSCI_A0_BASE, EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG));

    EUSCI_A_UART_transmitData(EUSCI_A0_BASE, byte);
}


uint8_t UartReceiveByteBlocking(void)
{
    while (!EUSCI_A_UART_getInterruptStatus(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG));

    return EUSCI_A_UART_receiveData(EUSCI_A0_BASE);
}


#pragma vector = USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{
    switch (__even_in_range(UCA0IV, 18)) {

        case 0x00: // No interrupt
            break;

        case 0x02: // RX interrupt
        {
            uint8_t rcvdByte = EUSCI_A_UART_receiveData(EUSCI_A0_BASE);

            // Echo received byte back out after TX buffer becomes available
            UartSendByte(rcvdByte);
            break;
        }

        case 0x06: // Start bit
            break;

        case 0x08: // TX complete
            break;

        default:
            break;
    }
}
