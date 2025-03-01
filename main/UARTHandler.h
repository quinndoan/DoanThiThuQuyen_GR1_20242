#ifndef UART_TASK_H
#define UART_TASK_H

void initialize_uart();

void tx_task();     // truyền data vào write byte   
void rx_task();     // read byte từ RX

void process_uart_command(const char* command);

#endif