#ifndef UART_TASK_H
#define UART_TASK_H

#define UART_PORT_COMMAND   2
#define UART_PORT_RDM6300   1

#define RX_BUF_SIZE     1024

#define TXD_PIN_COMMAND         5
#define RXD_PIN_COMMAND         4

#define RX_PIN_RDM6300 16   
#define TX_PIN_RDM6300 17

void init_uart_for_RFID_125KHz();
void initialize_uart_command();
//int sendData();     // data length
void tx_task();     // truyền data vào write byte   
void rx_task();     // read byte từ RX
// hai hàm trên có thể thông cần mà dùng sẵn uart_read_bytes và uart_write_bytes
void process_uart_command(const char* command);
void RFID_125_Task(void *arg);
#endif