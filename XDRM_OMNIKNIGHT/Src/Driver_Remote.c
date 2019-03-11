#include "Driver_Remote.h"
#include "config.h"
#include "DriverLib_Ramp.h"
#include "Driver_Chassis.h"
#include "StatusMachine.h"

InputMode_e	InputMode;
RC_Ctrl RC_CtrlData;

uint8_t stick_store = 0;

uint8_t stick1_change(void)
{		
	stick_store<<= 2;
	stick_store |= RC_CtrlData.rc.s1;

		if(((stick_store>>2)&0x03) != RC_CtrlData.rc.s1)  //�����00000011�ĳ�0x03֮��Ϳ�����	
		{		
			return 1;
		}
		else
		{ 
			stick_store >>= 2;
			return 0;
		}
}

uint32_t timetick2 = 0;
uint32_t timetick1 = 0;
uint32_t time_diff_1to3 = 0;
uint32_t time_diff_1to2 = 0;
uint8_t S_switch = 0;

uint8_t Action_stick = 0;//stick0 �����α仯ֵ������㶨ֵ
uint8_t stick1_action(void)
{
	S_switch = stick1_change();
	if(S_switch != 0)
	{
	 
		time_diff_1to3 = xTaskGetTickCount() - timetick2;
		time_diff_1to2 = xTaskGetTickCount() - timetick1;
		
		
		timetick2 = timetick1;
		timetick1 = xTaskGetTickCount();

		if(time_diff_1to2<NORMAL_TIME_2)
		{
			if(time_diff_1to3<NORMAL_TIME_3)
			{
				
				Action_stick = stick_store;
				return THREE_CHANGE;//������һ��������Σ����α任�����Ҳ���Ѽǣ������Ըı䲦��λ�ã������ñ�3��֮��õ���8λ����Ϊ��������
			}
			return TWO_CHANGE;
		}
	 else 
	 {
		 
			return ONE_CHANGE;
	 }
	}
	else 
//		  abc=xTaskGetTickCount() - timetick1;
			if((xTaskGetTickCount() - timetick1)>NORMAL_TIME_2)
			{
				stick_store = RC_CtrlData.rc.s1;//��һ��ʱ����δ��⵽�ٴ����䣬��ֻ����һ�Σ�����ֵ���
			}
			return NO_CHANGE;
		
	
}


/**
  * @brief  �ж�һ�����̼��Ƿ�����������
  * @param  Ҫ�����жϵİ������ַ���д
  * @retval 1 ����        0 δ����
  */
uint8_t Remote_CheckJumpKey(uint16_t Key)
{
    if(RC_CtrlData.key.v & Key)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}




void RemoteDataProcess(uint8_t *pData)
{
    if(pData == NULL)
    {
        return;
    }
	
    RC_CtrlData.rc.ch0 = ((int16_t)pData[0] | ((int16_t)pData[1] << 8)) & 0x07FF; 
    RC_CtrlData.rc.ch1 = (((int16_t)pData[1] >> 3) | ((int16_t)pData[2] << 5)) & 0x07FF;
    RC_CtrlData.rc.ch2 = (((int16_t)pData[2] >> 6) | ((int16_t)pData[3] << 2) |
                         ((int16_t)pData[4] << 10)) & 0x07FF;
    RC_CtrlData.rc.ch3 = (((int16_t)pData[4] >> 1) | ((int16_t)pData[5]<<7)) & 0x07FF;
	
		RC_CtrlData.rc.ch0 -= (int16_t)REMOTE_CONTROLLER_STICK_OFFSET;
		RC_CtrlData.rc.ch1 -= (int16_t)REMOTE_CONTROLLER_STICK_OFFSET;
		RC_CtrlData.rc.ch2 -= (int16_t)REMOTE_CONTROLLER_STICK_OFFSET;
		RC_CtrlData.rc.ch3 -= (int16_t)REMOTE_CONTROLLER_STICK_OFFSET;
    
    RC_CtrlData.rc.s1 = ((pData[5] >> 4) & 0x000C) >> 2;
    RC_CtrlData.rc.s2 = ((pData[5] >> 4) & 0x0003);

    RC_CtrlData.mouse.x = ((int16_t)pData[6]) | ((int16_t)pData[7] << 8);
    RC_CtrlData.mouse.y = ((int16_t)pData[8]) | ((int16_t)pData[9] << 8);
    RC_CtrlData.mouse.z = ((int16_t)pData[10]) | ((int16_t)pData[11] << 8);    

    RC_CtrlData.mouse.press_l = pData[12];
    RC_CtrlData.mouse.press_r = pData[13];
 
    RC_CtrlData.key.v = ((int16_t)pData[14]) | ((int16_t)pData[15] << 8);
//		S_switch = stick1_change();
		stick1_action();
}









void RemoteTaskInit(void)
{
	//б�³�ʼ��
//	LRSpeedRamp.SetScale(&LRSpeedRamp, MOUSE_LR_RAMP_TICK_COUNT);
//	FBSpeedRamp.SetScale(&FBSpeedRamp, MOUSR_FB_RAMP_TICK_COUNT);
//	LRSpeedRamp.ResetCounter(&LRSpeedRamp);
//	FBSpeedRamp.ResetCounter(&FBSpeedRamp);

	ChassisData.ChassisSpeedRef.Y = 0.0f;
	ChassisData.ChassisSpeedRef.X = 0.0f;
	ChassisData.ChassisSpeedRef.Omega = 0.0f;

}




