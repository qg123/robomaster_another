
/* Includes ------------------------------------------------------------------*/

#include "DriverLib_PID.h"




PID_Regulator_t RBMSpeedPID = BELT_MOTOR_SPEED_PID_DEFAULT;
PID_Regulator_t LBMSpeedPID = BELT_MOTOR_SPEED_PID_DEFAULT;//λ�û�Ҫ��Ҫ���Ժ���˵
PID_Regulator_t CMRotatePID = CHASSIS_MOTOR_ROTATE_PID_DEFAULT; 
PID_Regulator_t CM1SpeedPID = CHASSIS_MOTOR_SPEED_PID_DEFAULT;
PID_Regulator_t CM2SpeedPID = CHASSIS_MOTOR_SPEED_PID_DEFAULT;
PID_Regulator_t CM3SpeedPID = CHASSIS_MOTOR_SPEED_PID_DEFAULT;
PID_Regulator_t CM4SpeedPID = CHASSIS_MOTOR_SPEED_PID_DEFAULT;
PID_Regulator_t AMRotatePID = CHASSIS_MOTOR_SPEED_PID_DEFAULT; 

#define NB -3
#define NM -2
#define NS -1
#define ZO 0
#define PS 1
#define PM 2
#define PB 3


int deltaKpMatrix[7][7]={{PB,PB,PM,PM,PS,ZO,ZO},
												 {PB,PB,PM,PS,PS,ZO,NS},
												 {PM,PM,PM,PS,ZO,NS,NS},
												 {PM,PM,PS,ZO,NS,NM,NM},
												 {PS,PS,ZO,NS,NS,NM,NM},
												 {PS,ZO,NS,NM,NM,NM,NB},
												 {ZO,ZO,NM,NM,NM,NB,NB}};

int deltaKiMatrix[7][7]={{NB,NB,NM,NM,NS,ZO,ZO},
												 {NB,NB,NM,NS,NS,ZO,ZO},
												 {NB,NM,NS,NS,ZO,PS,PS},
												 {NM,NM,NS,ZO,PS,PM,PM},
												 {NM,NS,ZO,PS,PS,PM,PB},
												 {ZO,ZO,PS,PS,PM,PB,PB},
												 {ZO,ZO,PS,PM,PM,PB,PB}};

int deltaKdMatrix[7][7]={{PS,NS,NB,NB,NB,NM,PS},
												 {PS,NS,NB,NM,NM,NS,ZO},
												 {ZO,NS,NM,NM,NS,NS,ZO},
												 {ZO,NS,NS,NS,NS,NS,ZO},
												 {ZO,ZO,ZO,ZO,ZO,ZO,ZO},
												 {PB,NS,PS,PS,PS,PS,PB},
												 {PB,PM,PM,PM,PS,PS,PB}};



float	EC = 0;
float	E = 0;
float E_Index = 0;
float EC_Index = 0;
float E_Max = 0;
float EC_Max = 0;
float E_Min = 0;
float EC_Min = 0;

												 
void PID_Reset(PID_Regulator_t *pid)
{
	pid->err[0]=0;//err0��Ϊ������
	pid->err[1]=0;//err1��Ϊ�������
	pid->err[2]=0;//err2��Ϊ�ϴ����
	pid->err[3]=0;//err2��Ϊ���ϴ����
	pid->output=0;
}





void PID_Calc(PID_Regulator_t *pid)
{
	
	pid->err[3]=pid->err[2];//ֻ��������ʽpid�в��õ������ϴ���err3
	pid->err[2]=pid->err[1];//�ϴ����
	pid->err[1]=pid->ref-pid->fdb;//�������
	pid->err[0]+=pid->err[1];//������
	//VAL_LIMIT(pid->ki,-pid->componentKiMax,pid->componentKiMax);	//�����ͻ���//���Ǵ�ģ�����������output�ı��������ki�ˣ�ɾ����
	if(pid->type == POSITION_PID)
	{
		pid->output_kp = pid->kp*pid->err[1];//��������
		pid->output_ki = pid->ki*pid->err[2];//���ֲ���
		pid->output_kd = pid->kd*(pid->err[1]-pid->err[0]);	//���һ�������΢�ֲ��ֵ����
		
		VAL_LIMIT(pid->output_kp,-pid->output_kpMax,pid->output_kpMax);
		VAL_LIMIT(pid->output_ki,-pid->output_kiMax,pid->output_kiMax);//һ����������������Ϊ�����ͻ���//���������Ӳ����Ժ�
		VAL_LIMIT(pid->output_kd,-pid->output_kdMax,pid->output_kdMax);
		pid->output = pid->output_kp + pid->output_ki + pid->output_kd;
	}
	else if(pid->type == DELTA_PID)
	{
		pid->output = pid->kp*(pid->err[1]-pid->err[2])+pid->ki*pid->err[1]+pid->kd*(pid->err[1] - 2*pid->err[2]+pid->err[3]);//����ʽpid��ֻʹ�����м��书�ܵ��������粽�������
	}
	else if(pid->type == VAGUE_PID)
	{

		static float kp_delta = 0;
		static float ki_delta = 0;
		static float kd_delta = 0;
		
		EC = (pid->err[2]-pid->err[1]);//���仯��,�����ĵ�λ�����/ms
		E = 	pid->err[1];
		
		if(EC > EC_Max)
		{
			EC_Max = EC;
		}
		if(E > E_Max)
		{
			E_Max = E;
		}
		
				
		if(EC < EC_Min)
		{
			EC_Min = EC;
		}
		if(E < E_Min)
		{
			E_Min = E;
		}
		
		
								//*6��Ϊ�˻���0-6������
//		E_Index = E*6/pid->refmax - (E*6/pid->refmax - 1);//����������,��һ����1-E_Index
//		EC_Index = EC*6/pid->ecmax - (EC*6/pid->ecmax - 1);
		
//		kp_delta = pid->kp * (deltaKpMatrix[(int8_t)(E*6/pid->refmax+3)][(int8_t)(EC*6/pid->ecmax+3)]*(1-E_Index)*(1-EC_Index)\
													 +deltaKpMatrix[(int8_t)(E*6/pid->refmax+2)][(int8_t)(EC*6/pid->ecmax+3)]*(EC_Index)*(1-E_Index)\
													 +deltaKpMatrix[(int8_t)(E*6/pid->refmax+3)][(int8_t)(EC*6/pid->ecmax+2)]*(1-EC_Index)*(E_Index)\
													 +deltaKpMatrix[(int8_t)(E*6/pid->refmax+2)][(int8_t)(EC*6/pid->ecmax+2)]*(EC_Index)*(E_Index);
		
																				 //���int8_t ǿ��ת���ɱ�������Ǹ�����
//		ki_delta = pid->kp * (deltaKiMatrix[(int8_t)(E*6/pid->refmax+3)][(int8_t)(EC*6/pid->ecmax+3)]*(1-E_Index)*(1-EC_Index)\
													 +deltaKiMatrix[(int8_t)(E*6/pid->refmax+2)][(int8_t)(EC*6/pid->ecmax+3)]*(EC_Index)*(1-E_Index)\
													 +deltaKiMatrix[(int8_t)(E*6/pid->refmax+3)][(int8_t)(EC*6/pid->ecmax+2)]*(1-EC_Index)*(E_Index)\
													 +deltaKiMatrix[(int8_t)(E*6/pid->refmax+2)][(int8_t)(EC*6/pid->ecmax+2)]*(EC_Index)*(E_Index);
													 
													 
//		kp_delta = pid->kp * (deltaKdMatrix[(int8_t)(E*6/pid->refmax+3)][(int8_t)(EC*6/pid->ecmax+3)]*(1-E_Index)*(1-EC_Index)\
													 +deltaKdMatrix[(int8_t)(E*6/pid->refmax+2)][(int8_t)(EC*6/pid->ecmax+3)]*(EC_Index)*(1-E_Index)\
													 +deltaKdMatrix[(int8_t)(E*6/pid->refmax+3)][(int8_t)(EC*6/pid->ecmax+2)]*(1-EC_Index)*(E_Index)\
													 +deltaKdMatrix[(int8_t)(E*6/pid->refmax+2)][(int8_t)(EC*6/pid->ecmax+2)]*(EC_Index)*(E_Index);
		
		pid->kp = pid->kp+kp_delta;
		pid->ki = pid->ki+ki_delta;
		pid->kd = pid->kd+kd_delta;

		pid->output_kp = pid->kp*pid->err[1];//��������
		pid->output_ki = pid->ki*pid->err[2];//���ֲ���
		pid->output_kd = pid->kd*(pid->err[1]-pid->err[0]);	//���һ�������΢�ֲ��ֵ����
		
		VAL_LIMIT(pid->output_kp,-pid->output_kpMax,pid->output_kpMax);
		VAL_LIMIT(pid->output_ki,-pid->output_kiMax,pid->output_kiMax);//һ����������������Ϊ�����ͻ���//���������Ӳ����Ժ�
		VAL_LIMIT(pid->output_kd,-pid->output_kdMax,pid->output_kdMax);
		pid->output = pid->output_kp + pid->output_ki + pid->output_kd;

		
	}
	else
	{//���Կ�����ʽ����
		
			
		if(EC > EC_Max)
		{
			EC_Max = EC;
		}
		if(E > E_Max)
		{
			E_Max = E;
		}
		
				
		if(EC < EC_Min)
		{
			EC_Min = EC;
		}
		if(E < E_Min)
		{
			E_Min = E;
		}
		
		
		if(pid->err[1] > pid->ref/10)//�о������е���
		{
			pid->err[0] = 0;
		}
//		else if(pid->err[1] > pid->ref/100)
//		{
//			pid->err[0] /= 2; 
//		}
		else
		{
			pid->index = (1-pid->err[1]/pid->ref);
			pid->err[0] *= pid->index;
		}
		if(pid->err[1]<0)
		{

			
			pid->err[1] = 0;//kp
		}
		
		//�����Զ�΢������ԸĽ���˼·��1.����ȫ΢���㷨��������˼�һ��һ�׵�ͨ�˲������÷�Ӧ����һ�㣩2.�Ը���ֵ΢�֣�������ƫ��ֵe(t),������û���˶�Ŀ��ֵ�仯��΢������
		
		
		
		
		
		pid->output=pid->kp*pid->err[1]+pid->ki*pid->err[2]+pid->kd*(pid->err[1]-pid->err[0]);
	
	}
	VAL_LIMIT(pid->output,-pid->outputMax,pid->outputMax);
}
/**
	* @brief PID����
	* @param PID_Regulator_t *PID_Stucture
	* @param float ref
	* @param float fdb
	* @retval float output
*/
float PID_Task(PID_Regulator_t *PID_Stucture, float ref, float fdb)
{
	PID_Stucture->ref = ref;
	PID_Stucture->fdb = fdb;
	PID_Stucture->Calc(PID_Stucture);
	return PID_Stucture->output;
}

/*
	ģ������
	kp*
	e	|										ec
		|	NB		NM		NS		ZO		PS		PM		PB
		|
	NB| PB		PB		PM		PB		PS		ZO		ZO
	NM|	PB		PB		PM		PS		PS		ZO		NS
	NS|	PM		PM		PM		PS		ZO		NS		NS
	ZO|	PM		PM		PS		ZO		BS		NM		NM
	PS|	PS		OS		ZO		NS		NS		NM		NM
	PM|	PS		ZO		NS		NM		NM		NM		NB
	PB|	ZO		ZO		NM		NM		NM		NB		NB

	ki*
	e	|										ec
		|	NB		NM		NS		ZO		PS		PM		PB
		|
	NB| NB		NB		NM		NM		NS		ZO		ZO
	NM|	NB		NB		NM		NS		NS		ZO		NS
	NS|	NB		NM		NS		NS		ZO		PS		PS
	ZO|	NM		NM		NS		ZO		PS		PM		PM
	PS|	NM		NS		ZO		PS		PS		PB		PB
	PM|	ZO		ZO		PS		PS		PM		PB		PB
	PB|	ZO		ZO		PS		PM		PM		PB		PB
	
	kd*
	e	|										ec
		|	NB		NM		NS		ZO		PS		PM		PB
		|
	NB| PS		NS		NB		NB		NB		NM		PS
	NM|	PS		NS		NM		NM		NM		NS		ZO
	NS|	ZO		NS		NS		NM		NS		NS		ZO
	ZO|	ZO		NS		NS		NS		NS		NS		ZO
	PS|	ZO		ZO		ZO		ZO		ZO		ZO		ZO
	PM|	PB		NS		PS		PS		PS		PS		PB
	PB|	PB		PM		PM		PM		PS		PS		PB


https://blog.csdn.net/shuoyueqishilove/article/details/78236541





*/
