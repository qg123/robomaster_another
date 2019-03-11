#include "Driver_GuideWheel.h"
#include "BSP_TIM.h"
#include "Driver_Remote.h"
#include "Driver_Beltraise.h"
#include "Driver_Sensor.h"
#include "math.h"
#include "StatusMachine.h"
extern uint32_t count1;
extern uint32_t count2;

extern uint8_t Foward_C_flag;
extern uint8_t Back_C_flag;
uint8_t Foward_G_flag = 0;
uint8_t Back_GW_flag = 0;
uint8_t GW_direction;
uint16_t GW_Speed;
GuideWheelModeTypeDef GuideWheelMode;

void MotorInit(void)
{
	MotorStart();//�ر�ɲ��ͣ��
	HAL_GPIO_WritePin(GPIOF,GPIO_PIN_1,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOF,GPIO_PIN_0,GPIO_PIN_RESET);//ʹ������
}
void MotorBackInit(void)
{
	HAL_GPIO_WritePin(GPIOF,GPIO_PIN_1,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOF,GPIO_PIN_0,GPIO_PIN_SET);//ʹ�ܷ���
}

void GuideWheel_Move_Advance(void)
{
	MotorStart();
	__HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_1,50);
	__HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_2,50);
	if((count1+count2)>2000)//̧��ʱȫ�õ���
	{//19*6*2*nȦ = 2*�ź���*����(��λ�ź� = 8000)
		count1 = count2 = 0;
		MotorStop();
		GuideWheelMode = GuideWheelMove_Stop;
	}//һ�����ľ���
	if(UpIslandState == Up_Island_GuideWheelAdvance_First)
	{
		UpIslandState = Up_Island_BeltUp_First;
	}
	else if(UpIslandState == Up_Island_GuideWheelAdvance_Twice)
	{
		UpIslandState = Up_Island_BeltUp_Twice;
	}
}

void GuideWheel_Move_Back(void)
{
	MotorStart();
	MotorBackInit();
	__HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_1,50);
	__HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_2,50);
	if(InfraredState_back == 1 && InfraredState_front == 1)//�󷽴�������⵽����
	{
		if(DownIslandState == Down_Island_ChassisBack_First)
		{
			DownIslandState = Down_Island_BeltUp_First;
		}
		else if(DownIslandState == Down_Island_ChassisBack_Twice)
		{
			DownIslandState = Down_Island_BeltUp_Twice;
		}
	}
}

extern uint8_t steps;
extern uint8_t steps_down;
extern uint8_t BM_AngelGet;
void MotorSpeedSet(void)
{
	if(RC_CtrlData.rc.s2 == 3)
	{
		MotorStart();
		if(RC_CtrlData.rc.ch1 > 0)
		{
			MotorInit();
		}
		else if(RC_CtrlData.rc.ch1 < 0)
		{
			MotorBackInit();
		}
		GW_Speed = 100-(abs(RC_CtrlData.rc.ch1)/7.33);//ռ�ձ�10-100��0ʱ����ֹͣ660/90=7.33
		__HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_1,GW_Speed);
		__HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_2,GW_Speed);
	}
		 
	if(Foward_G_flag == 1)
		{
			MotorStart();
			__HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_1,50);
			__HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_2,50);
			if((count1+count2)>2000)//̧��ʱȫ�õ���
			{//19*6*2*nȦ = 2*�ź���*����(��λ�ź� = 8000)
				count1 = count2 = 0;
				MotorStop();
				Foward_G_flag =0;
				steps++;
			}//һ�����ľ���
		}
			else if(Back_GW_flag == 1)
			{
				MotorStart();
				MotorBackInit();
				__HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_1,50);
				__HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_2,50);
				if(InfraredState_back == 1 && InfraredState_front == 1)//�󷽴�������⵽����
				{
					Back_GW_flag = 0;
					MotorStop();
					BM_AngelGet = 1;
					steps_down++;
				}
			}
			else
			{
				MotorStop();
			}
		
}

void MotorSpeedSet_SM(void)
{
	switch (GuideWheelMode)
	{
		case Normal_Rc_GuideWheelMove:
		{
			MotorStart();
			if(RC_CtrlData.rc.ch1 > 0)
			{
				MotorInit();
			}
			else if(RC_CtrlData.rc.ch1 < 0)
			{
				MotorBackInit();
			}
			GW_Speed = 100-(abs(RC_CtrlData.rc.ch1)/7.33);//ռ�ձ�10-100��0ʱ����ֹͣ660/90=7.33
			__HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_1,GW_Speed);
			__HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_2,GW_Speed);
		}break;//��ʱң����ң�ص��ֵ�bug��û�иĹ���

		case GuideWheel_Advance:
		{
			GuideWheel_Move_Advance();
		}break;

		case GuideWheel_Back:
		{
			GuideWheel_Move_Back();
		}break;

		case GuideWheelMove_Stop:
		{
			MotorStop();
		}break;

		default:
		{
			MotorStop();
		}break;
	}
}
void GuideWheel_Control(void)
{
	MotorSpeedSet();
}

	

