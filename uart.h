#ifndef UART_H_
#define UART_H_

#include <stdint.h>

void InitUart(void);


void UartSendByte(uint8_t byte);


uint8_t UartReceiveByteBlocking(void);

#endif /* UART_H_ */
