#ifndef _SYS_TICK_H_
#define _SYS_TICK_H_

void sys_tick_init(void);		//10ms@22.1148MHz
void task0_Clock(void);
void sys_idle(void);
void dog_init(void);
void dog_clear(void);

#endif