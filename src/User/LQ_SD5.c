/*-----------------------------------------------------------------------------------------------------
【平    台】龙邱K60核心板-智能车板
【编    写】LQ-005
【E-mail  】chiusir@163.com
【软件版本】V1.0，龙邱开源代码，仅供参考，后果自负
【最后更新】2019年04月02日
【网    站】http://www.lqist.cn
【淘宝店铺】http://shop36265907.taobao.com
【编译平台】IAR 8.2
【功    能】测试CMT输出PWM控制SD5舵机
【注意事项】
-------------------------------------------------------------------------------------------------------*/
#include "include.h"
#include "LQ_SD5.h"

/*------------------------------------------------------------------------------------------------------
【函    数】Test_SD5
【功    能】测试CMT输出PWM控制SD5舵机
【参    数】无
【返 回 值】无
【实    例】Test_SD5(); //测试SD5舵机
【注意事项】注意，一定要对舵机打角进行限制
--------------------------------------------------------------------------------------------------------*/
void Test_SD5(void)
{
    LED_Init();
    KEY_Init();
    UART_Init(UART4, 115200);
    CMT_PwmInit(50, 0);
    printf("SD5舵机测试例程 \n");
    printf("第一次使用舵机的同学，调试之前最好不要装车防止舵机卡死堵转烧坏 \n");
    printf("这里使用50Hz的PWM信号控制 \n");
    printf("使用1.5ms作为舵机的中值 \n");
    printf("按下K0按键舵机归中 \n");
    printf("按下K1按键舵机向左 \n");
    printf("按下K2按键舵机向右 \n");
    
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
            CMT_PwmDuty(duty);     //50Hz 一个周期20ms  1.5 / 20 * CMT_PRECISON  = 75  //CMT_PRECISON已设置为10000
            break;           
          case 2: 
            OLED_CLS();
            OLED_P6x8Str(10,0,"LQ_Servo_Test");
            OLED_P6x8Str(10,2,"KEY1 Pressed");
            LED_Reverse(2); 
            duty +=50;
            if(duty > 1100)        //根据自己的实际情况对舵机打角进行限制，防止卡死
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
            if(duty < 225)        //根据自己的实际情况对舵机打角进行限制，防止卡死
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
** 舵机pd转向

** ===================================================================
*/
int PID_SD5(int distance){
  
    int turn_pwm_duty;      //定义实际值
    float err;              //定义偏差值
    static float err_pre = 0.0;          //定义上一个偏差值
    float turnKp,turnKd;    //定义比例、微分系数
    
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

