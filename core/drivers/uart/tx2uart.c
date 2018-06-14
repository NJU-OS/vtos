#include "tx2uart.h"

int init_uart(void)
{
    clock_enable(PERIPH_ID_UART1);
    return 0;
}