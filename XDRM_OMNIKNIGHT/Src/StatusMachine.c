#include "StatusMachine.h"
#include "SuperviseTask.h"
#include "ControlTask.h"

WorkState_e 	WorkState;
WorkState_e		LastWorkState = STOP_STATE;
CarMoveModeTypeDef CarMoveMode;
UpIslandStateTypeDef UpIslandState;
DownIslandStateTypeDef   DownIslandState;
uint16_t Up_Island_Flag;
uint16_t Down_Island_Flag;
extern uint32_t time_tick_1ms;

static void WorkStateSwitchProcess(void)
{
	//���������ģʽ�л���prapareģʽ��Ҫ��һϵ�в�����ʼ��
	if((LastWorkState != WorkState) && (WorkState == PREPARE_STATE))  
	{
		ControlLoopTaskInit();
		RemoteTaskInit();
	}
}

void SetWorkState(WorkState_e state)
{
    WorkState = state;
}


WorkState_e GetWorkState(void)
{
	return WorkState;
}

void WorkStateFSM(void)
{
  //����ֱ�ӻ�״̬���������
	LastWorkState = WorkState;
	if(Is_Lost_Error_Set(LOST_ERROR_RC) || InputMode == STOP)
  //���ɣ�ֱ��ʹ��InputMode��ʹ�ú���GetInputMode()��ʲô��������ʱ��Ϊû������
  {
    WorkState = STOP_STATE;
    return;
  }//��û�м��������Լ�У׼���ܣ�����Ŀǰֻ��ң����ʧ����һ������״��
  switch (WorkState)
  {
    case PREPARE_STATE:
    {
      if(time_tick_1ms > PREPARE_TIME_TICK_MS)
      {
        if(InputMode == REMOTE_INPUT)
        {
         WorkState = NORMAL_RC_STATE;
        }
        if(InputMode == KEYBOARD_INPUT)
        {
          WorkState = KEYBOARD_RC_STATE;
        }
      }
    }break;
  
    case NORMAL_RC_STATE:
    {
      if(InputMode == STOP)
      {
        WorkState = STOP_STATE;
      }
    }break;

    case KEYBOARD_RC_STATE:
    {
      if(InputMode == STOP)
      {
        WorkState = STOP_STATE;
      }
    }break;

    case STOP_STATE:
    {
      if(InputMode != STOP)
      {
        WorkState = PREPARE_STATE;
      }
    }break;
  }
	WorkStateSwitchProcess();
}



void Input_Mode_Select(void)
{
  if(RC_CtrlData.rc.s2 == STICK_UP)
  {
    InputMode = REMOTE_INPUT;
  }
  if(RC_CtrlData.rc.s2 == STICK_CENTRAL)
  {
    InputMode = KEYBOARD_INPUT;
  }
  if(RC_CtrlData.rc.s2 == STICK_DOWN)
  {
    InputMode = STOP;
  }
}
InputMode_e GetInputMode(void)
{
	return InputMode;
}

void Car_Move_State_Select(void)//���ܵ��˶�״̬ѡ��
{
  switch (WorkState)
  {
    case PREPARE_STATE:
    {
      CarMoveMode = Stop_Move_Mode;
    }break;

    case NORMAL_RC_STATE:
    {
      if (RC_CtrlData.rc.s1 == STICK_UP) 
      {
        CarMoveMode = Normal_Move_Mode;
      }
      if (RC_CtrlData.rc.s1 == STICK_CENTRAL) 
      {
        if(RC_CtrlData.rc.ch3 > 600)
        {
          Up_Island_Flag = 1;
        }
        else if(RC_CtrlData.rc.ch3 < -600)
        {
          Down_Island_Flag = 1;
        }
        if(Up_Island_Flag == 1)
        {
          CarMoveMode = Auto_Up_Island_Mode;
        }
        else if (Down_Island_Flag == 1) 
        {
          CarMoveMode = Auto_Down_Island_Mode;
        }
        
      }
      if (RC_CtrlData.rc.s1 == STICK_DOWN) 
      {
        CarMoveMode = Auto_Get_Box_Mode;
      }
    }break;

    case STOP_STATE:
    {
      CarMoveMode = Stop_Move_Mode;
    }break;

    default:
    {
      CarMoveMode = Stop_Move_Mode;
    }break;
  }
}

void CarMoveFSM(void)
{
  switch (CarMoveMode)
  {
    case Stop_Move_Mode:
    {
      BeltMode = BeltMove_Stop;
      ChassisMode = ChassisMove_Stop;
      GuideWheelMode = GuideWheelMove_Stop;
    }break;
  
    case Normal_Move_Mode:
    {
      BeltMode = Normal_Rc_BeltMove;
      ChassisMode = Normal_Rc_ChassisMove;
      GuideWheelMode = Normal_Rc_GuideWheelMove;
    }break;

    case Auto_Up_Island_Mode:
    {
      switch (UpIslandState)
      {
        case Up_Island_Prepare:
        {
          UpIslandState = Up_Island_BeltDown_First;
        }break;

        case Up_Island_BeltDown_First://Ƥ���½�ʱ�ѳ�����������
        {
          BM_AngelGet = 1;
          BeltMode = Belt_Down;
          ChassisMode = ChassisMove_Stop;
          GuideWheelMode = GuideWheelMove_Stop;
        }break;

        case Up_Island_GuideWheelAdvance_First:
        {
          BeltMode = BeltMove_Stop;
          ChassisMode = ChassisMove_Stop;
          GuideWheelMode = GuideWheel_Advance;
        }break;
				
        case Up_Island_BeltUp_First:
        {
          BM_AngelGet = 1;
          BeltMode = Belt_Up;
          ChassisMode = ChassisMove_Stop;
          GuideWheelMode = GuideWheelMove_Stop;
        }break;

        case Up_Island_ChassisAdvance_First:
        {
          BeltMode = BeltMove_Stop;
          ChassisMode = Chassis_Advance;
          GuideWheelMode = GuideWheelMove_Stop;
        }break;

        case Up_Island_BeltDown_Twice:
        {
          BM_AngelGet = 1;
          BeltMode = Belt_Down;
          ChassisMode = ChassisMove_Stop;
          GuideWheelMode = GuideWheelMove_Stop;
        }break;

        case Up_Island_GuideWheelAdvance_Twice:
        {
          BeltMode = BeltMove_Stop;
          ChassisMode = ChassisMove_Stop;
          GuideWheelMode = GuideWheel_Advance;
        }break;
				
        case Up_Island_BeltUp_Twice:
        {
          BM_AngelGet = 1;
          BeltMode = Belt_Up;
          ChassisMode = ChassisMove_Stop;
          GuideWheelMode = GuideWheelMove_Stop;
        }break;

        case Up_Island_Stop:
        {
          BeltMode = BeltMove_Stop;
          ChassisMode = ChassisMove_Stop;
          GuideWheelMode = GuideWheelMove_Stop;
        }break;

        default:
        {
          BeltMode = BeltMove_Stop;
          ChassisMode = ChassisMove_Stop;
          GuideWheelMode = GuideWheelMove_Stop;
        }break;
      }
    }

    case Auto_Down_Island_Mode:
    {
      switch (DownIslandState)
      {
        case Down_Island_Prepare:
        {
          DownIslandState = Down_Island_ChassisBack_First;
        }break;
        
        case Down_Island_ChassisBack_First:
        {
          BeltMode = BeltMove_Stop;
          ChassisMode = Chassis_Back;
          GuideWheelMode = GuideWheelMove_Stop;
        }break;

        case Down_Island_BeltDown_First:
        {
          BM_AngelGet = 1;
          BeltMode = Belt_Down;
          ChassisMode = ChassisMove_Stop;
          GuideWheelMode = GuideWheelMove_Stop;
        }break;

        case Down_Island_GuideWheelBack_First:
        {
          BeltMode = BeltMove_Stop;
          ChassisMode = ChassisMove_Stop;
          GuideWheelMode = GuideWheel_Back;
        }break;

        case Down_Island_BeltUp_First:
        {
          BM_AngelGet = 1;
          BeltMode = Belt_Up;
          ChassisMode = ChassisMove_Stop;
          GuideWheelMode = GuideWheelMove_Stop;
        }break;

        case Down_Island_ChassisBack_Twice:
        {
          BeltMode = BeltMove_Stop;
          ChassisMode = Chassis_Back;
          GuideWheelMode = GuideWheelMove_Stop;
        }break;

        case Down_Island_BeltDown_Twice:
        {
          BM_AngelGet = 1;
          BeltMode = Belt_Down;
          ChassisMode = ChassisMove_Stop;
          GuideWheelMode = GuideWheelMove_Stop;
        }break;


        case Down_Island_GuideWheelBack_Twice:
        {
          BeltMode = BeltMove_Stop;
          ChassisMode = ChassisMove_Stop;
          GuideWheelMode = GuideWheel_Back;
        }break;

        case Down_Island_BeltUp_Twice:
        {
          BM_AngelGet = 1;
          BeltMode = Belt_Up;
          ChassisMode = ChassisMove_Stop;
          GuideWheelMode = GuideWheelMove_Stop;
        }break;
      
        case Down_Island_stop:
        {
          BeltMode = BeltMove_Stop;
          ChassisMode = ChassisMove_Stop;
          GuideWheelMode = GuideWheelMove_Stop;
        }break;

        default:
        {
          BeltMode = BeltMove_Stop;
          ChassisMode = ChassisMove_Stop;
          GuideWheelMode = GuideWheelMove_Stop;
        }break;
      }
    }
    default:
    {
      BeltMode = BeltMove_Stop;
      ChassisMode = ChassisMove_Stop;
      GuideWheelMode = GuideWheelMove_Stop;
    }break;
  }
}


void UpIsland_Init(void)//���µ�����ʼ����
{
  ChassisMode = ChassisMove_Stop;
  GuideWheelMode = GuideWheelMove_Stop;
  BeltMode = BeltMove_Stop;
  Up_Island_Flag = 0;
  Down_Island_Flag = 0;
  UpIslandState = Up_Island_Prepare;
  DownIslandState = Down_Island_Prepare;
}


void StatusMachine_Init(void)
{
  WorkState = PREPARE_STATE;
  UpIsland_Init();
}


void StatusMachine_Update(void)
{
  Input_Mode_Select();
  WorkStateFSM();
  Car_Move_State_Select();
	CarMoveFSM();
}
	

void StatusMachine(void const * argument)
{
	portTickType xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();

  /* Infinite loop */
  for(;;)
  {

			
			StatusMachine_Update();
			
			vTaskDelayUntil(&xLastWakeTime,1/portTICK_RATE_MS);//��ʱ��������̬
		
		
  }
  /* USER CODE END Can_Send_Task */
}





