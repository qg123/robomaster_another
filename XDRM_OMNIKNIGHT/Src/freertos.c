/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "CanBusTask.h"
#include "ControlTask.h"
#include "StatusMachine.h"
#include "SuperviseTask.h"
//#include "StatusMachine.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/*
UBaseType_t uxTaskGetSystemState()*��ȡ����״̬�����ô˺���������������񣬽����Żָ��������񣬵���ʱ���á���غ�configUSE_TRACE_FACILITY
TaskHandle_t xTaskGetCurrentTaskHandle(void );��ȡ����������غ�INCLUDE_xTaskGetCurrentTaskHandle
UBaseType_t uxTaskGetStackHighWaterMark( TaskHandle_t xTask );����������Сʣ���ջ�ռ䣬����Ϊ��λ����غ�INCLUDE_uxTaskGetStackHighWaterMark
eTaskState eTaskGetState( TaskHandle_txTask );��ȡ����״̬��INCLUDE_eTaskGetState
volatile TickType_t xTaskGetTickCount(void );��ȡϵͳ���Ĵ��������ش�vTaskStartScheduler�������ú��ϵͳʱ�ӽ��Ĵ�����ISR����xTaskGetTickCountFromISR()
BaseType_t xTaskGetSchedulerState( void);��ȡ��������ǰ״̬�����ļ�FreeRTOSConfig.h�У���INCLUDE_xTaskGetSchedulerState
void vTaskList( char *pcWriteBuffer );*�����usTaskGetSystemState()����������ʱ�ã���ÿ�������״̬����ջʹ����������ַ�����ʽ���浽����pcWriteBufferָ�������
void vTaskGetRunTimeStats( char*pcWriteBuffer );*����ʱ�ã������������ͳ��ÿ�����������ʱ�䡣������һ������ʱ��ͳ�ƵĶ�ʱ����������������ʱ����������ľ���Ҫ���ٴ���10����ϵͳ��������
��غ�portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()portGET_RUN_TIME_COUNTER_VALUE
BaseType_txTaskCallApplicationTaskHook();ִ�������Ӧ�ù��Ӻ���
*/



/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId Led_ToggleHandle;
osThreadId Can_SendHandle;
osThreadId Task_ControlHandle;
osThreadId Status_UpdateHandle;
osThreadId Superviese_TaskHandle;

osMessageQId myQueue01Handle;
osMutexId myMutex01Handle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void Led_Toggle_Task(void const * argument);


void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  osMutexDef(myMutex01);
  myMutex01Handle = osMutexCreate(osMutex(myMutex01));

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of Led_Toggle */
  osThreadDef(Led_Toggle, Led_Toggle_Task, osPriorityLow, 0, 128);
  Led_ToggleHandle = osThreadCreate(osThread(Led_Toggle), NULL);

  /* definition and creation of Can_Send */
  osThreadDef(Can_Send, Can_Send_Task, osPriorityRealtime, 0, 256);
  Can_SendHandle = osThreadCreate(osThread(Can_Send), NULL);

  osThreadDef(Drivers_Control, Drivers_Control_Task, osPriorityRealtime, 0, 512);
  Task_ControlHandle = osThreadCreate(osThread(Drivers_Control), NULL);


  osThreadDef(Status_Update, StatusMachine, osPriorityRealtime, 0, 256);
  Status_UpdateHandle = osThreadCreate(osThread(Status_Update), NULL);//״̬��ֱ��д���������ֺܶ��ض�������⣬������ע�͵�
	
	osThreadDef(Superviese, Superviese_Task, osPriorityRealtime, 0, 256);
	Superviese_TaskHandle = osThreadCreate(osThread(Superviese),NULL);
  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
	
	osMessageQDef(myQueue01, 16, uint16_t);
	myQueue01Handle = osMessageCreate(osMessageQ(myQueue01), NULL);

  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
}

/* USER CODE BEGIN Header_Led_Toggle_Task */
/**
  * @brief  Function implementing the Led_Toggle thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_Led_Toggle_Task */
void Led_Toggle_Task(void const * argument)
{

  /* USER CODE BEGIN Led_Toggle_Task */
	
		
	portTickType xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();

  /* Infinite loop */
  for(;;)
  {
    
		HAL_GPIO_TogglePin(GPIOF,GPIO_PIN_14);
		vTaskDelayUntil(&xLastWakeTime,100/portTICK_RATE_MS);

  }
  /* USER CODE END Led_Toggle_Task */
}

/* USER CODE BEGIN Header_Can_Send_Task */

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
