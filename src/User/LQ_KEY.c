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
-------------------------------------------------------------------------------------------------------*/
#include "include.h"

/*------------------------------------------------------------------------------------------------------
����    ����LED_Init
����    �ܡ���ʼ�����İ��ĸ���ϵ�LED
����    ������
���� �� ֵ����
��ʵ    ����LED_Init(); //��ʼ��LED
��ע�����
--------------------------------------------------------------------------------------------------------*/
void KEY_Init(void)
{
    /* ��ʼ��ĸ���ϵ�KEY */
   GPIO_PinInit(PTB20, GPI_UP, 1);
   GPIO_PinInit(PTB21, GPI_UP, 1);
   GPIO_PinInit(PTB22, GPI_UP, 1);
}

/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
����  �ߡ�LQ-005
������˵��������������
������汾��V1.0
�������¡�2017��11��24�� 
����������
������ֵ��0���ް������� 1������1����  2:����2����  3:����3����
������ֵ��mode:0,��֧��������;1,֧��������;
������  ��KEY_Read(0);   //������������ĸ�����
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
uint8_t KEY_Read(uint8_t mode)
{
    static uint8_t key_up=1;     //�����ɿ���־
    if(mode==1) 
    {
      key_up=1;      //֧������
    }
    if(key_up && (GPIO_PinRead(PTB20)==0 || GPIO_PinRead(PTB21)==0 || GPIO_PinRead(PTB22)==0))
    {
      delayms(100);   //����
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
    return 0;   //�ް�������
     
}

/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
����  �ߡ�LQ-005
������˵�������԰���  
������汾��V1.0
�������¡�2017��11��24�� 
����������
������ֵ����
������ֵ����
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
void Test_KEY(void)
{ 
    LED_Init();
    KEY_Init(); 
    while (1)
    {  
        //���԰���      

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
        //��ʱ
        delayms(50);
    }
}

//------------------------------------------------------------------------------------------------    
//ȫ�ֱ���  ���ڲ��԰����ⲿ�ж�
//------------------------------------------------------------------------------------------------ 
volatile uint8_t key_exti_flag = 0;
/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
����  �ߡ�LQ-005
������˵�������԰���  
������汾��V1.0
�������¡�2017��11��24�� 
����������
������ֵ����
������ֵ����
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
void Test_ExtiKEY(void)
{ 
    LED_Init();
    
    /* �ڲ����� �½��ش����ж� �жϷ������� IRQ_Handler.c */
    GPIO_ExtiInit(PTB20, falling_up);
    GPIO_ExtiInit(PTB21, falling_up);
    GPIO_ExtiInit(PTB22, falling_up);
    
    /* ���ȼ����� ��ռ���ȼ�1  �����ȼ�2   ԽС���ȼ�Խ��  ��ռ���ȼ��ɴ�ϱ���ж� */
    NVIC_SetPriority(PORTB_IRQn,NVIC_EncodePriority(NVIC_GetPriorityGrouping(),1,2));
    NVIC_EnableIRQ(PORTB_IRQn);			         //ʹ��PORTB_IRQn���ж�  
    while (1)
    {  
        //���԰���      

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
        //��ʱ
        delayms(50);
    }
}


/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
����  �ߡ�LQ-005
������˵�����ܲ��Ժ���  
������汾��V1.0
�������¡�2019��5��24�� 
����������
������ֵ����
������ֵ����
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/

//Test_ICM20602();              //����ICM20602ģ��
	//VE2ĸ����Ժ�����ͨ������ѡ��Ҫ���Ե�ģ�飬����Ҽ����м��������ԣ���λ���ص���ʼ���档
	//����LED����������������(ʹ���°�ת��ͷ)���������룬ADC���������
#define     Step_Middle      750 //����Ƶ�ʸı䣬��ֵ��Ҫ���±궨
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
        OLED_P6x8Str(8,7,"9AX_test");  //�������
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
          OLED_P6x8Str(1,1,"Servo_test");      //��һ��ʹ�ö����ͬѧ������֮ǰ��ò�Ҫװ����ֹ���������ת�ջ�
          while(key_ok)
         {
          switch(KEY_Read(0))  //��ⰴ�� 
              {
                  case 0:
                  break;
                  case 3:
                  servopwm -= 25; 
                  if(servopwm <-350) servopwm =-350;                //PWM�޷���ע��Զ������ֵ�����޷��������ת
                  CMT_PwmDuty(Step_Middle+servopwm);     //50Hz һ������20ms  1.5 / 20 * CMT_PRECISON  = 750
                  break;           
                  case 1:           
                  servopwm = 0;                                     //PWM�޷���ע��Զ������ֵ�����޷��������ת
                  CMT_PwmDuty(Step_Middle+servopwm);     //50Hz һ������20ms  1.5 / 20 * CMT_PRECISON  = 750
                  break;
                  case 2:           
                  servopwm += 25;
                  if(servopwm >350) servopwm =350;                  //PWM�޷���ע��Զ������ֵ�����޷��������ת
                  CMT_PwmDuty(Step_Middle+servopwm);     //50Hz һ������20ms  1.5 / 20 * CMT_PRECISON  = 750
                  break;
              }
            time=(Step_Middle+servopwm)*20.0/10000.0;               //����ߵ�ƽʱ��
            sprintf((char*)txt,"time:%4.3f ms",time);
            OLED_P6x8Str(0,5,(uint8 *)txt);                         //OLED��ʾ�ߵ�ƽʱ��
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
         switch(KEY_Read(0))  //��ⰴ��
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
          OLED_CLS();                    //����Ļ
          OLED_Show_Frame94();
          OLED_P6x8Str(1,1,"LQV034_test");
         while(key_ok)
         {
              while(Field_Over_Flag)
               {
              Get_Use_Image();
              Get_01_Value(0);    //��򷨶�ֵ��
              Draw_Road();        //OLED��ʾ
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
          FTM_ABInit(FTM1);             //��������ʼ�� PTA12   PTA13
          FTM_ABInit(FTM2);             //��������ʼ�� PTB18   PTB19
    //      PIT_Init(PIT0,50);          //PIT��ʱ�жϳ�ʼ�� 
    //      NVIC_SetPriority(PIT0_IRQn,NVIC_EncodePriority(NVIC_GetPriorityGrouping(),1,2));
    //      NVIC_EnableIRQ(PIT0_IRQn);	
          while(key_ok)
          {
          OLED_CLS();
          OLED_P6x8Str(1,1,"FTM_AB_test"); 
          while(key_ok)
          {
           LPULSE=FTM_ABGet(FTM1);//��ȡ�ٶ���ֵ
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
          OLED_P6x8Str(1,1,"9AX_test"); //���Ծ���
          while(key_ok)
          {
           Test_9AX();
          }
        }
      }
    }
}

