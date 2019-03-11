#ifndef __CONTROL_TASK_H
#define __CONTROL_TASK_H

#include "config.h"

#define PREPARE_TIME_TICK_MS 4000

typedef enum
{
    PREPARE_STATE,          //�ϵ���ʼ״̬
    NORMAL_RC_STATE,        //ң��������״̬
    KEYBOARD_RC_STATE,      //���̿���״̬
    STOP_STATE,             //ֹͣ״̬
}WorkState_e;

void ControlLoopTaskInit(void);
void Drivers_Control_Task(void const * argument);


#endif



