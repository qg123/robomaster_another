#include "Driver_Chassis.h"
#include "CanBusTask.h"
#include "DriverLib_PID.h"
#include "Driver_Remote.h"
#include "math.h"
#include "config.h"
#include "Driver_Sensor.h"
#include "StatusMachine.h"

#define M_PI  (float)3.1415926535f
#define Ang2Rad(m)  (m/180.0f*M_PI)

ChassisDataTypeDef ChassisData;
ChassisModeTypeDef ChassisMode;

uint8_t Foward_C_flag = 0;
uint8_t Back_C_flag = 0;
extern uint8_t BM_AngelGet;
extern uint8_t steps_down;

void CM_Get_PID(void)
{
	CMRotatePID.kp =15;

	CM1SpeedPID.kp = CM2SpeedPID.kp  = CM3SpeedPID.kp = CM4SpeedPID.kp = 60;//60
	CM1SpeedPID.ki = CM2SpeedPID.ki = CM3SpeedPID.ki = CM4SpeedPID.ki = 0;
	CM1SpeedPID.kd = CM2SpeedPID.kd = CM3SpeedPID.kd = CM4SpeedPID.kd = 0;//

	CM1SpeedPID.err[2] =  CM2SpeedPID.err[2] = CM3SpeedPID.err[2] = CM4SpeedPID.err[2] = 0;
	CM1SpeedPID.ki = CM2SpeedPID.ki = CM3SpeedPID.ki = CM4SpeedPID.ki = 0;

}




void 	CM_Calc_Output(void)
{
	PID_Task(&CM1SpeedPID,ChassisData.ChassisWheelSpeedRef[0],Motor1_Measure.speed_rpm/10.0);//float /10.0
	PID_Task(&CM2SpeedPID,ChassisData.ChassisWheelSpeedRef[1],Motor2_Measure.speed_rpm/10.0);
	PID_Task(&CM3SpeedPID,ChassisData.ChassisWheelSpeedRef[2],Motor3_Measure.speed_rpm/10.0);
	PID_Task(&CM4SpeedPID,ChassisData.ChassisWheelSpeedRef[3],Motor4_Measure.speed_rpm/10.0);


	if(CM1SpeedPID.output>-700 && CM1SpeedPID.output<700 ) CM1SpeedPID.output=0;//���������ǲ��ǾͲ��ü�ki�Ϳ���ͣ��б������
	if(CM2SpeedPID.output>-700 && CM2SpeedPID.output<700 ) CM2SpeedPID.output=0;
	if(CM3SpeedPID.output>-700 && CM3SpeedPID.output<700 ) CM3SpeedPID.output=0;
	if(CM4SpeedPID.output>-700 && CM4SpeedPID.output<700 ) CM4SpeedPID.output=0;
	
}



/**
  * @brief  ����3���ٶ�ֵ�������ӵ��ٶ�
  * @param  float vx X�����ϵ��ٶ�
  * @param  float vy Y�����ϵ��ٶ�
  * @param  float omega ��ת���ٶ�
  * @param  float radian ��ʱ���������ĽǶ�(����)��˳ʱ��Ϊ����
  * @param  int16_t maxspped  ����ٶ�
  * @retval None
  */
static void CalculateWheelSpeed(float vx, float vy, float omega, float radian, uint16_t maxspeed)//
{
	float   fWheelSpd[5];
	float	Chassis_forward_back_ref;		//��ս������ϵ�Ĳο��ٶ�
	float	Chassis_left_right_ref;
	float 	fMaxSpd = 0;
	int16_t s16_WheelSpd[5];
	Chassis_forward_back_ref = vy*cos(radian)+vx*sin(radian);		
	Chassis_left_right_ref   = vx*cos(radian)-vy*sin(radian);
	//���ֽ���
	fWheelSpd[0] = -Chassis_forward_back_ref + Chassis_left_right_ref + omega;	//���Ͻǵ����ʼ��ʱ����
	fWheelSpd[1] =  Chassis_forward_back_ref + Chassis_left_right_ref + omega;
	fWheelSpd[2] =  Chassis_forward_back_ref - Chassis_left_right_ref + omega;
	fWheelSpd[3] = -Chassis_forward_back_ref - Chassis_left_right_ref + omega;

	  
	//���� �ҵ��ٶ����ֵ
	fMaxSpd = fabs(fWheelSpd[0]);		
	if(fabs(fWheelSpd[1]) > fMaxSpd)
		fMaxSpd = fabs(fWheelSpd[1]);
	if(fabs(fWheelSpd[2]) > fMaxSpd)
		fMaxSpd = fabs(fWheelSpd[2]);
	if(fabs(fWheelSpd[3]) > fMaxSpd)
		fMaxSpd = fabs(fWheelSpd[3]);
  
	//�����������ٶ�
	if(fMaxSpd > maxspeed)
	{
		s16_WheelSpd[0]   = (int16_t)(fWheelSpd[0]*(maxspeed/fMaxSpd));
		s16_WheelSpd[1]   = (int16_t)(fWheelSpd[1]*(maxspeed/fMaxSpd));
		s16_WheelSpd[2]   = (int16_t)(fWheelSpd[2]*(maxspeed/fMaxSpd));
		s16_WheelSpd[3]   = (int16_t)(fWheelSpd[3]*(maxspeed/fMaxSpd));
	}
	else
	{
		s16_WheelSpd[0]   = (int16_t)fWheelSpd[0];
		s16_WheelSpd[1]   = (int16_t)fWheelSpd[1];
		s16_WheelSpd[2]   = (int16_t)fWheelSpd[2];
		s16_WheelSpd[3]   = (int16_t)fWheelSpd[3];
	}
	memcpy((void*)ChassisData.ChassisWheelSpeedRef, (void*)s16_WheelSpd, 8);
}



uint32_t tick_c_1 = 0;

uint16_t underpan_acc = 30;

void CM_Get_SpeedRef(void)
{
		if(RC_CtrlData.rc.s2 == 1||RC_CtrlData.rc.s2 == 3)
		{
			ChassisData.ChassisSpeedRef.Y = RC_CtrlData.rc.ch1;
			ChassisData.ChassisSpeedRef.X = RC_CtrlData.rc.ch0;
			ChassisData.ChassisSpeedRef.Omega  = RC_CtrlData.rc.ch2/2;
			ChassisData.ChassisAngle = 0;//GMYawEncoder.ecd_angle;//���棬���angle�����
			
			static int16_t Y_temp = 0;
			static int16_t X_temp = 0;
			//����ң�������ٶ�
			if(abs(RC_CtrlData.rc.ch1 - Y_temp) > underpan_acc)
			{
				if(RC_CtrlData.rc.ch1 >Y_temp && RC_CtrlData.rc.ch1 > 0)
					Y_temp+=underpan_acc;
				else if (RC_CtrlData.rc.ch1 <Y_temp && RC_CtrlData.rc.ch1 < 0)
					Y_temp-=underpan_acc;	
				else
					Y_temp = RC_CtrlData.rc.ch1;
			}
			else
			{
				Y_temp = RC_CtrlData.rc.ch1;
			}
			
			if(abs(RC_CtrlData.rc.ch0 - X_temp) > underpan_acc)
			{
				if(RC_CtrlData.rc.ch0 >X_temp && RC_CtrlData.rc.ch0 > 0)
					X_temp+=underpan_acc;
				else if (RC_CtrlData.rc.ch0 <X_temp && RC_CtrlData.rc.ch0 < 0)
					X_temp-=underpan_acc;	
				else
					X_temp = RC_CtrlData.rc.ch0;
			}
			else
			{
				X_temp = RC_CtrlData.rc.ch0;
			}
			ChassisData.ChassisSpeedRef.Y		= -Y_temp * 1;
			ChassisData.ChassisSpeedRef.X   	= X_temp * 1; 
			
			
			
				if(RC_CtrlData.rc.s1 == 1)
		{
			if(Foward_C_flag == 1)
			{
				ChassisData.ChassisSpeedRef.Y = -250;//
				if(xTaskGetTickCount() - tick_c_1 > 1000)//2000/3000
				{
					Foward_C_flag = 0;
				}
			}
			else 
						ChassisData.ChassisSpeedRef.Y = -0;//

		}
		else if(RC_CtrlData.rc.s1 == 2)
		{
			if(Back_C_flag==1)
			{
				ChassisData.ChassisSpeedRef.Y = 100;
				if(InfraredState_back == 1 && InfraredState_front == 0)//�󷽴�������⵽����
				{
					Back_C_flag = 0;
					BM_AngelGet = 1;
					steps_down++;
				}
			}
			else
			{
				ChassisData.ChassisSpeedRef.Y = 0;
			}
		}
		}
		else
		{
			ChassisData.ChassisSpeedRef.Y = 0;
			ChassisData.ChassisSpeedRef.X = 0;
			ChassisData.ChassisSpeedRef.Omega  = 0;
			ChassisData.ChassisAngle = 0;
		
		}
		
		
	
		CalculateWheelSpeed(ChassisData.ChassisSpeedRef.X,\
											ChassisData.ChassisSpeedRef.Y,\
											ChassisData.ChassisSpeedRef.Omega,\
											Ang2Rad(ChassisData.ChassisAngle),30000);
}


void CM_Get_SpeedRef_SM(void)
{
	switch (ChassisMode)
	{
		case Normal_Rc_ChassisMove:
		{
			ChassisData.ChassisSpeedRef.Y = RC_CtrlData.rc.ch1;
			ChassisData.ChassisSpeedRef.X = RC_CtrlData.rc.ch0;
			ChassisData.ChassisSpeedRef.Omega  = RC_CtrlData.rc.ch2/2;
			ChassisData.ChassisAngle = 0;//GMYawEncoder.ecd_angle;//���棬���angle�����
			
			static int16_t Y_temp = 0;
			static int16_t X_temp = 0;
			//����ң�������ٶ�
			if(abs(RC_CtrlData.rc.ch1 - Y_temp) > underpan_acc)
			{
				if(RC_CtrlData.rc.ch1 >Y_temp && RC_CtrlData.rc.ch1 > 0)
					Y_temp+=underpan_acc;
				else if (RC_CtrlData.rc.ch1 <Y_temp && RC_CtrlData.rc.ch1 < 0)
					Y_temp-=underpan_acc;	
				else
					Y_temp = RC_CtrlData.rc.ch1;
			}
			else
			{
				Y_temp = RC_CtrlData.rc.ch1;
			}
			
			if(abs(RC_CtrlData.rc.ch0 - X_temp) > underpan_acc)
			{
				if(RC_CtrlData.rc.ch0 >X_temp && RC_CtrlData.rc.ch0 > 0)
					X_temp+=underpan_acc;
				else if (RC_CtrlData.rc.ch0 <X_temp && RC_CtrlData.rc.ch0 < 0)
					X_temp-=underpan_acc;	
				else
					X_temp = RC_CtrlData.rc.ch0;
			}
			else
			{
				X_temp = RC_CtrlData.rc.ch0;
			}
			ChassisData.ChassisSpeedRef.Y		= -Y_temp * 1;
			ChassisData.ChassisSpeedRef.X   	= X_temp * 1; 
		}break;

		//case Normal_Key_ChassisMove:{}�Ժ�д

		case Chassis_Advance:
		{
			ChassisData.ChassisSpeedRef.Y = -250;//
			if(xTaskGetTickCount() - tick_c_1 > 1000)//2000/3000
			{
				ChassisMode = ChassisMove_Stop;
				if(UpIslandState == Up_Island_ChassisAdvance_First)
				{
					UpIslandState = Up_Island_BeltDown_Twice;
				}
			}
		}break;

		case Chassis_Back:
		{
			ChassisData.ChassisSpeedRef.Y = 100;
			if(InfraredState_back == 1 && InfraredState_front == 0)//�󷽴�������⵽����
			{
				if(DownIslandState == Down_Island_ChassisBack_First)
				{
					DownIslandState = Down_Island_BeltDown_First;
				}
				else if(DownIslandState == Down_Island_ChassisBack_First)
				{
					DownIslandState = Down_Island_BeltDown_Twice;
				}
			}
		}break;

		case ChassisMove_Stop:
		{
			ChassisData.ChassisSpeedRef.Y = 0;
			ChassisData.ChassisSpeedRef.X = 0;
			ChassisData.ChassisSpeedRef.Omega  = 0;
			ChassisData.ChassisAngle = 0;
		}
	
		default:
		{
			ChassisData.ChassisSpeedRef.Y = 0;
			ChassisData.ChassisSpeedRef.X = 0;
			ChassisData.ChassisSpeedRef.Omega  = 0;
			ChassisData.ChassisAngle = 0;
		}break;
	}
	
	CalculateWheelSpeed(ChassisData.ChassisSpeedRef.X,\
						ChassisData.ChassisSpeedRef.Y,\
						ChassisData.ChassisSpeedRef.Omega,\
						Ang2Rad(ChassisData.ChassisAngle),30000);
}

void CM_Set_Current(void)
{
		if(RC_CtrlData.rc.s2 == 2)
		{
		 Can_Send_CM(0,0,0,0);
		}
		else
		Can_Send_CM( CHASSIS_SPEED_ATTENUATION * CM1SpeedPID.output, \
								 CHASSIS_SPEED_ATTENUATION * CM2SpeedPID.output, \
								 CHASSIS_SPEED_ATTENUATION * CM3SpeedPID.output, \
								 CHASSIS_SPEED_ATTENUATION * CM4SpeedPID.output);		

}





void Chassis_Control(void)
{
	CM_Get_PID();
	CM_Get_SpeedRef();
	CM_Calc_Output();
	CM_Set_Current();

}



