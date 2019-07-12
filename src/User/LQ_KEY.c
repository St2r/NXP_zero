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
-------------------------------------------------------------------------------------------------------*/
#include "include.h"

/*------------------------------------------------------------------------------------------------------
【函    数】LED_Init
【功    能】初始化核心板和母板上的LED
【参    数】无
【返 回 值】无
【实    例】LED_Init(); //初始化LED
【注意事项】
--------------------------------------------------------------------------------------------------------*/
void KEY_Init(void)
{
    /* 初始化母板上的KEY */
   GPIO_PinInit(PTB20, GPI_UP, 1);
   GPIO_PinInit(PTB21, GPI_UP, 1);
   GPIO_PinInit(PTB22, GPI_UP, 1);
}

/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
【作  者】LQ-005
【功能说明】按键处理函数
【软件版本】V1.0
【最后更新】2017年11月24日 
【函数名】
【返回值】0：无按键按下 1：按键1按下  2:按键2按下  3:按键3按下
【参数值】mode:0,不支持连续按;1,支持连续按;
【例子  】KEY_Read(0);   //检测三个按键哪个按下
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
uint8_t KEY_Read(uint8_t mode)
{
    static uint8_t key_up=1;     //按键松开标志
    if(mode==1) 
    {
      key_up=1;      //支持连按
    }
    if(key_up && (GPIO_PinRead(PTB20)==0 || GPIO_PinRead(PTB21)==0 || GPIO_PinRead(PTB22)==0))
    {
      delayms(100);   //消抖
      key_up=0;
      if(GPIO_PinRead(PTB20)==0)      
      {
          return 1;
      }
      
      else if(GPIO_PinRead(PTB21)==0) 
      {
          return 2;    
      }
      
      else if(GPIO_PinRead(PTB22)==0) 
      {
          return 3;    
      }
      
    }
    if(GPIO_PinRead(PTB20)==1 && GPIO_PinRead(PTB21)==1 && GPIO_PinRead(PTB22)==1) 
    {
     key_up=1;   
    }
    return 0;   //无按键按下
     
}

/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
【作  者】LQ-005
【功能说明】测试按键  
【软件版本】V1.0
【最后更新】2017年11月24日 
【函数名】
【返回值】无
【参数值】无
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
void Test_KEY(void)
{ 
    LED_Init();
    KEY_Init(); 
    while (1)
    {  
        //测试按键      

        switch(KEY_Read(0))  //
        {
            case 1:
                LED_Reverse(0);
                break;           
            case 2:      
                LED_Reverse(1);
                break;
            case 3:      
                LED_Reverse(2);
                break;
            default:
                LED_Reverse(3);
                break;
        }
        //延时
        delayms(50);
    }
}

//------------------------------------------------------------------------------------------------    
//全局变量  用于测试按键外部中断
//------------------------------------------------------------------------------------------------ 
volatile uint8_t key_exti_flag = 0;
/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
【作  者】LQ-005
【功能说明】测试按键  
【软件版本】V1.0
【最后更新】2017年11月24日 
【函数名】
【返回值】无
【参数值】无
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
void Test_ExtiKEY(void)
{ 
    LED_Init();
    
    /* 内部上拉 下降沿触发中断 中断服务函数在 IRQ_Handler.c */
    GPIO_ExtiInit(PTB20, falling_up);
    GPIO_ExtiInit(PTB21, falling_up);
    GPIO_ExtiInit(PTB22, falling_up);
    
    /* 优先级配置 抢占优先级1  子优先级2   越小优先级越高  抢占优先级可打断别的中断 */
    NVIC_SetPriority(PORTB_IRQn,NVIC_EncodePriority(NVIC_GetPriorityGrouping(),1,2));
    NVIC_EnableIRQ(PORTB_IRQn);			         //使能PORTB_IRQn的中断  
    while (1)
    {  
        //测试按键      

        switch(key_exti_flag)  //
        {
            case 1:
                LED_Reverse(0);
                break;           
            case 2:      
                LED_Reverse(1);
                break;
            case 3:      
                LED_Reverse(2);
                break;
            default:
                LED_Reverse(3);
                break;
        }
        //延时
        delayms(50);
    }
}


/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
【作  者】LQ-005
【功能说明】总测试函数  
【软件版本】V1.0
【最后更新】2019年5月24日 
【函数名】
【返回值】无
【参数值】无
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/

//Test_ICM20602();              //测试ICM20602模块
	//VE2母板测试函数，通过按键选择要测试的模块，左加右减，中间键进入测试，复位键回到初始界面。
	//测试LED，舵机，电机，神眼(使用新版转接头)，正交解码，ADC，九轴测试
#define     Step_Middle      750 //总线频率改变，中值需要重新标定
void Test_ALL(void) 
{
      KEY_Init();
      OLED_Init();
      int LPULSE=0,RPULSE=0;
      char txt[16]="X:";    
      static int key_count=1;
      volatile int key_ok = 0;
      while(1)
      { 
        OLED_P6x8Str(20,0,"LQ_Test_ALL");
        OLED_P6x8Str(8,1,"LED_test");
        OLED_P6x8Str(8,2,"Servo_test");
        OLED_P6x8Str(8,3,"Motor_test");
        OLED_P6x8Str(8,4,"LQV034_test");
        OLED_P6x8Str(8,5,"FTM_AB_test");
        OLED_P6x8Str(8,6,"ADC_test");
        OLED_P6x8Str(8,7,"9AX_test");  //九轴测试
        switch(KEY_Read(0))
        {
        case 1:
          key_ok=1;
          break;
        case 2:
          key_count--;
          break;
        case 3:
          key_count++;
          break;
        }
          if(key_count > 7)  key_count = 1;
          if(key_count < 1)  key_count = 7;
        if(key_count==1)
        {
          OLED_P6x8Str(0,1,(uint8_t*)"*");
          OLED_P6x8Str(0,2,(uint8_t*)" ");
          OLED_P6x8Str(0,3,(uint8_t*)" ");
          OLED_P6x8Str(0,4,(uint8_t*)" ");
          OLED_P6x8Str(0,5,(uint8_t*)" ");
          OLED_P6x8Str(0,6,(uint8_t*)" ");
          OLED_P6x8Str(0,7,(uint8_t*)" ");
          while(key_ok)
          {
          OLED_CLS();
          OLED_P6x8Str(1,1,"LED_test");
           while(key_ok)
           {
              Test_LED();
           } 
          }
        }
       else if(key_count==2)
       {
          OLED_P6x8Str(0,1,(uint8_t*)" ");
          OLED_P6x8Str(0,2,(uint8_t*)"*");
          OLED_P6x8Str(0,3,(uint8_t*)" ");
          OLED_P6x8Str(0,4,(uint8_t*)" ");
          OLED_P6x8Str(0,5,(uint8_t*)" ");
          OLED_P6x8Str(0,6,(uint8_t*)" ");
          OLED_P6x8Str(0,7,(uint8_t*)" ");
          CMT_PwmInit(50, 0);
          int servopwm=0;
          float time=0;
         while(key_ok)
         {
          OLED_CLS();
          OLED_P6x8Str(1,1,"Servo_test");      //第一次使用舵机的同学，调试之前最好不要装车防止舵机卡死堵转烧坏
          while(key_ok)
         {
          switch(KEY_Read(0))  //检测按键 
              {
                  case 0:
                  break;
                  case 3:
                  servopwm -= 25; 
                  if(servopwm <-350) servopwm =-350;                //PWM限幅，注意对舵机极限值进行限幅，避免堵转
                  CMT_PwmDuty(Step_Middle+servopwm);     //50Hz 一个周期20ms  1.5 / 20 * CMT_PRECISON  = 750
                  break;           
                  case 1:           
                  servopwm = 0;                                     //PWM限幅，注意对舵机极限值进行限幅，避免堵转
                  CMT_PwmDuty(Step_Middle+servopwm);     //50Hz 一个周期20ms  1.5 / 20 * CMT_PRECISON  = 750
                  break;
                  case 2:           
                  servopwm += 25;
                  if(servopwm >350) servopwm =350;                  //PWM限幅，注意对舵机极限值进行限幅，避免堵转
                  CMT_PwmDuty(Step_Middle+servopwm);     //50Hz 一个周期20ms  1.5 / 20 * CMT_PRECISON  = 750
                  break;
              }
            time=(Step_Middle+servopwm)*20.0/10000.0;               //计算高电平时间
            sprintf((char*)txt,"time:%4.3f ms",time);
            OLED_P6x8Str(0,5,(uint8 *)txt);                         //OLED显示高电平时间
            OLED_P6x8Str(0,4,"High power level time");
            OLED_P6x8Str(0,6,"Fre:50Hz");
            sprintf((char*)txt,"servopwm:%06d",Step_Middle+servopwm);
            OLED_P6x8Str(0,3,(uint8 *)txt);
           }
         }
       }
       else if(key_count==3)
       {
          OLED_P6x8Str(0,1,(uint8_t*)" ");
          OLED_P6x8Str(0,2,(uint8_t*)" ");
          OLED_P6x8Str(0,3,(uint8_t*)"*");
          OLED_P6x8Str(0,4,(uint8_t*)" ");
          OLED_P6x8Str(0,5,(uint8_t*)" ");
          OLED_P6x8Str(0,6,(uint8_t*)" ");
          OLED_P6x8Str(0,7,(uint8_t*)" ");
        FTM_PwmInit(FTM0, FTM_CH0, 12000, 0);
        FTM_PwmInit(FTM0, FTM_CH1, 12000, 0);
        FTM_PwmInit(FTM0, FTM_CH2, 12000, 0);
        FTM_PwmInit(FTM0, FTM_CH3, 12000, 0); 
          short motorpwm=0;
          int ratio=0;
        while(key_ok)
        {
          OLED_CLS();
          OLED_P6x8Str(1,1,"Motor_test"); 
        while(key_ok)
        {
         switch(KEY_Read(0))  //检测按键
              {
                  case 0:
                  break;
                  case 3:
                  motorpwm -= 1000;
                  if(motorpwm <-9000) motorpwm = -9000;
                  if(motorpwm < 0) 
                  {
                  FTM_PwmDuty(FTM0, FTM_CH0,-motorpwm);
                  FTM_PwmDuty(FTM0, FTM_CH1,0);
                  FTM_PwmDuty(FTM0, FTM_CH2,-motorpwm);
                  FTM_PwmDuty(FTM0, FTM_CH3,0);
                  }
                  else if(motorpwm>0) 
                  {
                  FTM_PwmDuty(FTM0, FTM_CH0,0);
                  FTM_PwmDuty(FTM0, FTM_CH1,motorpwm);
                  FTM_PwmDuty(FTM0, FTM_CH2,0);
                  FTM_PwmDuty(FTM0, FTM_CH3,motorpwm);
                  }
                  break;           
                  case 2:           
                  motorpwm += 1000;
                  if(motorpwm > 9000) motorpwm = 9000;
                  FTM_PwmDuty(FTM0, FTM_CH0,0);
                  FTM_PwmDuty(FTM0, FTM_CH1,motorpwm);
                  FTM_PwmDuty(FTM0, FTM_CH2,0);
                  FTM_PwmDuty(FTM0, FTM_CH3,motorpwm);
                  break;
                  case 1:
                  motorpwm = 0;
                  FTM_PwmDuty(FTM0, FTM_CH0,motorpwm);
                  FTM_PwmDuty(FTM0, FTM_CH1,motorpwm);
                  FTM_PwmDuty(FTM0, FTM_CH2,motorpwm);
                  FTM_PwmDuty(FTM0, FTM_CH3,motorpwm);
                  break;
              }
            
            ratio=motorpwm/100;
            if(ratio<0) ratio=-ratio;
            sprintf((char*)txt,"ratio:%02d %",ratio);
            OLED_P6x8Str(0,4,(uint8 *)txt);
            
            sprintf((char*)txt,"motorpwm:%06d",motorpwm);
            OLED_P6x8Str(0,3,(uint8 *)txt);
            }   
          }
        }
       else if(key_count==4)
       {
          OLED_P6x8Str(0,1,(uint8_t*)" ");
          OLED_P6x8Str(0,2,(uint8_t*)" ");
          OLED_P6x8Str(0,3,(uint8_t*)" ");
          OLED_P6x8Str(0,4,(uint8_t*)"*");
          OLED_P6x8Str(0,5,(uint8_t*)" ");
          OLED_P6x8Str(0,6,(uint8_t*)" ");
          OLED_P6x8Str(0,7,(uint8_t*)" ");
           
         while(key_ok)
        {
          CAMERA_Init();
          OLED_CLS();                    //清屏幕
          OLED_Show_Frame94();
          OLED_P6x8Str(1,1,"LQV034_test");
         while(key_ok)
         {
              while(Field_Over_Flag)
               {
              Get_Use_Image();
              Get_01_Value(0);    //大津法二值化
              Draw_Road();        //OLED显示
              Field_Over_Flag= 0;
               }
            }
          }
       }
       else if(key_count==5)
         {
          OLED_P6x8Str(0,1,(uint8_t*)" ");
          OLED_P6x8Str(0,2,(uint8_t*)" ");
          OLED_P6x8Str(0,3,(uint8_t*)" ");
          OLED_P6x8Str(0,4,(uint8_t*)" ");
          OLED_P6x8Str(0,5,(uint8_t*)"*");
          OLED_P6x8Str(0,6,(uint8_t*)" ");
          OLED_P6x8Str(0,7,(uint8_t*)" ");
          FTM_ABInit(FTM1);             //编码器初始化 PTA12   PTA13
          FTM_ABInit(FTM2);             //编码器初始化 PTB18   PTB19
    //      PIT_Init(PIT0,50);          //PIT定时中断初始化 
    //      NVIC_SetPriority(PIT0_IRQn,NVIC_EncodePriority(NVIC_GetPriorityGrouping(),1,2));
    //      NVIC_EnableIRQ(PIT0_IRQn);	
          while(key_ok)
          {
          OLED_CLS();
          OLED_P6x8Str(1,1,"FTM_AB_test"); 
          while(key_ok)
          {
           LPULSE=FTM_ABGet(FTM1);//读取速度数值
           RPULSE=-FTM_ABGet(FTM2); 
           sprintf((char*)txt,"LPULSE:%06d A12 A13",LPULSE);
           OLED_P6x8Str(0,2,(uint8 *)txt);
           sprintf((char*)txt,"RPULSE:%06d B18 B19",RPULSE);
           OLED_P6x8Str(0,3,(uint8 *)txt);
           delayms(100);
          }
        }
       }
       else if(key_count==6)
         {
          OLED_P6x8Str(0,1,(uint8_t*)" ");
          OLED_P6x8Str(0,2,(uint8_t*)"");
          OLED_P6x8Str(0,3,(uint8_t*)" ");
          OLED_P6x8Str(0,4,(uint8_t*)" ");
          OLED_P6x8Str(0,5,(uint8_t*)" ");
          OLED_P6x8Str(0,6,(uint8_t*)"*");
          OLED_P6x8Str(0,7,(uint8_t*)" ");
          ADC_Init(ADC1);
          uint16_t adc1=0,adc2=0,adc3=0,adc4=0;
           while(key_ok)
          {
          OLED_CLS();
          OLED_P6x8Str(1,1,"ADC_test"); 
          while(key_ok)
          {
           adc1=ADC_Ave(ADC1,ADC1_SE10,ADC_12bit,50);   //PTB4
           adc2=ADC_Ave(ADC1,ADC1_SE11,ADC_12bit,50);   //PTB5
           adc3=ADC_Ave(ADC1,ADC1_SE12,ADC_12bit,50);   //PTB6
           adc4=ADC_Ave(ADC1,ADC1_SE13,ADC_12bit,50);   //PTB7
           sprintf((char*)txt,"adc1:%06d B4",adc1);
           OLED_P6x8Str(0,2,(uint8 *)txt);
           
           sprintf((char*)txt,"adc2:%06d B5",adc2);
           OLED_P6x8Str(0,3,(uint8 *)txt);
           
           sprintf((char*)txt,"adc3:%06d B6",adc3);
           OLED_P6x8Str(0,4,(uint8 *)txt);
           
           sprintf((char*)txt,"adc4:%06d B7",adc4);
           OLED_P6x8Str(0,5,(uint8 *)txt);
          }
        }
       }
       else if(key_count==7)
         {
          OLED_P6x8Str(0,1,(uint8_t*)" ");
          OLED_P6x8Str(0,2,(uint8_t*)" ");
          OLED_P6x8Str(0,3,(uint8_t*)" ");
          OLED_P6x8Str(0,4,(uint8_t*)" ");
          OLED_P6x8Str(0,5,(uint8_t*)" ");
          OLED_P6x8Str(0,6,(uint8_t*)" ");
          OLED_P6x8Str(0,7,(uint8_t*)"*");
           while(key_ok)
        {
          OLED_CLS();
          OLED_P6x8Str(1,1,"9AX_test"); //测试九轴
          while(key_ok)
          {
           Test_9AX();
          }
        }
      }
    }
}

