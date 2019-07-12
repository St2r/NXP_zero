/*-----------------------------------------------------------------------------------------------------
��ƽ    ̨������K60���İ�-���ܳ���
����    д��LQ-005
��E-mail  ��chiusir@163.com
������汾��V1.0������Դ���룬�����ο�������Ը�
�������¡�2019��04��02��
����    վ��http://www.lqist.cn
���Ա����̡�http://shop36265907.taobao.com
������ƽ̨��IAR 8.2
����    �ܡ�����CMT���PWM����SD5���
��ע�����
-------------------------------------------------------------------------------------------------------*/
#include "include.h"
#include "LQ_SD5.h"

/*------------------------------------------------------------------------------------------------------
����    ����Test_SD5
����    �ܡ�����CMT���PWM����SD5���
����    ������
���� �� ֵ����
��ʵ    ����Test_SD5(); //����SD5���
��ע�����ע�⣬һ��Ҫ�Զ����ǽ�������
--------------------------------------------------------------------------------------------------------*/
void Test_SD5(void)
{
    LED_Init();
    KEY_Init();
    UART_Init(UART4, 115200);
    CMT_PwmInit(50, 0);
    printf("SD5����������� \n");
    printf("��һ��ʹ�ö����ͬѧ������֮ǰ��ò�Ҫװ����ֹ���������ת�ջ� \n");
    printf("����ʹ��50Hz��PWM�źſ��� \n");
    printf("ʹ��1.5ms��Ϊ�������ֵ \n");
    printf("����K0����������� \n");
    printf("����K1����������� \n");
    printf("����K2����������� \n");
    
    uint16_t duty = 750;
    while(1)
    {
        switch(KEY_Read(1))     
        {
          case 1:
            OLED_CLS();
            OLED_P6x8Str(10,0,"LQ_Servo_Test");
            OLED_P6x8Str(10,1,"KEY0 Pressed");
            LED_Reverse(1); 
            duty = 550;
            CMT_PwmDuty(duty);     //50Hz һ������20ms  1.5 / 20 * CMT_PRECISON  = 75  //CMT_PRECISON������Ϊ10000
            break;           
          case 2: 
            OLED_CLS();
            OLED_P6x8Str(10,0,"LQ_Servo_Test");
            OLED_P6x8Str(10,2,"KEY1 Pressed");
            LED_Reverse(2); 
            duty +=50;
            if(duty > 1100)        //�����Լ���ʵ������Զ����ǽ������ƣ���ֹ����
            {
              duty = 1100;}
           CMT_PwmDuty(duty);
            break;
          case 3: 
            OLED_CLS();
            OLED_P6x8Str(10,0,"LQ_Servo_Test");
            OLED_P6x8Str(10,3,"KEY2 Pressed");
            LED_Reverse(3); 
            duty -=50;
            if(duty < 225)        //�����Լ���ʵ������Զ����ǽ������ƣ���ֹ����
            {
                duty = 225;
            }
            CMT_PwmDuty(duty);
            break;
          default:
            
            break;
        }
        LED_Reverse(0);    
        delayms(100);
    }
}

void SD5_Init(void)
{
	CMT_PwmInit(50, 0);
	CMT_PwmDuty(dutyMIDDLE);
	return;
}


/*
** ===================================================================
** ���pdת��

** ===================================================================
*/
int PID_SD5(int distance){
  
    int turn_pwm_duty;      //����ʵ��ֵ
    float err;              //����ƫ��ֵ
    static float err_pre = 0.0;          //������һ��ƫ��ֵ
    float turnKp,turnKd;    //���������΢��ϵ��
    
    turnKp=70;
    turnKd=0.2;
    err=distance;
    //err_pre=0.0;

    
    turn_pwm_duty=(int)(turnKp*err+turnKd*(err-err_pre)); 
    
    err_pre = err ;
    turn_pwm_duty = dutyMIDDLE - turn_pwm_duty;
    
    if(turn_pwm_duty < dutyMIN ){turn_pwm_duty = dutyMIN; }
    
    else if(turn_pwm_duty > dutyMAX){turn_pwm_duty = dutyMAX;} 
    return turn_pwm_duty;
}

