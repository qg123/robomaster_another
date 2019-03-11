/**
  ******************************************************************************
  * File Name          : CAN.c
  * Description        : This file provides code for the configuration
  *                      of the CAN instances.
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "BSP_CAN.h"


#include "BSP_GPIO.h"


/* USER CODE BEGIN 0 */


CanRxMsgTypeDef CAN1_Receive;
	
CanTxMsgTypeDef CAN1_ReadyToSend;
CanTxMsgTypeDef CAN1_ReallySend;



CAN_FilterTypeDef CAN1_Filter;
/* USER CODE END 0 */


CAN_HandleTypeDef hcan1;

/* CAN1 init function */
void MX_CAN1_Init(void)//����һֱҲ�ǳ�ʼ�������⣬֮������һ��֮ǰ�����ֿ����ˡ��������//st-link����֮�����������˺ٺ�
{

  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 3;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_9TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_4TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;//��ʱ�䴥��ͨ��ģʽ//�����ɽ��շ�������ʱ���ֵ
  hcan1.Init.AutoBusOff = ENABLE;//����Զ����߹���//һ����⵽128��11����������λ��Ӳ���Զ����߹ر�/////
  hcan1.Init.AutoWakeUp = DISABLE;//�Զ�����ģʽ//���ͨ��can_mcr��sleepλ���㷢������󣬽���˯��
  hcan1.Init.AutoRetransmission = ENABLE;//�Ƿ��ֹ�Զ��ط���//���۽����Σ�ֻ����һ��//��һ����ֱ����׼��Ϣ���ͳɹ�
  hcan1.Init.ReceiveFifoLocked = DISABLE;//����FIFO����ģʽ//FIFOװ������һ������ǰһ����Ϣ
  hcan1.Init.TransmitFifoPriority = DISABLE;//����FIFO���ȼ�//����Ϣ��ʶ������
	
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	  HAL_CAN_Start(&hcan1);
//	Can1_Filter.SlaveStartFilterBank = ;һ��can�Ͳ��ù�
	CAN1_Filter.FilterActivation = ENABLE;
	CAN1_Filter.FilterBank = 0;
	CAN1_Filter.FilterFIFOAssignment = CAN_FilterFIFO0;
	CAN1_Filter.FilterIdHigh = 0;
	CAN1_Filter.FilterIdLow = 0;
	CAN1_Filter.FilterMode = CAN_FILTERMODE_IDMASK;//��ʶ�����룬32λ����32λ��ʶ��//��һ��ģʽ�Ϳ���������32λ��ʶ����
	CAN1_Filter.FilterMaskIdHigh = 0;
	CAN1_Filter.FilterMaskIdLow = 0;
	CAN1_Filter.FilterScale = CAN_FILTERSCALE_32BIT;

	CAN1_ReadyToSend.tx_header.DLC = 0x08;
	CAN1_ReadyToSend.tx_header.IDE = CAN_ID_STD;
	CAN1_ReadyToSend.tx_header.RTR = CAN_RTR_DATA;
	CAN1_ReallySend.tx_header.DLC = 0x08;
	CAN1_ReallySend.tx_header.IDE = CAN_ID_STD;
	CAN1_ReallySend.tx_header.RTR = CAN_RTR_DATA;
	
	CAN1_Receive.rx_header.DLC = 0x08;//�����һ���ͷ�����ɡ�
	CAN1_Receive.rx_header.RTR = CAN_RTR_DATA;
	CAN1_Receive.rx_header.IDE = CAN_ID_STD;
	
	HAL_CAN_ConfigFilter(&hcan1,&CAN1_Filter);//����д���ղ�������

  HAL_CAN_ActivateNotification(&hcan1,CAN_IT_RX_FIFO0_MSG_PENDING);	//ojbk��// �о����ǣ����й����жϣ����޹���clear,����ΪFMP���ڱ���Ϊ��һ��flagλ����һ�������ģ�Ӳ���Զ��Ӽ�   //����can�Ĺ���������ڽ��������
	HAL_CAN_ActivateNotification(&hcan1,CAN_IT_TX_MAILBOX_EMPTY);
	
				
}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspInit 0 */

  /* USER CODE END CAN1_MspInit 0 */
    /* CAN1 clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();
  
    /**CAN1 GPIO Configuration    
    PB8     ------> CAN1_RX
    PB9     ------> CAN1_TX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_0;//ÿ��can�ڶ�Ӧ��ͬ�����ţ����ĸ�ͷ�����ĸ�����
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* CAN1 interrupt Init */
    HAL_NVIC_SetPriority(CAN1_TX_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(CAN1_TX_IRQn);
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
  /* USER CODE BEGIN CAN1_MspInit 1 */

  /* USER CODE END CAN1_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspDeInit 0 */

  /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();
  
    /**CAN1 GPIO Configuration    
    PB8     ------> CAN1_RX
    PB9     ------> CAN1_TX 
    */
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_1|GPIO_PIN_0);

    /* CAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(CAN1_TX_IRQn);
    HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);
  /* USER CODE BEGIN CAN1_MspDeInit 1 */

  /* USER CODE END CAN1_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
