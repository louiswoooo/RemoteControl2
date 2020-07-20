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
#include "http_ap.h"
#include "string.h"
#include "http_ap.h"

typedef enum _t_status_
{
	ap=1,
	client
}_t_status;

_t_status Status;

void main(void)
{
	u8 *p;
	Status = ap;
	bsp();
	while(1)
	{
		switch(Status)
		{
			case ap:
				if(wifi_receive())
				{
					debug(WIFI_RBUF);
					p = strstr(WIFI_RBUF, HTTP_REQUEST_INDEX);
					if(p)
						http_send(HTTP_INDEX_CONTENT);
					else
					{
						p = strstr(WIFI_RBUF, HTTP_REQUEST_CONTROL);
						if(p)
						{
							if(DevicesControl(p) == 1)
								http_send(HTTP_INDEX_CONTENT);
							else
								http_send("Devices control Fail !!!");
						}
					}
				}
				break;
			case client:
				break;
			default:
				break;
		}
		
	}
}
