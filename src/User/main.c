/*-----------------------------------------------------------------------------------------------------
��ƽ    ̨������K60���İ�-���ܳ���
����    д��LQ-005
��E-mail  ��chiusir@163.com
������汾��V1.0������Դ���룬�����ο�������Ը�
�������¡�2019��04��02��
����    վ��http://www.lqist.cn
���Ա����̡�http://shop36265907.taobao.com
������ƽ̨��IAR 8.2
����    �ܡ�
��ע�����
-------------------------------------------------------------
LED             ��Ƭ���ӿ�
LED0            PTE26
LED1            PTA17
LED2            PTC0
LED3            PTD15
=============================================================
-------------------------------------------------------------
����            ��Ƭ���ӿ�
K0              PTB20
K1              PTB21
K2              PTB22
-------------------------------------------------------------
-------------------------------------------------------------
OLED/TFT1.8ģ�� ��Ƭ���ӿ�
VCC             3.3V
GND             GND
SCK             PTC16
SDA             PTC15
RST             PTC14
DC              PTC13
CS              PTC12                        
-------------------------------------------------------------
-------------------------------------------------------------
����ͷ�ӿ�      ��Ƭ���ӿ�
Y0-7            PTD8-PTD15           
HREF            PTC6
VSY             PTC7
PCK             PTE6           
SCL             PTE5           
SDA             PTE4           
VCC             3.3V    
-------------------------------------------------------------
-------------------------------------------------------------
���ڽӿ�      ��Ƭ���ӿ�
TXD            PTE24           
RXD            PTE25  
-------------------------------------------------------------
-------------------------------------------------------------
CCD�ӿ�
              
GND            GND 
VDD            5V
AO             ADC0_SE16 
CLK            PTE2        
SI             PTE3               
-------------------------------------------------------------
-------------------------------------------------------------
�������ӿ�
              
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
//            ���汣��               ����BUG
-------------------------------------------------------------------------------------------------------*/
#include "include.h"


/*״̬��־λ*/
volatile uint8_t ImageCatchedFlag = 0;
volatile uint8_t DataProcessedFlag = 0;
volatile uint8_t RunningFlag = 1;


/*״̬����*/
volatile uint8_t MotorSpeedLeft = 0;
volatile uint8_t MotorSpeedRight = 0;
volatile uint8_t SD5Duty = 0;
volatile uint8_t MotorDutyLeft = 0;
volatile uint8_t MotorDutyRight = 0;

extern uint8_t LineLeft[LCDH];
extern uint8_t LineRight[LCDH];

/*��������*/
volatile uint8_t BlackMiddle = 0;
volatile uint8_t BlackLeft = 0;
volatile uint8_t BlackRight = 0;

/*�м����*/
char txt[16];


void main(void) 
{
	/*������Ƶ*/
	PLL_Init(PLL200); 

	/*�����ж����ȼ�*/
    NVIC_SetPriorityGrouping(0x07 - 2);     
    	
	/*ģ���ʼ��*/
	/*LED*/
	LED_Init();//����LED3
	/*����*/
	KEY_Init();//����
	/*���*/
    SD5_Init();
    /*������*/
	ENC_Init();
	/*OLED*/
	OLED_Init();
	OLED_CLS();
	if(DebugCamera)
		OLED_Show_Frame94();//��ͼ�� LCDW*LCDH ���
	/*����ͷ*/
	CAMERA_Init();
	/*������ģ��*/
	//HCSR_Init();	//todo::��bug
	/*���*/
	MOTOR_Init();


	while (RunningFlag)
	{
		LED_Reverse(1);

		/*ͼ��ɼ����*/
		if (Field_Over_Flag)       //���һ��ͼ��ɼ�
		{

			Get_Use_Image();      //�Ӳɼ�ͼ��������ȡ���Լ���Ҫʹ�õĴ�С ͼ���СΪ60�� 94��
			Get_01_Value(1);      //��ֵ��ͼ������
			if(DebugCamera)
				Draw_Road();          //����OLEDģ����ʾ��̬ͼ��
			
			for (uint8_t temp = 0; temp < 60; temp++)
			{
				Camera_Black(&Pixle[temp][0], &Road[temp][0],&LineLeft[temp],&LineRight[temp]);
			}

			if (DebugRoad)
				Draw_Road_alt();
			Field_Over_Flag = 0;		//����ͼ��ɼ�
			ImageCatchedFlag = 1;
		}

		/*ͼ������ɣ����ݴ���*/
		if (ImageCatchedFlag)
		{
			/*�ӱ�������õ�ǰ�ٶ�*/
			MotorSpeedLeft = FTM_ABGet(FTM1);//todo:: ���ҵ���
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


			//todo::��ֵͼ���ȡ����λ��
			//todo::���ݺ������ö��duty
			//todo::���õ��duty
			ImageCatchedFlag = 0;
			DataProcessedFlag = 1;
		}
		/*���Ƶ���Ͷ��*/
		if (DataProcessedFlag)
		{
			CMT_PwmDuty(SD5Duty);//���ö��ת��

			MOTOR_Ctrl(MotorLeft, MotorDutyLeft);     //���õ��1��ת��
			MOTOR_Ctrl(MotorRight, MotorDutyRight);     //���õ��2��ת��
			
			DataProcessedFlag = 0;
			Field_Over_Flag = 0;//����ͼ��ɼ�
		}
		//todo::������ϣ�
		//todo::��·��Ŵ�����ͣ��
	}
}



