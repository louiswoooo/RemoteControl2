/*****************************************************************************
  * @file:    	main.c
  * @author:  	louiswoo
  * @version: 	V1.0
  * @date:	2020-6-6
  * @brief:	主程序，初始化系统各个模块和各个任务
	while(1),执行各个任务任务函数在执行的时候需要控制好自己的
	时间计数器，控制好任务的执行时间，不要超时执行
*******************************************************************************/
#include "config.h"
#include "sys.h"
#include "debug.h"
#include "bsp.h"

void main(void)
{
		
	bsp();
	while(1)
	{
		if(wifi_receive())
		{
			debug(WIFI_RBUF);
			DevicesControl(WIFI_RBUF);
		}
	}
}
