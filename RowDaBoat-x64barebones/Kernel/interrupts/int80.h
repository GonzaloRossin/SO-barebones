#ifndef __INT80__
#define __INT80__
#include <stdint.h>

// the int 80h dispatcher
uint64_t int80Dispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rax);

char sys_read();			//sys call 0
void sys_write(char * rsi, int rdx);			//sys call 1
void sys_newline();                         //sys call 3
void sys_cpuInfo(char * vendor , uint32_t * version);//sys call 4

#endif