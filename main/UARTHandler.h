#ifndef UART_TASK_H
#define UART_TASK_H
void init_uart_for_RFID_125KHz();
void initialize_uart_command();
//int sendData();     // data length
void tx_task();     // truyền data vào write byte   
void rx_task();     // read byte từ RX
// hai hàm trên có thể thông cần mà dùng sẵn uart_read_bytes và uart_write_bytes
void process_uart_command(const char* command);
void RFID_125_Task(void *arg);
#endif