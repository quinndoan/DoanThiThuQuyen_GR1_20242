#ifndef UART_H
#define UART_H

void initializeUART();
int sendData();     // data length
void tx_task();     // truyền data vào write byte   
void rx_task();     // read byte từ RX
// hai hàm trên có thể thông cần mà dùng sẵn uart_read_bytes và uart_write_bytes


#endif