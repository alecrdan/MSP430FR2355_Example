#include <driverlib.h>
#include <stdint.h>

/*
 *  MSP-EXP430FR2355 UART internal loopback (UCA1 with UCLISTEN set):
 *  RX interrupts are triggered purely in hardware — no pin setup required.
 */

static void enableUartLoopback(void)
{
    // Route TX internally back to RX so we don't need an external jumper
    HWREG16(EUSCI_A1_BASE + OFS_UCAxSTATW) |= UCLISTEN;
}


// -------------------------------
// UART INITIALIZATION (A1)
// -------------------------------
void InitUart(void)
{
    // UART baud: SMCLK = 1 MHz, BRDIV=6, BRS=0x20, BRF=8 → 9600 baud
    EUSCI_A_UART_initParam uartConfig = {
        .selectClockSource = EUSCI_A_UART_CLOCKSOURCE_SMCLK,
        .clockPrescalar    = 6,
        .firstModReg       = 8,
        .secondModReg      = 0x20,
        .parity            = EUSCI_A_UART_NO_PARITY,
        .msborLsbFirst     = EUSCI_A_UART_LSB_FIRST,
        .numberofStopBits  = EUSCI_A_UART_ONE_STOP_BIT,
        .uartMode          = EUSCI_A_UART_MODE,
        .overSampling      = EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION
    };

    // Initialize UART A1
    EUSCI_A_UART_init(EUSCI_A1_BASE, &uartConfig);

    // Enable RX interrupt only (TX is polled)
    EUSCI_A_UART_enableInterrupt(EUSCI_A1_BASE,
                                 EUSCI_A_UART_RECEIVE_INTERRUPT);

    // Enable the UART peripheral
    EUSCI_A_UART_enable(EUSCI_A1_BASE);

    // Allow loopback so no external wiring is needed
    enableUartLoopback();
}


// -------------------------------
// UART SEND BYTE (blocking)
// -------------------------------
void UartSendByte(uint8_t byte)
{
    // Wait for TX buffer to be ready
    while (!EUSCI_A_UART_getInterruptStatus(EUSCI_A1_BASE,
                                            EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG));

    EUSCI_A_UART_transmitData(EUSCI_A1_BASE, byte);
}


// -------------------------------
// UART RECEIVE BYTE (blocking)
// -------------------------------
uint8_t UartReceiveByteBlocking(void)
{
    while (!EUSCI_A_UART_getInterruptStatus(EUSCI_A1_BASE,
                                            EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG));

    return EUSCI_A_UART_receiveData(EUSCI_A1_BASE);
}


// -------------------------------
// UART ISR — eUSCI_A1
// -------------------------------
#pragma vector = EUSCI_A1_VECTOR
__interrupt void EUSCI_A1_ISR(void)
{
    switch (__even_in_range(UCA1IV, 18))
    {
        case 0x00:  // No interrupt
            break;

        case 0x02:  // RX interrupt
        {
            uint8_t rcvdByte = EUSCI_A_UART_receiveData(EUSCI_A1_BASE);
            UartSendByte(rcvdByte);  // Echo
            break;
        }

        case 0x06:  // Start bit
            break;

        case 0x08:  // TX complete
            break;

        default:
            break;
    }
}
