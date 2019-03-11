#include "ControlTask.h"

#include "Driver_Chassis.h"
#include "Driver_Beltraise.h"
#include "Driver_GuideWheel.h"
#include "Driver_Sensor.h"
#include "Driver_Manipulator.h"

#include "CanBusTask.h"
#include "StatusMachine.h"

uint32_t time_tick_1ms = 0;
void ControlLoopTaskInit(void)
{
	time_tick_1ms = 0;
	StatusMachine_Init();
}
void ControlTask(void)
{
//	static uint32_t tick = 0;����֮ǰ��д������һЩ����
	time_tick_1ms++;

	InfraredSensor_StateGet();
	if(time_tick_1ms%4==0)
	{
		Chassis_Control();
	}
	Manipulator_Control();
	GuideWheel_Control();
	Belt_Control();
	
}



void Drivers_Control_Task(void const * argument)
{
	portTickType xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	ControlLoopTaskInit();
  /* Infinite loop */
  for(;;)
  {

			
			ControlTask();
			
			vTaskDelayUntil(&xLastWakeTime,1/portTICK_RATE_MS);//��ʱ��������̬

	

		
		
		
		
  }
  /* USER CODE END Can_Send_Task */
}
