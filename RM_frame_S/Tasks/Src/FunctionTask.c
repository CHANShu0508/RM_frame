/**
  ******************************************************************************
  * File Name          : FunctionTask.c
  * Description        : 用于记录机器人独有的功能
  ******************************************************************************
  *
  * Copyright (c) 2018 Team TPP-Shanghai Jiao Tong University
  * All rights reserved.
  *
  ******************************************************************************
  */
#include "includes.h"

#define qudan
//#define shangdao
KeyboardMode_e KeyboardMode = NO_CHANGE;
MouseMode_e MouseLMode = NO_CLICK;
MouseMode_e MouseRMode = NO_CLICK;
RampGen_t LRSpeedRamp = RAMP_GEN_DAFAULT;   	//斜坡函数
RampGen_t FBSpeedRamp = RAMP_GEN_DAFAULT;
ChassisSpeed_Ref_t ChassisSpeedRef; 

#define rightClaw HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_12)
#define leftClaw HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_13)
#define masterHigh HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4)
#define masterLow HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_5)

int16_t channelrrow = 0;
int16_t channelrcol = 0;
int16_t channellrow = 0;
int16_t channellcol = 0;
int16_t testIntensity = 0;

uint32_t relookcount=0;
uint32_t counting=0;
int32_t wheelcount=0;
uint32_t firsttime=0;
uint32_t setwheelzero=0;
uint32_t SLcnt=0;
uint32_t SRcnt=0;
uint32_t SLflag=0;
uint32_t SRflag=0;
uint32_t checkleft=0;
uint32_t checkright=0;

uint32_t right_warning=0;
uint32_t left_warning=0;
int32_t  right_warning_state=0;
int32_t  left_warnging_state=0;
uint32_t left_warning_count=0;
uint32_t right_warning_count=0;
uint8_t leftClawZero = 0;
uint8_t rightClawZero = 0;
uint8_t rightClawTight = 0;
uint8_t leftClawTight = 0;
uint8_t loosing = 0;
uint8_t loosed = 0;
uint8_t readL,readR;

uint32_t firstread=0;

extern uint32_t AutoClimb_ComeToTop;
extern uint32_t AutoClimb_AlreadyTop;
extern uint32_t AutoClimbing;

uint32_t openthegay=0;

uint32_t FrontBackInspect=0;


//初始化
void FunctionTaskInit()
{
	LRSpeedRamp.SetScale(&LRSpeedRamp, MOUSE_LR_RAMP_TICK_COUNT);
	FBSpeedRamp.SetScale(&FBSpeedRamp, MOUSR_FB_RAMP_TICK_COUNT);
	LRSpeedRamp.ResetCounter(&LRSpeedRamp);
	FBSpeedRamp.ResetCounter(&FBSpeedRamp);
	
	ChassisSpeedRef.forward_back_ref = 0.0f;
	ChassisSpeedRef.left_right_ref = 0.0f;
	ChassisSpeedRef.rotate_ref = 0.0f;
	
	KeyboardMode=NO_CHANGE;
}

void OptionalFunction()
{
	Cap_Control();
	PowerLimitation();
}

void relook()
{
				relookcount++;
				if(relookcount%2==1)
					YTY.TargetAngle=-120;
				if(relookcount%2==0)
					YTY.TargetAngle=0;
}
uint32_t wheeldown=0;
void SetWheelZero()
{
	if(setwheelzero==0)
	{
	if(WHEEL.RxMsgC6x0.moment>-2000)
	{
		counting=0;
		WHEEL.TargetAngle-=10;
	}
	if(WHEEL.RxMsgC6x0.moment<=-2000)
	{
	  counting=1;
	}
	if(wheelcount>=1000)
	{
		WHEEL.RealAngle=0;
		WHEEL.TargetAngle=0;
		setwheelzero=1;
	}
  }
	
	if(setwheelzero==1&&wheeldown==1)
	{
		if(WHEEL.RxMsgC6x0.moment<2000)
	{
		counting=0;
		WHEEL.TargetAngle+=10;
	}
	if(WHEEL.RxMsgC6x0.moment>=2000)
	{
	  counting=1;
	}
	if(wheelcount>=1000)
	{
		WHEEL.TargetAngle=WHEEL.RealAngle;
		wheeldown=0;
	}
	}
}


uint8_t transdata[1];
extern uint8_t store;
extern uint32_t slave_flag;
extern uint32_t last_flag;
uint32_t saving_left=0;
uint32_t saving_right=0;
void Rescue()
{
	if(firstread==0)
			{
				firstread=1;
				SR.TargetAngle=-0;
				SL.TargetAngle=0;
			}
			loosed = 1;
			if(rightClaw == 1 && !rightClawTight)
			{
				saving_right=1;
			}
			if(leftClaw == 1 && !leftClawTight)
			{
				saving_left=1;
			}
}
void EndRescue()
{
	firstread=0;
	loosing = 1;
	saving_left=0;
	saving_right=0;
}
void RescueLoop()
{
	readL = masterHigh;
	readR = masterLow;
	checkleft=rightClaw;
	checkright=leftClaw;
	SetWheelZero();
	if(!leftClawZero)
	{
		if(SL.RxMsgC6x0.moment < 3000)
		{SL.TargetAngle += 1;SLflag=0;}
		else
		{
			SLflag = 1;
		}
		if(SLcnt>=800)
		{
			SL.RealAngle = 130;
			SL.TargetAngle = 120;
			leftClawZero = 1;
			SLflag = 0;
			SLcnt = 0;
			leftClawTight = 0;
		}
	}
	if(saving_left==1)
	{
		if(SL.RxMsgC6x0.moment >- 3000)
		{SL.TargetAngle -= 3;SLflag=0;}
		else
		{
			SLflag = 1;
		}
		if(SLcnt>=500)
		{
			SL.TargetAngle = SL.RealAngle+5;
			leftClawTight = 1;
			SLflag = 0;
			SLcnt = 0;
		}
	}
	if(!rightClawZero)
	{	
		if(SR.RxMsgC6x0.moment > -3000)
		{SR.TargetAngle -= 1;SRflag=0;}
		else
		{
			SRflag = 1;
		}
		if(SRcnt>=800)
		{
			SR.RealAngle = -130;
			SR.TargetAngle = -120;
			rightClawZero = 1;
			SRflag = 0;
			SRcnt = 0;
			rightClawTight = 0;
		}
	}
	if(saving_right==1)
	{	
		if(SR.RxMsgC6x0.moment <3000)
		{SR.TargetAngle += 3;SRflag=0;}
		else
		{
			SRflag = 1;
		}
		if(SRcnt>=500)
		{
			SR.TargetAngle = SR.RealAngle-5;
			rightClawTight = 1;
			SRflag = 0;
			SRcnt = 0;
		}
	}
	if(leftClawTight==1&&rightClawTight==1)
  {
		transdata[0]='t';
		HAL_UART_Transmit_IT(&huart8,transdata,1);
	}
	else
	{
		transdata[0]='n';
		HAL_UART_Transmit_IT(&huart8,transdata,1);
	}
	if(leftClawZero && rightClawZero)
	{
		if(slave_flag==1)//Rescue
		{
			Rescue();
		}
		if(slave_flag==5)
		{
			if(firstread==0)
			{
				firstread=1;
			}
			loosed = 1;
			saving_right=1;
			saving_left=1;
		}
		if(slave_flag==2)//loose
		{
			EndRescue();
		}
		else loosing = 0;
		if(loosing && loosed)
		{
			loosed = 0;
			leftClawZero = 0;
			rightClawZero = 0;
		}
		
		if(slave_flag==3)//wheeldown
		{
			loosed=1;
			wheeldown=1;
		}
		if(slave_flag==4)//common
		{
			setwheelzero=0;
			loosed = 1;
		}
		if(slave_flag==6)
		{
			if(last_flag==5)
			{
				firstread=0;
				loosed=1;
				leftClawZero=1;
				rightClawZero=1;
				saving_right=1;
			  saving_left=1;
				leftClawTight=0;
				rightClawTight=0;
			}
			else if(last_flag==2)
			{
				loosed=1;
				loosing=1;
			}
		}
	}
	else
	{
		if(masterHigh==0 && masterLow==1)//loose
		{
			firstread=0;
			loosing = 1;
		}
		else loosing = 0;
		if(loosing && loosed)
		{
			loosed = 0;
			leftClawZero = 0;
			rightClawZero = 0;
		}
		
		if(slave_flag==5)//tight
		{
			loosed=1;
			if(!rightClawTight || !leftClawTight)
			{
				SL.TargetAngle = 0;
				SR.TargetAngle = 0;
			}
			leftClawTight = 1;
			rightClawTight = 1;
		}
		if(masterHigh==0 && masterLow==0)
		{
			loosed = 1;
		}
	}
}

void Limit_and_Synchronization()
{
	//demo
	//MINMAX(NMUDL.TargetAngle,-700,700);//limit
	NMCDL.TargetAngle = NMCDR.TargetAngle;//sychronization
	UM1.TargetAngle=-UM2.TargetAngle;
	
	//demo end
}
//******************
//遥控器模式功能编写
//******************
void RemoteControlProcess(Remote *rc)
{
	if(WorkState <= 0) return;
	//max=660
	channelrrow = (rc->ch0 - (int16_t)REMOTE_CONTROLLER_STICK_OFFSET); 
	channelrcol = (rc->ch1 - (int16_t)REMOTE_CONTROLLER_STICK_OFFSET); 
	channellrow = (rc->ch2 - (int16_t)REMOTE_CONTROLLER_STICK_OFFSET); 
	channellcol = (rc->ch3 - (int16_t)REMOTE_CONTROLLER_STICK_OFFSET); 
	if(WorkState == NORMAL_STATE)
	{	
		
		#ifdef qudan
		SetWheelZero();
		if(channellrow>500)
			WHEEL.TargetAngle=0;
		if(channellrow<-500)
			WHEEL.TargetAngle=-155;
		UM1.TargetAngle+=channelrrow*0.001;
		UM2.TargetAngle-=channelrrow*0.001;//右横向是爪子的上下移动
			
		if(channellcol>500)
		CLAWOUT;//左纵向是爪子的向前弹出
		if(channellcol<-500)                                  
		CLAWIN;
	
	
		
	
		#else
		
		
   if(NMCDL.RxMsgC6x0.moment>-12000&&NMCDR.RxMsgC6x0.moment>-14000&&channellcol<0)
		{
		NMCDL.TargetAngle+=channellcol*0.06;
		NMCDR.TargetAngle+=channellcol*0.06;
		}
		if(channellcol>0)
		{
		NMCDL.TargetAngle+=channellcol*0.06;
		NMCDR.TargetAngle+=channellcol*0.06;
		}
		CM1.TargetAngle+=channellrow*0.02;
		CM2.TargetAngle+=channellrow*-0.02;
		#endif
		
	}
	if(WorkState == ADDITIONAL_STATE_ONE)
	{
   #ifdef qudan
		//手动挡
		if(channellcol>200){       //UP  左纵向是整个机构的上下
			NMUDL.TargetAngle -= channellcol * 0.05;
			NMUDR.TargetAngle -= channellcol * 0.05;
		}	else if(channellcol<-200){		//DOWN 
			NMUDL.TargetAngle -= channellcol * 0.05;
			NMUDR.TargetAngle -= channellcol * 0.05;
		}
	  if(channelrrow>500)
		{CLAWTIGHT;}//右横向是抓紧的开关
			if(channelrrow<-500)
			{		CLAWLOOSE;}
			
			if(channelrcol>500)
				LAUNCH;//右纵向是弹药箱弹出的开关
			if(channelrcol<-500)
				LAND;

			UFM.TargetAngle-=channellrow*0.01;//左横向是水平电机   向左远离（角度++）向右靠近（角度--）
  #else
	     
			ChassisSpeedRef.forward_back_ref = channelrcol * RC_CHASSIS_SPEED_REF;
		ChassisSpeedRef.left_right_ref   = channelrrow * RC_CHASSIS_SPEED_REF/2;
		ChassisSpeedRef.rotate_ref = -channellrow * RC_ROTATE_SPEED_REF;
			
	#endif
			
}
	if(WorkState == ADDITIONAL_STATE_TWO)
	{
		//****************自动取弹程序//UM1--是拔出来UM2相反  最大120//大的PH2 小的PH4***************
/****************************自检**********************************/
			//*****调试数据UFM.RxMsgC6x0.moment   >5000 在远端卡住 <-5000 在近端卡住  + 往远端移动  - 近端移动 
			//**targetAngle 总行程830左右
			//360°共11个齿 每个齿12.7mm 
			//最靠近电机moment是负的 realangle=0,中间是410左右 最远端700左右
			//比较健康的moment是3000    靠近电机-3000 远离电机3000 
			//NMUDL840 NMUDR-840
			//红外2000 3000
			/*if(channelrcol>500&&Claw_UpToPosition==0)//一键抬升整个机构
			{
				Claw_UpToPosition=1;
			}
			else if(channelrcol<-500)
			{
				Claw_UpToPosition=0;
				Claw_UpAngle=0;
			}
		  Claw_Up();
			
			if(channellrow>500&&AutoGet_Start==0)//启动自动取弹程序
				AutoGet_Start=1;
			if(channellrow<-500)//中途停止（用于故障处理）
			  AutoGet_Stop_And_Clear();
			
			
			AutoGet_SwitchState();*/
     		
		
			 ComeToTop();
			ChassisSpeedRef.forward_back_ref = channelrcol * RC_CHASSIS_SPEED_REF;
		  ChassisSpeedRef.left_right_ref   = channelrrow * RC_CHASSIS_SPEED_REF/2;
			ChassisSpeedRef.rotate_ref = -channellrow * RC_ROTATE_SPEED_REF;
		#ifdef shangdao	
		Chassis_Choose(1,1);  
		#endif
			
			
			
			/*if(channelrrow>500)
			 openthegay=1;
			if(channelrrow<-500)
				openthegay=0;
			
			if(openthegay==1)
			{
				__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,600);
				//__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_3,600);
			}
			else
			{
				__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,2000);
				//__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_3,2200);
			}*/
			//测试救援用 平常关闭   左++ 右--
//		ChassisSpeedRef.forward_back_ref = channelrcol * RC_CHASSIS_SPEED_REF;
//		ChassisSpeedRef.left_right_ref   = channelrrow * RC_CHASSIS_SPEED_REF/2;
//		ChassisSpeedRef.rotate_ref = -channellrow * RC_ROTATE_SPEED_REF;
//			setzero();
//			protect();
//			if(channellcol>500)
//			{
//				SL.TargetAngle=240;
//				SR.TargetAngle=-240;
//				lefttight=0;
//				righttight=0;
//				auto_counter=1000;
//			}
//			if(channellcol<-500)
//			{
//				setzerol=0;
//				setzeror=0;
//			}
			
			
//			if((leftstate==1)&&auto_counter==0)
//				lefttight=1;
//			if(lefttight==1)
//			{
//				if(SL.RxMsgC6x0.moment>-3000)
//					SL.TargetAngle-=20;
//				if(SL.RxMsgC6x0.moment<-5000)
//					SL.TargetAngle+=10;
//			}
//			if(rightstate==1&&auto_counter==0)
//				righttight=1;
//			if(righttight==1)
//			{
//				if(SR.RxMsgC6x0.moment<3000)
//					SR.TargetAngle+=20;
//				if(SR.RxMsgC6x0.moment>5000)
//					SR.TargetAngle-=10;
//			}
	}
	Limit_and_Synchronization();
}


uint16_t KM_FORWORD_BACK_SPEED 	= NORMAL_FORWARD_BACK_SPEED;
uint16_t KM_LEFT_RIGHT_SPEED  	= NORMAL_LEFT_RIGHT_SPEED;
void MouseModeFSM(Mouse *mouse);
void KeyboardModeFSM(Key *key);

//****************
//键鼠模式功能编写
//****************
void MouseKeyControlProcess(Mouse *mouse, Key *key)
{	
	if(WorkState <= 0) return;
	
	MINMAX(mouse->x, -150, 150); 
	MINMAX(mouse->y, -150, 150); 
	
	#ifdef USE_CHASSIS_FOLLOW

	if(AutoClimbing==0)
	ChassisSpeedRef.rotate_ref = mouse->x * MOUSE_TO_YAW_ANGLE_INC_FACT*-15;
	YTP.TargetAngle -= mouse->y * MOUSE_TO_PITCH_ANGLE_INC_FACT*5;
	//YTY.TargetAngle -= mouse->x * MOUSE_TO_YAW_ANGLE_INC_FACT*3;

	ChassisSpeedRef.rotate_ref = -mouse->x * MOUSE_TO_YAW_ANGLE_INC_FACT;
	YTP.TargetAngle -= mouse->y * MOUSE_TO_PITCH_ANGLE_INC_FACT;

	#else
	ChassisSpeedRef.rotate_ref = mouse->x * RC_ROTATE_SPEED_REF;
	#endif

	
	MouseModeFSM(mouse);
	
	switch(MouseRMode)
	{
		case SHORT_CLICK:
		{
		
		}break;
		case LONG_CLICK:
		{
			ChassisSpeedRef.rotate_ref = 0;
			YTY.TargetAngle -= mouse->x * MOUSE_TO_YAW_ANGLE_INC_FACT*3;
		}break;
		default: break;
	}
	
	switch (MouseLMode)
	{
		case SHORT_CLICK:
		{
			
		}break;
		case LONG_CLICK:
		{
			
		}
		default: break;
	}


	KeyboardModeFSM(key);//下面是移动的控制 在写命令时不要用wasd键
		if(key->v & KEY_W)  		//key: w
		{
			if (FrontBackInspect%2==0)
				ChassisSpeedRef.forward_back_ref =  KM_FORWORD_BACK_SPEED* FBSpeedRamp.Calc(&FBSpeedRamp);
			if (FrontBackInspect%2==1)
				ChassisSpeedRef.forward_back_ref =  -KM_FORWORD_BACK_SPEED* FBSpeedRamp.Calc(&FBSpeedRamp);
		}
			else if(key->v & KEY_S) 	//key: s
			{
				if(FrontBackInspect%2==0)
				ChassisSpeedRef.forward_back_ref = -KM_FORWORD_BACK_SPEED* FBSpeedRamp.Calc(&FBSpeedRamp);
				else
						ChassisSpeedRef.forward_back_ref = KM_FORWORD_BACK_SPEED* FBSpeedRamp.Calc(&FBSpeedRamp);
			}
			else
			{
				ChassisSpeedRef.forward_back_ref = 0;
				FBSpeedRamp.ResetCounter(&FBSpeedRamp);
			}
			if(key->v & KEY_D)  		//key: d
			{	
			if(FrontBackInspect%2==0)
				ChassisSpeedRef.left_right_ref =  KM_LEFT_RIGHT_SPEED * LRSpeedRamp.Calc(&LRSpeedRamp);
			else
				ChassisSpeedRef.left_right_ref =  -KM_LEFT_RIGHT_SPEED * LRSpeedRamp.Calc(&LRSpeedRamp);
			}
			else if(key->v & KEY_A) 	//key: a
			{	
			if(FrontBackInspect%2==0)
				ChassisSpeedRef.left_right_ref = -KM_LEFT_RIGHT_SPEED * LRSpeedRamp.Calc(&LRSpeedRamp);
			else
				ChassisSpeedRef.left_right_ref = KM_LEFT_RIGHT_SPEED * LRSpeedRamp.Calc(&LRSpeedRamp);
			}
			else
			{
				ChassisSpeedRef.left_right_ref = 0;
				LRSpeedRamp.ResetCounter(&LRSpeedRamp);
			}
			
	switch (KeyboardMode)
	{
		case SHIFT_CTRL:		//State control
		{
			
			break;
		}
		case CTRL:				//slow
		{
			if(key->v & KEY_C&&Claw_UpToPosition==0)
			{
				Claw_UpToPosition=1;
			}
			if(key->v & KEY_V)
			{
				NMUDL.TargetAngle=0;
				NMUDR.TargetAngle=0;
			}
			if(key->v & KEY_Q)
			 CLAWOUT;
			if(key->v & KEY_E)
			 CLAWIN;
			
			
			if(key->v & KEY_Z)
			{
				if(Claw_SelfInspecting==2)
				AutoGet_Start=1;
			}
			if(key->v & KEY_X)
			{
				if(Claw_SelfInspecting==2)
				AutoGet_Start=2;
			}
		}break;
		case SHIFT:				//quick
		{
			if(key->v & KEY_Q)
			{
				Claw_FindingNextBox_Upper=1;
			}
			else if(key->v & KEY_C)
			{
				AutoClimbing=1;
			}
			else if(key->v & KEY_V)
			{
				AutoClimbing=0;
			}
		}break;
		case NO_CHANGE:			//normal
		{//CM Movement Process
		
			if(key->v & KEY_X)
			{ 
				AutoGet_Stop_And_Clear();
			}
			else if(key->v & KEY_C)
			{
				if(Claw_SelfInspecting==2)
				Claw_TakeThisBox=1;
			}
			else if(key->v & KEY_V)
			{
				if(Claw_SelfInspecting==2)
				Claw_TakeThisBox=2;
			}
			else if(key->v & KEY_B)
			{
				if(Claw_SelfInspecting==2)
				Claw_TakeThisBox=3;
			}
			else if(key->v & KEY_F)
			{
				if(Claw_SelfInspecting==2)
				Claw_TakeThisBox=4;
			}
			else if(key->v & KEY_G)
			{
				if(Claw_SelfInspecting==2)
				Claw_TakeThisBox=5;
			}
			else if(key->v & KEY_Z)
			{
				Claw_SelfInspecting=1;
			}
			else if(key->v & KEY_Q)
			{
				Claw_FindingNextBox_Lower=1;
			}
			else if(key->v & KEY_E)
			{
				Claw_FindingNextBox_Lower=0;
				Claw_FindingNextBox_Upper=0;
				Sensor_Ready[0]=0;
			}
			OnePush(key->v & KEY_R,relook());
//			if(key->v & KEY_R)
//			{
//				
//				if(FrontBackInspect%2==0)
//					YTY.TargetAngle = 180;
//				else
//					YTY.TargetAngle = 0;
//				FrontBackInspect++;
//			}
			
		}
		Claw_GetSpecifiedBox();
		Claw_SelfInspect();
		if(Claw_FindingNextBox_Lower==1)
		Claw_GoToNextBox_lower();
		if(Claw_FindingNextBox_Upper==1)
		Claw_GoToNextBox_upper();	
		Claw_Up();
		Box_Land();
		AutoGet_SwitchState();
		AutoClimb_SwitchState();
		ComeToTop();
	}
	Limit_and_Synchronization();
}

void KeyboardModeFSM(Key *key)
{
	if((key->v & 0x30) == 0x30)//Shift_Ctrl
	{
		KM_FORWORD_BACK_SPEED=  LOW_FORWARD_BACK_SPEED;
		KM_LEFT_RIGHT_SPEED = LOW_LEFT_RIGHT_SPEED;
		KeyboardMode=SHIFT_CTRL;
	}
	else if(key->v & KEY_SHIFT)//Shift
	{
		KM_FORWORD_BACK_SPEED=  HIGH_FORWARD_BACK_SPEED;
		KM_LEFT_RIGHT_SPEED = HIGH_LEFT_RIGHT_SPEED;
		KeyboardMode=SHIFT;
	}
	else if(key->v & KEY_CTRL)//Ctrl
	{
		KM_FORWORD_BACK_SPEED=  LOW_FORWARD_BACK_SPEED;
		KM_LEFT_RIGHT_SPEED = LOW_LEFT_RIGHT_SPEED;
		KeyboardMode=CTRL;
	}
	else
	{
		KM_FORWORD_BACK_SPEED=  NORMAL_FORWARD_BACK_SPEED;
		KM_LEFT_RIGHT_SPEED = NORMAL_LEFT_RIGHT_SPEED;
		KeyboardMode=NO_CHANGE;
	}	
}

void MouseModeFSM(Mouse *mouse)
{
	static uint8_t counterl = 0;
	static uint8_t counterr = 0;
	switch (MouseLMode)
	{
		case SHORT_CLICK:
		{
			counterl++;
			if(mouse->press_l == 0)
			{
				MouseLMode = NO_CLICK;
				counterl = 0;
			}
			else if(counterl>=50)
			{
				MouseLMode = LONG_CLICK;
				counterl = 0;
			}
			else
			{
				MouseLMode = SHORT_CLICK;
			}
		}break;
		case LONG_CLICK:
		{
			if(mouse->press_l==0)
			{
				MouseLMode = NO_CLICK;
			}
			else
			{
				MouseLMode = LONG_CLICK;
			}
		}break;
		case NO_CLICK:
		{
			if(mouse->press_l)
			{
				MouseLMode = SHORT_CLICK;
			}
		}break;
	}
	
	switch (MouseRMode)
	{
		case SHORT_CLICK:
		{
			counterr++;
			if(mouse->press_r == 0)
			{
				MouseRMode = NO_CLICK;
				counterr = 0;
			}
			else if(counterr>=50)
			{
				MouseRMode = LONG_CLICK;
				counterr = 0;
			}
			else
			{
				MouseRMode = SHORT_CLICK;
			}
		}break;
		case LONG_CLICK:
		{
			if(mouse->press_r==0)
			{
				MouseRMode = NO_CLICK;
			}
			else
			{
				MouseRMode = LONG_CLICK;
			}
		}break;
		case NO_CLICK:
		{
			if(mouse->press_r)
			{
				MouseRMode = SHORT_CLICK;
			}
		}break;
	}
}

