#include "Driver_Beltraise.h"
#include "StatusMachine.h"
#include "DriverLib_PID.h"
#include "CanBusTask.h"
#include "Driver_Remote.h"
#include "Driver_Sensor.h"

int16_t BeltMotorSpeedRef[2] = {0,0};

BeltModeTypeDef BeltMode;
int bmkp = 60;
extern uint8_t S_switch;
uint16_t raise_speed = 300;
extern uint8_t Foward_G_flag;
uint8_t steps = 0;
uint8_t steps_down = 0;
uint8_t BM_AngelGet = 0;
extern uint8_t Foward_C_flag;
extern uint8_t Back_C_flag;
extern uint8_t Back_GW_flag;
extern uint32_t tick_c_1;

void Belt_Move_Up(void)
{
	static int32_t LBM_Angle = 0;
	static int32_t RBM_Angle = 0;
			
	BeltMotorSpeedRef[0] = -raise_speed;//250
	BeltMotorSpeedRef[1] = raise_speed;
	if(BM_AngelGet == 1)
	{
		LBM_Angle = LBeltM_Measure.ecd_angle;
		RBM_Angle = RBeltM_Measure.ecd_angle;
		BM_AngelGet = 0;
	}
	if(abs(LBeltM_Measure.ecd_angle - LBM_Angle) > THRESHOLD)//9000
	{
		BeltMotorSpeedRef[0] = 0;
		BeltMotorSpeedRef[1] = 0;
		if(UpIslandState == Up_Island_BeltUp_First) 
		{
			UpIslandState = Up_Island_ChassisAdvance_First;
		}
		else if(UpIslandState == Up_Island_BeltUp_Twice) 
		{
			UpIslandState = Up_Island_Stop;
		}
		else if(DownIslandState == Down_Island_BeltUp_First)
		{
			DownIslandState = Down_Island_ChassisBack_Twice;
		}
		else if(DownIslandState == Down_Island_BeltUp_Twice)
		{
			DownIslandState = Down_Island_stop; 
		}
	}
}

void Belt_Move_Down(void)
{
	static int32_t LBM_Angle = 0;
	static int32_t RBM_Angle = 0;
	BeltMotorSpeedRef[0] = raise_speed;//250
	BeltMotorSpeedRef[1] = -raise_speed;
	if(BM_AngelGet == 1)
	{
		LBM_Angle = LBeltM_Measure.ecd_angle;
		RBM_Angle = RBeltM_Measure.ecd_angle;
		BM_AngelGet = 0;
	}
	if(abs(LBeltM_Measure.ecd_angle - LBM_Angle) > THRESHOLD)//9000
	{
		BeltMotorSpeedRef[0] = 100;
		BeltMotorSpeedRef[1] = -100;
		if (UpIslandState == Up_Island_BeltDown_First) 
		{
			UpIslandState = Up_Island_GuideWheelAdvance_First;
		}
		else if(UpIslandState == Up_Island_BeltDown_Twice)
		{
			UpIslandState = Up_Island_GuideWheelAdvance_Twice;
		}
		else if(DownIslandState == Down_Island_BeltDown_First)
		{
			DownIslandState = Down_Island_GuideWheelBack_First;
		}
		else if(DownIslandState == Down_Island_BeltDown_Twice)
		{
			DownIslandState = Down_Island_GuideWheelBack_Twice;
		}
		
	}
}

//����Ҫ��һ���������̴����ôд�ˡ�
void BM_Get_PID(void)
{

	RBMSpeedPID.kp = bmkp;//60
	RBMSpeedPID.ki = 0;
	RBMSpeedPID.kd = 0;//
	
	
	LBMSpeedPID.kp = bmkp;//60
	LBMSpeedPID.ki = 0;
	LBMSpeedPID.kd = 0;//

}


void 	BM_Calc_Output(void)
{
	PID_Task(&LBMSpeedPID,BeltMotorSpeedRef[0],LBeltM_Measure.speed_rpm/10.0);//float /10.0
	PID_Task(&RBMSpeedPID,BeltMotorSpeedRef[1],RBeltM_Measure.speed_rpm/10.0);

	if(LBMSpeedPID.output>-800 && LBMSpeedPID.output<800 ) LBMSpeedPID.output=0;
	if(RBMSpeedPID.output>-800 && RBMSpeedPID.output<800 ) RBMSpeedPID.output=0;


}
//��Ҫ�ģ�
void BM_Get_SpeedRef(void)
{	
	if(RC_CtrlData.rc.s2 == 3)//ֻ���ڲ���2Ϊ�м�ֵʱ����ͨ������1̧��,����1Ϊ3ʱң��������̧����1ʱ�ϵ����̣�2ʱ�µ�
	{													//����2Ϊ3ʱ,����1��������ͬ����;Ϊ1ʱ���������ƻ�е��
		if(RC_CtrlData.rc.s1 == 3)
		{
			BeltMotorSpeedRef[0] = RC_CtrlData.rc.ch3/2;
			BeltMotorSpeedRef[1] = -RC_CtrlData.rc.ch3/2;
		}
		else if(RC_CtrlData.rc.s1 == 1)//����
		{	
			static int32_t LBM_Angle = 0;
			static int32_t RBM_Angle = 0;
			if(steps == 0)
			{
				BeltMotorSpeedRef[0] = raise_speed;//250
				BeltMotorSpeedRef[1] = -raise_speed;
			
				static uint16_t count = 0;
				if(S_switch == 1)
				{
					count++;
				}
				if(count>13)//1000/72
				{
					LBM_Angle = LBeltM_Measure.ecd_angle;
					RBM_Angle = RBeltM_Measure.ecd_angle;
					count = 0;
				}
				if(abs(LBeltM_Measure.ecd_angle - LBM_Angle) > THRESHOLD)//9000
				{
					static int ii = 0;
					BeltMotorSpeedRef[0] = 100;
					BeltMotorSpeedRef[1] = -100;//120Ҳ��
					ii++;
					if(ii == 1)
					{
						Foward_G_flag = 1;
					}
				}
			}
			else if(steps == 1)
			{
				BeltMotorSpeedRef[0] = -raise_speed;
				BeltMotorSpeedRef[1] = raise_speed;
			static int ii = 0;
				if(((RBeltM_Measure.ecd_angle<(RBM_Angle-1200))&&(LBeltM_Measure.ecd_angle>(LBM_Angle+1200)))||(ii >= 1))
				{	
					ii++;//���ii���µ�������ٽ紦,���¶���//˳��ѱ�־λ�仯
					if(ii == 1)
					{
						Foward_C_flag = 1;
						tick_c_1 = xTaskGetTickCount();
					}
					BeltMotorSpeedRef[0] = 0;
					BeltMotorSpeedRef[1] = 0;
				}
				
				
				if(Foward_C_flag == 0 &&(tick_c_1>2000))//����о����������
				{
					BeltMotorSpeedRef[0] = raise_speed;//250
					BeltMotorSpeedRef[1] = -raise_speed;
					if(abs(LBeltM_Measure.ecd_angle - LBM_Angle) > THRESHOLD)//9000
					{
						BeltMotorSpeedRef[0] = 100;
						BeltMotorSpeedRef[1] = -100;//120Ҳ��
						static int aa =0 ;
						aa++;
						if(aa == 1)
						{
							Foward_G_flag = 1;
					
						}
					}
				}
				}
				else if(steps == 2)
				{
					BeltMotorSpeedRef[0] = -raise_speed;
					BeltMotorSpeedRef[1] = raise_speed;
					static int ii = 0;
					if(((RBeltM_Measure.ecd_angle<RBM_Angle)&&(LBeltM_Measure.ecd_angle>LBM_Angle))||ii >=1)
					{
						ii++;
						//finish
						BeltMotorSpeedRef[0] = 0;
						BeltMotorSpeedRef[1] = 0;
					}
				}
		}
		else if(RC_CtrlData.rc.s1 == 2)
		{
			static int32_t LBM_Angle = 0;
			static int32_t RBM_Angle = 0;
			if(steps_down == 0)
			{
				Back_C_flag = 1;
			}
			else if(steps_down == 1)
			{
				BeltMotorSpeedRef[0] = raise_speed;//250
				BeltMotorSpeedRef[1] = -raise_speed;
				if(BM_AngelGet == 1)
				{
					LBM_Angle = LBeltM_Measure.ecd_angle;
					RBM_Angle = RBeltM_Measure.ecd_angle;
					BM_AngelGet = 0;
				}
				if(abs(LBeltM_Measure.ecd_angle - LBM_Angle) > THRESHOLD)//9000
				{
					BeltMotorSpeedRef[0] = 100;
					BeltMotorSpeedRef[1] = -100;
					Back_GW_flag = 1;
				}
			}
			else if(steps_down == 2)
			{
				BeltMotorSpeedRef[0] = -raise_speed;//250
				BeltMotorSpeedRef[1] = raise_speed;
				if(BM_AngelGet == 1)
				{
					LBM_Angle = LBeltM_Measure.ecd_angle;
					RBM_Angle = RBeltM_Measure.ecd_angle;
					BM_AngelGet = 0;
				}
				if(abs(LBeltM_Measure.ecd_angle - LBM_Angle) > THRESHOLD)//9000
				{
					BeltMotorSpeedRef[0] = 0;
					BeltMotorSpeedRef[1] = 0;
					Back_C_flag = 1;
				}
			}
			else if(steps_down == 3)
			{
				BeltMotorSpeedRef[0] = raise_speed;//250
				BeltMotorSpeedRef[1] = -raise_speed;
				if(BM_AngelGet == 1)
				{
					LBM_Angle = LBeltM_Measure.ecd_angle;
					RBM_Angle = RBeltM_Measure.ecd_angle;
					BM_AngelGet = 0;
				}
				if(abs(LBeltM_Measure.ecd_angle - LBM_Angle) > THRESHOLD)//9000
				{
					BeltMotorSpeedRef[0] = 100;
					BeltMotorSpeedRef[1] = -100;
					Back_GW_flag = 1;
				}
			}
			else if(steps_down == 4)
			{
				BeltMotorSpeedRef[0] = -raise_speed;//250
				BeltMotorSpeedRef[1] = raise_speed;
				if(BM_AngelGet == 1)
				{
					LBM_Angle = LBeltM_Measure.ecd_angle;
					RBM_Angle = RBeltM_Measure.ecd_angle;
					BM_AngelGet = 0;
				}
				if(abs(LBeltM_Measure.ecd_angle - LBM_Angle) > THRESHOLD)//9000
				{
					BeltMotorSpeedRef[0] = 0;
					BeltMotorSpeedRef[1] = 0;
	//				Back_C_flag = 1;
				}
			}
	//		steps = 0;
	//		Foward_G_flag = 0;
	//		Foward_C_flag = 0;
	//		BeltMotorSpeedRef[0] = 0;
	//		BeltMotorSpeedRef[1] = -0;
		}
	}
	else
	{
		steps = 0;
		Foward_G_flag = 0;
		Foward_C_flag = 0;
		BeltMotorSpeedRef[0] = 0;
		BeltMotorSpeedRef[1] = -0;
	}
}


void BM_Get_SpeedRef_SM(void)
{
	switch (BeltMode)
	{
		case Normal_Rc_BeltMove:
		{
			BeltMotorSpeedRef[0] = RC_CtrlData.rc.ch3/2;
			BeltMotorSpeedRef[1] = -RC_CtrlData.rc.ch3/2;
		}break;
	
		//case Normal_Key_BeltMove{}�Ժ���д

		case Belt_Up:
		{
			Belt_Move_Up();
		}break;

		case Belt_Down:
		{
			Belt_Move_Down();
		}break;

		case BeltMove_Stop:
		{
			BeltMotorSpeedRef[0] = 0;
			BeltMotorSpeedRef[1] = 0;
		}break;

		default:
		{
			BeltMotorSpeedRef[0] = 0;
			BeltMotorSpeedRef[1] = 0;
		}break;
	}
}


void BM_Set_Current(void)
{
		if(RC_CtrlData.rc.s2 == 2)
		{
			Can_Send_BM(0,0,0,0);
		}
		else
		{
		
			Can_Send_BM(RBMSpeedPID.output*1.5,LBMSpeedPID.output*1.5,0,0);//��Ҫ������output�Ӹ���
	//		Can_Send_BM(-LBMSpeedPID.output,RBMSpeedPID.output,0,0);//���������������⣬��������ʲô�ĸ��������
		}



}


void Belt_Control(void)
{
	BM_Get_PID();
	BM_Get_SpeedRef();
	BM_Calc_Output();
	BM_Set_Current();

}




















