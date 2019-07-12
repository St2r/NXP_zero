/*-----------------------------------------------------------------------------------------------------
【平    台】龙邱K60核心板-智能车板
【编    写】LQ-005
【E-mail  】chiusir@163.com
【软件版本】V1.0，龙邱开源代码，仅供参考，后果自负
【最后更新】2019年04月02日
【网    站】http://www.lqist.cn
【淘宝店铺】http://shop36265907.taobao.com
【编译平台】IAR 8.2
【功    能】
【注意事项】
-------------------------------------------------------------
LED             单片机接口
LED0            PTE26
LED1            PTA17
LED2            PTC0
LED3            PTD15
=============================================================
-------------------------------------------------------------
按键            单片机接口
K0              PTB20
K1              PTB21
K2              PTB22
-------------------------------------------------------------
-------------------------------------------------------------
OLED/TFT1.8模块 单片机接口
VCC             3.3V
GND             GND
SCK             PTC16
SDA             PTC15
RST             PTC14
DC              PTC13
CS              PTC12                        
-------------------------------------------------------------
-------------------------------------------------------------
摄像头接口      单片机接口
Y0-7            PTD8-PTD15           
HREF            PTC6
VSY             PTC7
PCK             PTE6           
SCL             PTE5           
SDA             PTE4           
VCC             3.3V    
-------------------------------------------------------------
-------------------------------------------------------------
串口接口      单片机接口
TXD            PTE24           
RXD            PTE25  
-------------------------------------------------------------
-------------------------------------------------------------
CCD接口
              
GND            GND 
VDD            5V
AO             ADC0_SE16 
CLK            PTE2        
SI             PTE3               
-------------------------------------------------------------
-------------------------------------------------------------
超声波接口
              
GND            GND 
VCC            5V
Trig           E9
Echo           E10            
-------------------------------------------------------------
//                       .::::.
//                     .::::::::.
//                    :::::::::::
//                 ..:::::::::::'
//              '::::::::::::'
//                .::::::::::
//           '::::::::::::::..
//                ..::::::::::::.
//              ``::::::::::::::::
//               ::::``:::::::::'        .:::.
//              ::::'   ':::::'       .::::::::.
//            .::::'      ::::     .:::::::'::::.
//           .:::'       :::::  .:::::::::' ':::::.
//          .::'        :::::.:::::::::'      ':::::.
//         .::'         ::::::::::::::'         ``::::.
//     ...:::           ::::::::::::'              ``::.
//    ```` ':.          ':::::::::'                  ::::..
//                       '.:::::'                    ':'````..
//
//            佛祖保佑               永无BUG
-------------------------------------------------------------------------------------------------------*/
#include "include.h"


/*状态标志位*/
volatile uint8_t ImageCatchedFlag = 0;
volatile uint8_t DataProcessedFlag = 0;
volatile uint8_t RunningFlag = 1;


/*状态参数*/
volatile uint8_t MotorSpeedLeft = 0;
volatile uint8_t MotorSpeedRight = 0;
volatile uint8_t SD5Duty = 0;
volatile uint8_t MotorDutyLeft = 0;
volatile uint8_t MotorDutyRight = 0;

extern uint8_t LineLeft[LCDH];
extern uint8_t LineRight[LCDH];

/*环境参数*/
volatile uint8_t BlackMiddle = 0;
volatile uint8_t BlackLeft = 0;
volatile uint8_t BlackRight = 0;

/*中间变量*/
char txt[16];


void main(void) 
{
	/*设置主频*/
	PLL_Init(PLL200); 

	/*设置中断优先级*/
    NVIC_SetPriorityGrouping(0x07 - 2);     
    	
	/*模块初始化*/
	/*LED*/
	LED_Init();//禁用LED3
	/*按键*/
	KEY_Init();//按键
	/*舵机*/
    SD5_Init();
    /*编码器*/
	ENC_Init();
	/*OLED*/
	OLED_Init();
	OLED_CLS();
	if(DebugCamera)
		OLED_Show_Frame94();//画图像 LCDW*LCDH 外框
	/*摄像头*/
	CAMERA_Init();
	/*超声波模块*/
	//HCSR_Init();	//todo::有bug
	/*电机*/
	MOTOR_Init();


	while (RunningFlag)
	{
		LED_Reverse(1);

		/*图像采集完成*/
		if (Field_Over_Flag)       //完成一场图像采集
		{

			Get_Use_Image();      //从采集图像数据中取出自己想要使用的大小 图像大小为60行 94列
			Get_01_Value(1);      //二值化图像数据
			if(DebugCamera)
				Draw_Road();          //龙邱OLED模块显示动态图像
			
			for (uint8_t temp = 0; temp < 60; temp++)
			{
				Camera_Black(&Pixle[temp][0], &Road[temp][0],&LineLeft[temp],&LineRight[temp]);
			}

			if (DebugRoad)
				Draw_Road_alt();
			Field_Over_Flag = 0;		//禁用图像采集
			ImageCatchedFlag = 1;
		}

		/*图像处理完成，数据处理*/
		if (ImageCatchedFlag)
		{
			/*从编码器获得当前速度*/
			MotorSpeedLeft = FTM_ABGet(FTM1);//todo:: 左右调整
			MotorSpeedRight = FTM_ABGet(FTM2);

			if(DebugEnc)
			{
				//printf("\r\n/ENC1 %5d \r\n ", MotorSpeedLeft);
				sprintf(txt, "enc1:%5d ", MotorSpeedLeft);
				OLED_P8x16Str(20, 2, (uint8_t*)txt);

				//printf("\r\n/ENC2 %5d \r\n ", MotorSpeedRight);
				sprintf(txt, "enc2:%5d ", MotorSpeedRight);
				OLED_P8x16Str(20, 4, (uint8_t*)txt);
			}


			/*if (ALineOverCout < LinADCout) {
				Camera_
				(&ADdata[ALineOverCout][0], &Rightblackedge[ALineOverCout],
					&Leftblackedge[ALineOverCout], &CameraStation[ALineOverCout]);
				ALineOverCout++;*/


			//todo::二值图像获取黑线位置
			//todo::根据黑线设置舵机duty
			//todo::设置电机duty
			ImageCatchedFlag = 0;
			DataProcessedFlag = 1;
		}
		/*控制电机和舵机*/
		if (DataProcessedFlag)
		{
			CMT_PwmDuty(SD5Duty);//设置舵机转角

			MOTOR_Ctrl(MotorLeft, MotorDutyLeft);     //设置电机1的转速
			MOTOR_Ctrl(MotorRight, MotorDutyRight);     //设置电机2的转速
			
			DataProcessedFlag = 0;
			Field_Over_Flag = 0;//申请图像采集
		}
		//todo::红外避障？
		//todo::六路电磁传感器停车
	}
}



