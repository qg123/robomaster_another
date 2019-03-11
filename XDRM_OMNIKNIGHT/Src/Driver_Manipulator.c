#include "Driver_Manipulator.h"
#include "CanBusTask.h"
#include "Driver_Remote.h"


#include "DriverLib_PID.h"



float ArmMotorAngleRef = 0;

void ARM_Get_PID(void)
{
	AMRotatePID.kp =60;
	AMRotatePID.ki =0;
	AMRotatePID.kd =0;

}
uint8_t AM_Stall_Flag = 0;
uint32_t	time_tick_arm = 0;
void ARM_Get_AngleRef(void)
{
	
	if(RC_CtrlData.rc.s2 == 1)
	{
		if(Action_stick == REMOTE_SWITCH_CHANGE_1TO3TO1TO3)//1-3-1-3Ϊȡ��һ��//�տ�ʼ��е����Ҫ̧����
		{
//			if(AM_Stall_Flag==0&&(xTaskGetTickCount() - time_tick_arm>3000)
//			{
//				ARM_PINCH;//��һ�°���Ū�ɼ����ŵ�
//				static uint16_t aaaab = 0;
//				aaaab++;
//				if(aaaab > 500)
//				{
//					ARM_RAISE;
//					ArmMotorAngleRef = TurntableEncoder.ecd_angle/19 - 120;
//					
//				}
//			
//			}
//			ArmMotorAngleRef = TurntableEncoder.ecd_angle/19 + 120;
//			time_tick_arm = xTaskGetTickCount();
			
		
		}
		else if(Action_stick == REMOTE_SWITCH_CHANGE_2TO3TO2TO3)//2-3-2-3Ϊ��������ط�ת
		{		
			static int sw_count = 0;
			if(sw_count++%2 == 0)//���Ҳ��̫���ʣ���ü���ת��Ƕȴ���
			{
				ArmMotorAngleRef = TurntableEncoder.ecd_angle/19 - 120;//���angle��ʱ��ͳһ�ĵ�
				time_tick_arm = xTaskGetTickCount();
				AM_Stall_Flag = 1;
			}
			else 
			{
				ArmMotorAngleRef = TurntableEncoder.ecd_angle/19 + 120;
				time_tick_arm = xTaskGetTickCount();
				AM_Stall_Flag = 1;

			}
				Action_stick = 0;
				stick_store = RC_CtrlData.rc.s1;
		}
		else if(Action_stick == REMOTE_SWITCH_CHANGE_1TO3TO2TO3)//1-3-2-3Ϊ̧�����׵������ƶ�
		{
		
		}
		else if(Action_stick == REMOTE_SWITCH_CHANGE_3TO2TO3TO2)//3-2-3-2Ϊ��ȡ���׵ļкͷ�
		{
		
		
		}
		
		
		if(xTaskGetTickCount() - time_tick_arm > 2000)//������õ���ʱ�䣬��ʵ��ת��Ƕ��ƺ�������
		{
			ArmMotorAngleRef = TurntableEncoder.ecd_angle/19;	//Ҳ���ǵ���0��Ҳ����outputΪ��
			AM_Stall_Flag = 0;
		}//��ֹ��ת
	}

}
void ARM_Calc_Output(void)
{
	PID_Task(&AMRotatePID,ArmMotorAngleRef,TurntableEncoder.ecd_angle/19);//������ٱ�
	if(AMRotatePID.output>-300 && AMRotatePID.output<300 ) AMRotatePID.output=0;
	

}

void ARM_Set_Current(void)
{
		if(RC_CtrlData.rc.s2 == 2)
		{
		 AMRotatePID.output = 0;
		}
		else
		{
		//���������ͬ������can��������˳�㷢��
		}


}



void ARM_Gas_Operate(void)
{
		if(Action_stick == REMOTE_SWITCH_CHANGE_1TO3TO1TO3)//1-3-1-3Ϊȡ��һ��
		{
		
		}
		else if(Action_stick == REMOTE_SWITCH_CHANGE_2TO3TO2TO3)//2-3-2-3Ϊ��������ط�ת
		{
		
		
		}
		else if(Action_stick == REMOTE_SWITCH_CHANGE_1TO3TO2TO3)//1-3-2-3Ϊ̧�����׵������ƶ�
		{
			if(HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_10)==RESET)
			{
				ARM_RAISE;
				
			}
			else
				ARM_DESCEND;

			Action_stick = 0;
			stick_store = RC_CtrlData.rc.s1;
		
		}
		else if(Action_stick == REMOTE_SWITCH_CHANGE_3TO2TO3TO2)//3-2-3-2Ϊ��ȡ���׵ļкͷ�
		{
		
			if(HAL_GPIO_ReadPin(GPIOI,GPIO_PIN_9)==RESET)
			{
				ARM_PINCH;
			}
			else 
			{
				ARM_UNPINCH;
			}
				
			Action_stick = 0;
			stick_store = RC_CtrlData.rc.s1;

		}

	



}



void ARM_Electric_Operate(void)
{
	ARM_Get_PID();
	ARM_Get_AngleRef();
	ARM_Calc_Output();
	ARM_Set_Current();
}



void Manipulator_Control(void)
{

	ARM_Gas_Operate();
	ARM_Electric_Operate();

}







