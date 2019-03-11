#include "main.h"
#include "SuperviseTask.h"
#include "cmsis_os.h"
#include "task.h"

uint32_t *GetLostCounter(uint32_t index);
uint16_t GetLostCounter_max(uint32_t index);
uint32_t lost_err = 0;     //ÿһλ����һ������
uint32_t lost_counter[40] = {0};			//���Ź�����

Lost_Counter_Max lost_counter_max = {	
										.LOST_ERROR_RC_MAX 			= 20,
										.LOST_ERROR_MOTOR1_MAX 		= 500,
										.LOST_ERROR_MOTOR2_MAX 		= 500,
										.LOST_ERROR_MOTOR3_MAX 		= 500,
										.LOST_ERROR_MOTOR4_MAX  	= 500,
										.LOST_ERROR_MOTOR_LBELT_MAX = 500,
										.LOST_ERROR_MOTOR_RBELT_MAX = 500,
										.LOST_ERROR_MOTOR_ARM_MAX	= 500,
                    .LOST_ERROR_CAN_TX_MAX          = 1000,
                    .LOST_ERROR_CAN_RX_MAX          = 1000,
                                    };

//��־����λ
void Set_Error_Flag(uint32_t index)
{
	lost_err |= (uint32_t)(1<<index);
}

//���ô���λ
void Reset_Error_Flag(uint32_t index)
{
	lost_err &= (uint32_t)~(1<<index);
}                                    

//��ȡ�����ĵ�ַ
uint32_t *GetLostCounter(uint32_t index)
{
	return (uint32_t *)&lost_counter[index];
}
//��ȡ��������
uint16_t GetLostCounter_max(uint32_t index)
{
	uint16_t *pmax =(uint16_t *)(&lost_counter_max + index*8*sizeof(uint16_t));
	return *pmax;
}
//�Ƿ����
uint8_t Is_Lost_Error_Set(uint32_t index)
{
	if((lost_err &(1 << index)) != 0)return 1;
	else							return 0;
}

/****
uint8_t Is_Serious_Error()
{
	#if (XDRM_CONVERTER == 0)
	return Is_Lost_Error_Set(LOST_ERROR_IMU);
	
	#else
	return Is_Lost_Error_Set(LOST_ERROR_XDRMBLACK);

	#endif
}
��֪��2019��Ĺ��̻᲻���õ���������ע�͵�
****/

/**
  * @brief  ���ڼ���̵߳�ռ�����
  * @param  obj   Mode(THREAD_IN�����߳̿�ͷ THREAD_OUT���ڽ�β,����)
  * @retval None
  */
void ThreadMonitor(ThreadMonitor_t* obj,uint8_t Mode)
{
	uint32_t currentTime = xTaskGetTickCount();//18�경���õ����Լ�д��Get_Time_Micros()
	if(Mode == THREAD_IN)
	{
		obj->deltaTime = xTaskGetTickCount() - obj->previousTime;	//��������ϴ����е�ʱ��
		obj->previousTime = currentTime;						//�߳��ϴν����ʱ��
	}
	else if(Mode == THREAD_OUT)
	{
		obj->excutionTime = currentTime - obj->previousTime;	//�̵߳�ִ��ʱ��
	}	
}

uint32_t lost_err;
uint32_t lost_counter[40];


uint16_t DBUSFrameRate = 0;
uint16_t DBUSFrameCounter = 0;

uint16_t ChassisFrameRate[4] = {0};
uint16_t ChassisFrameCounter[4] = {0};

uint16_t LbeltFrameRate = 0;
uint16_t LbeltFrameCounter = 0;

uint16_t RbeltFrameRate = 0;
uint16_t RbeltFrameCounter = 0;

uint16_t MotorArmFrameRate = 0;
uint16_t MotorArmFrameCounter = 0;

uint16_t CAN_Send_FrameRate = 0;
uint16_t CAN_Send_FrameCounter = 0;

uint16_t CAN_Res_FrameRate = 0;
uint16_t CAN_Res_FrameCounter = 0;

uint16_t UART2FrameRate = 0;
uint16_t UART2FrameCounter = 0;

portBASE_TYPE Control_Stack = 1;
portBASE_TYPE CANSend_Stack = 1;
portBASE_TYPE SuperviseTask_Stack = 1;

extern osThreadId Led_ToggleHandle;
extern osThreadId Can_SendHandle;
extern osThreadId Task_ControlHandle;
extern osThreadId Status_UpdateHandle;
extern osThreadId Superviese_TaskHandle;

void Task_Monitor(void)
{
	portTickType CurrentTaskMonitor_Tick = 0;
	while(1)
	{		
		osDelayUntil(&CurrentTaskMonitor_Tick, 500);
		//******************  ��   ����֡��ͳ��   ��  **************************/
			//DBUS֡��ͳ��
			DBUSFrameRate = DBUSFrameCounter*2;	
			DBUSFrameCounter = 0;
			
			//���̵��֡��ͳ��
			ChassisFrameRate[0] = ChassisFrameCounter[0]*2;
			ChassisFrameCounter[0] = 0;
			ChassisFrameRate[1] = ChassisFrameCounter[1]*2;
			ChassisFrameCounter[1] = 0;
			ChassisFrameRate[2] = ChassisFrameCounter[2]*2;
			ChassisFrameCounter[2] = 0;
			ChassisFrameRate[3] = ChassisFrameCounter[3]*2;
			ChassisFrameCounter[3] = 0;
			
            //�����ȵ��֡��ͳ��
			LbeltFrameRate = LbeltFrameCounter*2;
			LbeltFrameCounter = 0;
		
			RbeltFrameRate = RbeltFrameCounter*2;
			RbeltFrameCounter = 0;
			
            //��е�۵��֡��ͳ��
			MotorArmFrameRate = MotorArmFrameCounter*2;
			MotorArmFrameCounter = 0;
			
            //CAN�շ�֡��ͳ��
			CAN_Send_FrameRate = CAN_Send_FrameCounter*2;
			CAN_Send_FrameCounter = 0;

			CAN_Res_FrameRate = CAN_Res_FrameCounter*2;
			CAN_Res_FrameCounter = 0;
		
		//����֡�ʼ��
		UART2FrameRate = UART2FrameCounter*2;
		UART2FrameCounter = 0;
		
//---------------------------------------------------------------------		
		//ң����֡�ʹ���
		if(DBUSFrameRate < 10)
		{
			Set_Error_Flag(LOST_ERROR_RC);
		}
		else
		{
			Reset_Error_Flag(LOST_ERROR_RC);
		}
		//���̵��֡�ʹ���
		if(ChassisFrameRate[0] < 200)
		{
			Set_Error_Flag(LOST_ERROR_MOTOR1);
		}
		else
		{
			Reset_Error_Flag(LOST_ERROR_MOTOR1);
		}
		
		if(ChassisFrameRate[1] < 200)
		{
			Set_Error_Flag(LOST_ERROR_MOTOR2);
		}
		else
		{
			Reset_Error_Flag(LOST_ERROR_MOTOR2);
		}
		
		if(ChassisFrameRate[2] < 200)
		{
			Set_Error_Flag(LOST_ERROR_MOTOR3);
		}
		else
		{
			Reset_Error_Flag(LOST_ERROR_MOTOR3);
		}
		
		if(ChassisFrameRate[3] < 200)
		{
			Set_Error_Flag(LOST_ERROR_MOTOR4);
		}
		else
		{
			Reset_Error_Flag(LOST_ERROR_MOTOR4);
		}
		
        //�����ȵ��֡�ʹ���
		if(RbeltFrameRate < 200)
		{
			Set_Error_Flag(LOST_ERROR_MOTOR4);
		}
		else
		{
			Reset_Error_Flag(LOST_ERROR_MOTOR4);
		}
		
		if(LbeltFrameRate < 200)
		{
			Set_Error_Flag(LOST_ERROR_MOTOR_LBELT);
		}
		else
		{
			Reset_Error_Flag(LOST_ERROR_MOTOR_RBELT);
		}
		
        //��е�۵��֡�ʹ���
		if(MotorArmFrameRate < 200)
		{
			Set_Error_Flag(LOST_ERROR_MOTOR_ARM);
		}
		else
		{
			Reset_Error_Flag(LOST_ERROR_MOTOR4);
		}
        //CAN�շ�֡�ʹ���
        if(CAN_Send_FrameRate < 400)
		{
			Set_Error_Flag(LOST_ERROR_CAN_TX);
		}
		else
		{
			Reset_Error_Flag(LOST_ERROR_CAN_TX);
		}

        if(CAN_Res_FrameRate < 400)
		{
			Set_Error_Flag(LOST_ERROR_CAN_RX);
		}
		else
		{
			Reset_Error_Flag(LOST_ERROR_CAN_RX);
		}
		
		
//-----------------------------------����ϵͳ��ջ���------------------------------------
		CANSend_Stack=uxTaskGetStackHighWaterMark(Can_SendHandle);
		Control_Stack=uxTaskGetStackHighWaterMark(Task_ControlHandle);
		SuperviseTask_Stack=uxTaskGetStackHighWaterMark(Superviese_TaskHandle);
	}
        
}		

void Superviese_Task(void const * argument)
{

  
	portTickType xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();

  /* Infinite loop */
  for(;;)
  {

			
			Task_Monitor();
			
			vTaskDelayUntil(&xLastWakeTime,1/portTICK_RATE_MS);//��ʱ��������̬

  }

}
		


