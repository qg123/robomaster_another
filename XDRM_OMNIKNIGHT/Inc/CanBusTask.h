#ifndef __CANBUSTASK_H
#define __CANBUSTASK_H



#include "Config.h"


#define CAN_BUS1_MOTOR1_FEEDBACK_MSG_ID           0x201
#define CAN_BUS1_MOTOR2_FEEDBACK_MSG_ID           0x202
#define CAN_BUS1_MOTOR3_FEEDBACK_MSG_ID           0x203
#define CAN_BUS1_MOTOR4_FEEDBACK_MSG_ID           0x204
#define CAN_BUS1_BELTMOTOR1_FEEDBACK_MSG_ID				0x205
#define CAN_BUS1_BELTMOTOR2_FEEDBACK_MSG_ID				0x206
#define CAN_BUS1_ARMMOTOR_FEEDBACK_MSG_ID					0x207


#define CAN_SEND_NUM 3


#define RATE_BUF_SIZE 6
typedef struct{
	int16_t	 	speed_rpm;//ת��
	int16_t  	real_current;//ʵ��ת��
	uint8_t  	Temperature;
	uint16_t 	angle;//�Ƕ�
	uint16_t  lastangle;
	int32_t   ecd_angle;
	int16_t   round_cnt;
}Measure;


typedef struct{
	int32_t raw_value;   									//���������������ԭʼֵ
	int32_t last_raw_value;								//��һ�εı�����ԭʼֵ
	int32_t ecd_value;                       //��������������ı�����ֵ
	int32_t diff;													//���α�����֮��Ĳ�ֵ
	int32_t temp_count;                   //������
	uint8_t buf_count;								//�˲�����buf��
	int32_t ecd_bias;											//��ʼ������ֵ	
	int32_t ecd_raw_rate;									//ͨ������������õ����ٶ�ԭʼֵ
	int32_t rate_buf[RATE_BUF_SIZE];	
	int32_t round_cnt;										//Ȧ��
	int32_t filter_rate;											//�ٶ�
	float ecd_angle;											//�Ƕ�
}Encoder;


void Can_Send_CM(int16_t iq_1,int16_t iq_2,int16_t iq_3,int16_t iq_4);
void Can_Send(void);
void Can_Send_Task(void const * argument);
void Can_Msg_Process(void);
void Can_Send_BM(int16_t iq_1,int16_t iq_2,int16_t iq_3,int16_t iq_4);

extern Measure Motor1_Measure;
extern Measure Motor2_Measure;
extern Measure Motor3_Measure;
extern Measure Motor4_Measure;

extern Measure LBeltM_Measure;
extern Measure RBeltM_Measure;
extern Measure ARMM_Measure;

extern volatile Encoder TurntableEncoder;
#endif


