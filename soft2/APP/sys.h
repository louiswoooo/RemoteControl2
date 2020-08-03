#ifndef _SYS_TICK_H_
#define _SYS_TICK_H_
#define TICK_PER_SECOND	100			//定义每秒的时间片sys_tick数
#define MS_PER_TICK	(1000/TICK_PER_SECOND)
void sys_tick_init(void);		//10ms@22.1148MHz
void task0_Clock(void);
void sys_idle(void);
void dog_init(void);
void dog_clear(void);

#endif
