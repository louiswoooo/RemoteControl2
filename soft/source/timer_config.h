#ifndef _SYS_TIME_CONFIG_
#define _SYS_TIME_CONFIG_

#define SYS_DURO_MS	10
#define TICK_PER_SECOND	1000/SYS_DURO_MS
extern u8 second,minute,hour,sys_tick;

void sys_time_init(void);

#endif
