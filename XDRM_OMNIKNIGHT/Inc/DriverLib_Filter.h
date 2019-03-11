#ifndef __DRIVERLIB_FILTER_H
#define __DRIVERLIB_FILTER_H


#include "config.h"


typedef __packed struct
{
    float input;        //��������
    float out;          //�˲����������
    float num;       //�˲�����
    float frame_period; //�˲���ʱ���� ��λ s
} first_order_filter_type_t;


#endif



