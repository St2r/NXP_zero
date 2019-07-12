/*---------------------------------------------------------------
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
----------------------------------------------------------------*/
#include "include.h"

uint8_t Image_Data[IMAGEH][IMAGEW];      //ͼ��ԭʼ���ݴ��
uint8_t Image_Use[LCDH][LCDW];           //ѹ����֮�����ڴ����Ļ��ʾ����
uint8_t Pixle[LCDH][LCDW];               //��ֵ��������OLED��ʾ������
uint8_t Road[LCDH][LCDW];               //��ֵ��������OLED��ʾ������
uint8_t LineLeft[LCDH];
uint8_t LineRight[LCDH];
volatile uint8_t  Field_Over_Flag=0;              //����ʶ

/*---------------------------------------------------------------
����    ����DMA_CH0_Handler
����    �ܡ�DMAͨ��4���жϷ�����
����    ������
���� �� ֵ����
��ע�����ע������Ҫ����жϱ�־λ
----------------------------------------------------------------*/
void DMA0_IRQHandler(void)
{
    /* ���ͨ�������жϱ�־λ    (���������ٴν����ж�) */
    DMA_IRQ_CLEAN(DMA_CH0);          

    Field_Over_Flag=1;     //��������ʶ                                      
}

/*---------------------------------------------------------------
����    ����PORTC_Interrupt
����    �ܡ�PORTC�˿ڵ��жϷ����� ��������ͷ�г��ж�
����    ������
���� �� ֵ����
��ע�����ע������Ҫ����жϱ�־λ
----------------------------------------------------------------*/
void PORTC_IRQHandler(void)
{     
    
    if((PORTC_ISFR & 0x0080))//(1<<7)
    {    
        // �û����� 
        if(0 == Field_Over_Flag)
        {          
            DMATransDataStart(DMA_CH0,(uint32_t)(&Image_Data[0][0]));   //����DMA���� 
        } 
        PORTC_ISFR |= (0x0080);  //����жϱ�ʶ
    } 
}

/*---------------------------------------------------------------
����    ����CAMERA_Init
����    �ܡ���ʼ������ͷ 
����    ������
���� �� ֵ����
��ע�����ע�ⲻҪʹ�� ��PD15
----------------------------------------------------------------*/
void CAMERA_Init(void)
{
    /*  ����ͷ��ʼ�� */
    LQMT9V034_Init(0);            //����ͷ��ʼ��
    
    //����ͷ�йؽӿڳ�ʼ��
    GPIO_ExtiInit(PTC7,rising_down);    //���ж�   
    
    /* ���ȼ����� ��ռ���ȼ�0  �����ȼ�2   ԽС���ȼ�Խ��  ��ռ���ȼ��ɴ�ϱ���ж� */
    NVIC_SetPriority(PORTC_IRQn,NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0,2));
    NVIC_EnableIRQ(PORTC_IRQn);		   //ʹ��PORTC_IRQn���ж� 
    
    //��λ���������
    GPIO_PinInit(PTD8,GPI,0);                
    GPIO_PinInit(PTD9,GPI,0);
    GPIO_PinInit(PTD10,GPI,0);
    GPIO_PinInit(PTD11,GPI,0);
    GPIO_PinInit(PTD12,GPI,0);
    GPIO_PinInit(PTD13,GPI,0);
    GPIO_PinInit(PTD14,GPI,0);
    GPIO_PinInit(PTD15,GPI,0);     //ע�ⲻҪʹ�õ�PD15
    
    ///��ʼ��DMA�ɼ�       ͨ��0        PTD9-PTD15           Ŀ�ĵ�ַ      �����ź� ÿ��һ��BYTE  ����            �½����ش���
    DMA_PORTx2BUFF_Init (DMA_CH0, (void *)&PTD_BYTE1_IN,(void*)Image_Data, PTE6, DMA_BYTE1, ((IMAGEH)*(IMAGEW)), DMA_falling_down_keepon); 
    /* ���ȼ����� ��ռ���ȼ�0  �����ȼ�2   ԽС���ȼ�Խ��  ��ռ���ȼ��ɴ�ϱ���ж� */
    NVIC_SetPriority(DMA0_IRQn,NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0,2));
    NVIC_EnableIRQ(DMA0_IRQn);		   //ʹ��DMA�ж�  
    /* ����ͷ��ʼ������ */
}


/*---------------------------------------------------------------
����    ����Test_Camera
����    �ܡ���������ͷ 
����    ������
���� �� ֵ����
��ע�����ע�ⲻҪʹ�� ��PD15
----------------------------------------------------------------*/
void Test_Camera(void)
{  
    LED_Init();
    OLED_Init();
    OLED_CLS();
    OLED_Show_Frame94();      //��ͼ�� LCDW*LCDH ���
    UART_Init(UART4, 115200);      //���ڳ�ʼ��
    /*  ����ͷ��ʼ�� */
    LQMT9V034_Init(0);            //����ͷ��ʼ��
    
    //����ͷ�йؽӿڳ�ʼ��
	GPIO_ExtiInit(PTC6, rising_down);   //���ж�
    GPIO_ExtiInit(PTC7,falling_up);    //���ж�   
    
    /* ���ȼ����� ��ռ���ȼ�0  �����ȼ�2   ԽС���ȼ�Խ��  ��ռ���ȼ��ɴ�ϱ���ж� */
    NVIC_SetPriority(PORTC_IRQn,NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0,2));
    NVIC_EnableIRQ(PORTC_IRQn);		   //ʹ��PORTC_IRQn���ж� 
    
    //��λ���������
    GPIO_PinInit(PTD8,GPI,0);                
    GPIO_PinInit(PTD9,GPI,0);
    GPIO_PinInit(PTD10,GPI,0);
    GPIO_PinInit(PTD11,GPI,0);
    GPIO_PinInit(PTD12,GPI,0);
    GPIO_PinInit(PTD13,GPI,0);
    GPIO_PinInit(PTD14,GPI,0);
    GPIO_PinInit(PTD15,GPI,0);     //ע�ⲻҪʹ�õ�PD15
    
    ///��ʼ��DMA�ɼ�       ͨ��0        PTD9-PTD15           Ŀ�ĵ�ַ      �����ź� ÿ��һ��BYTE  ����            �½����ش���
    DMA_PORTx2BUFF_Init (DMA_CH0, (void *)&PTD_BYTE1_IN,(void*)Image_Data, PTE6, DMA_BYTE1, ((IMAGEH)*(IMAGEW)), DMA_falling_down_keepon); 
    
	/* ���ȼ����� ��ռ���ȼ�0  �����ȼ�2   ԽС���ȼ�Խ��  ��ռ���ȼ��ɴ�ϱ���ж� */
    //NVIC_SetPriority(DMA0_IRQn,NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0,2));
    //NVIC_EnableIRQ(DMA0_IRQn);		   //ʹ��DMA�ж�  
    /* ����ͷ��ʼ������ */
    
    while(1)
    { 
        LED_Reverse(1);           //LEDָʾ��������״̬
        if(Field_Over_Flag)       //���һ��ͼ��ɼ�
        { 

			Get_Use_Image();      //�Ӳɼ�ͼ��������ȡ���Լ���Ҫʹ�õĴ�С
            Get_01_Value(1);      //��ֵ��ͼ������
            
            Draw_Road();          //����OLEDģ����ʾ��̬ͼ��
            Field_Over_Flag= 0;  
        }    
    }
}


/*---------------------------------------------------------------
����    ����Get_Use_Image
����    �ܡ���ȡ��Ҫʹ�õ�ͼ���С
����    ������
���� �� ֵ����
��ע�����
----------------------------------------------------------------*/
__ramfunc void Get_Use_Image(void)
{
  int i = 0,j = 0,row = 0,line = 0;
  uint8_t div_h = IMAGEH/LCDH;
  uint8_t div_w = IMAGEW/LCDW;
  for(i = 0; i  < IMAGEH; i += div_h)    //IMAGEH / div  = LCDH
  {
    for(j = 0; j < IMAGEW; j += div_w)   //IMAGEW / div  = LCDW
    {        
      Image_Use[row][line] = Image_Data[i][j];         
      line++;        
    }      
    line = 0;
    row++;      
  }  
}

/*---------------------------------------------------------------
����    ����Get_01_Value
����    �ܡ���ֵ��
����    ����mode  ��  0��ʹ�ô����ֵ    1��ʹ��ƽ����ֵ
���� �� ֵ����
��ע�����
----------------------------------------------------------------*/
void Get_01_Value(uint8_t mode)
{
  int i = 0,j = 0;
  uint8_t Threshold;
  uint32_t  tv=0;
  char txt[16];
  if(mode)
  {
      //�ۼ�
      for(i = 0; i <LCDH; i++)
      {    
          for(j = 0; j <LCDW; j++)
          {                            
              tv+=Image_Use[i][j];   //�ۼ�  
          } 
      }
      Threshold=tv/LCDH/LCDW;        //��ƽ��ֵ,����Խ��ԽС��ȫ��Լ35��������ĻԼ160��һ������´�Լ100
      Threshold=Threshold*7/10+10;   //�˴���ֵ���ã����ݻ����Ĺ������趨 
  }
  else
  {
      Threshold = GetOSTU(Image_Use);//�����ֵ
      Threshold = (uint8_t)(Threshold * 0.5) + 70;
  } 
  if (DebugCamera)
  {
	  sprintf(txt, "%03d", Threshold);//ǰ��Ϊ�����õ���ֵ������Ϊƽ��ֵ 
#ifdef LQ_OLED 
	  OLED_P6x8Str(80, 1, (u8*)txt);
#else
	  TFTSPI_P8X8Str(0, 15, (u8*)txt, u16RED, u16BLUE);
#endif
  }
  for(i = 0; i < LCDH; i++)
  {
    for(j = 0; j < LCDW; j++)
    {                                
      if(Image_Use[i][j] >Threshold) //��ֵԽ����ʾ������Խ�࣬��ǳ��ͼ��Ҳ����ʾ����    
        Pixle[i][j] =1;        
      else                                        
        Pixle[i][j] =0;
    }    
  }
}


//todo::ʶ�����
void Camera_Black(uint8_t *point,uint8_t *Road,uint8_t *LineLeft,uint8_t * LineRight)
{
	//60*94
	int16_t temp0, temp1;
	int16_t temp_value1, temp_value2;
	uint8_t leftedge, rightedge;
	uint8_t leftavg, rightavg;
	uint8_t leftflag, rightflag;
	uint8_t leftcount, rightcount;
	uint8_t LINEWITH, LEFTLINEFAV, RINGTLINFAV, FAV;
	static uint8_t centre_lastblack = 47;

	LINEWITH = 2;       //���߿�� 
		LEFTLINEFAV = 100;  //���½��������ѹ�仯ֵ��Сֵ
		RINGTLINFAV = 100;  //���½��������ѹ�仯ֵ��Сֵ
		FAV = 100;          //��ֵ����ֵ 
	leftcount = 0;     //����߿�ȼ���ֵ
	rightcount = 0;    //�Һ��߿�ȼ���ֵ 
	leftedge = 0;      //��߽� 
	rightedge = 0;     //�ұ߽�
	leftavg = 0;       //��������ߵ�ƽ��ֵ
	rightavg = 0;      //�����Һ��ߵ�ƽ��ֵ
	leftflag = 0;      //���߱�־λ
	rightflag = 0;     //�Ҷ��߱�־λ

	for (temp0 = 0; temp0 < 94; temp0++)
		Road[temp0] = 0;
	/*��������ؿ�ʼ���ҵ����غ�����ѭ��*/
	for (temp0 = centre_lastblack; temp0 > LINEWITH; temp0--)               //���������
	{
		for (temp1 = temp0; temp1 > (temp0 - LINEWITH); temp1--)      //Ѱ��һ�㸽���ļ������Ƿ��ܹ����½���
		{
			if (point[temp1] == 1) break;                                   //������ʵ����������ѭ��             

									             //�Ƚϰ׵㸽���ļ���ֵ
			if (point[temp1]==0)                          //�ж��Ƿ�����Ҫ��
			{
				leftcount++;                                        //������������ֵ��һ
		   //     testarry[temp0]=temp_value1;
			}

		}

		if (leftcount >= LINEWITH - 1)                        //�ж��Ƿ������������
		{
			leftedge = temp0;                                  //�������������ҵ�
			leftflag = 0;                                         //�ҵ����أ���־λ��1 
			break;
		}
		else
		{
			leftcount = 0;                                        //δ�ҵ��������  
			leftflag = 1;                                         //��־λ���
			leftedge = 5;                                         // ���ظ�ֵ 
		}

		if (leftedge < LINEWITH) leftedge = 5;
	}
	Road[leftedge] = 1;
	*LineLeft = leftedge;
	/*����ز��ҽ���*/

	/*�����ұ��ؿ�ʼ���ҵ����غ�����ѭ��*/
	for (temp0 = centre_lastblack; temp0 < 95-LINEWITH; temp0++)          //�ұ��ز��ҷ�ʽ�Ͳ��������һ��
	{
		for (temp1 = temp0; temp1 < (temp0 + LINEWITH); temp1++)
		{
			if (point[temp1]) break;

			

			if (point[temp1]==0)
			{
				rightcount++;
			}


		}

		if (rightcount >= LINEWITH - 1)
		{
			rightedge = temp0;
			rightflag = 0;
			break;
		}
		else
		{
			rightcount = 0;
			rightflag = 1;
			rightedge = 91;
		}

		if (rightedge > 91) rightedge = 91;

	}
	Road[rightedge] = 1;
	*LineRight = rightedge;
	/*�ұ��ز��ҽ���*/


	///*�ж����ҵ����صĿɿ���*/

	///*����صĿɿ����ж�*/
	//if (leftflag == 0)
	//{
	//	/*������ߵ�ƽ��ֵ*/
	//	leftcount = 0;
	//	for (temp0 = leftedge; temp0 > (leftedge - LINEWITH); temp0--)
	//	{
	//		if (temp0 <= 0)break;
	//		leftcount++;
	//		leftavg += point[temp0 + 1];
	//	}

	//	leftavg = leftavg / leftcount;


	//	/*�Ƚϰ׵׺ͺ���֮��Ĳ�ֵ*/
	//	leftcount = 0;
	//	for (temp0 = leftedge; temp0 < (leftedge + LINEWITH); temp0++)   //�жϱ��ظ����İ׵㣬�Ƿ���������
	//	{
	//		if (temp0 >= 155)break;

	//		temp_value1 = point[temp0 - 1] - leftavg;                  //�׵������ƽ��ֵ���Ƚ�
	//		if (temp_value1 >= LEFTLINEFAV)                         //�ж��Ƿ�����Ҫ��
	//		{
	//			leftcount++;                                         //�ҵ����أ���־λ��1 
	//		}
	//		else
	//		{
	//			leftcount = 0;                                         // ��־λ���
	//		}
	//	}

	//	if (leftcount > (LINEWITH - 2))                               //�ж��Ƿ������������
	//	{
	//		*leftblackedge = leftedge;                               //�������������㸳ֵ������
	//		leftedge = 1;
	//	}
	//	else
	//	{
	//		*leftblackedge = 5;
	//		leftcount = 0;                                          //δ�ҵ�������� 
	//		leftflag = 0;                                           //��־λ���
	//	}
	//}
	//else
	//{
	//	*leftblackedge = 5;
	//}
	///*��߽�ɿ����жϽ���*/



	///*�ұ߽�ɿ����жϿ�ʼ*/
	//if (rightflag == 0)                      // �ұ߽��жϿɿ��ԣ�����߽��жϷ�ʽһ��                  
	//{
	//	rightcount = 0;
	//	for (temp0 = rightedge; temp0 < (rightedge + LINEWITH); temp0++)
	//	{
	//		if (temp0 >= 155)break;
	//		rightcount++;
	//		rightavg += point[temp0 - 1];
	//	}

	//	rightavg = rightavg / rightcount;

	//	rightcount = 0;
	//	for (temp0 = rightedge; temp0 > (rightedge - LINEWITH); temp0--)
	//	{
	//		if (temp0 <= 0)break;

	//		temp_value2 = point[temp0] - rightavg;
	//		if (temp_value2 >= RINGTLINFAV)
	//		{
	//			rightcount++;
	//		}
	//		else
	//		{
	//			rightcount = 0;
	//		}
	//	}

	//	if (rightcount > (LINEWITH - 2))
	//	{
	//		*rightblackedge = rightedge;
	//		rightedge = 1;
	//	}
	//	else
	//	{
	//		*rightblackedge = 155;
	//		rightcount = 0;
	//		rightflag = 0;
	//	}

	//}
	//else
	//{
	//	*rightblackedge = 155;
	//}

	///*�ұ߽�ɿ����жϽ���*/

	//if ((leftflag == 0) && (rightflag == 0))
	//{
	//	*centre_black = (uint8)((leftedge + rightedge) / 2);
	//	centre_lastblack = *centre_black;
	//}
	//else if ((leftflag == 0) && (rightflag == 1))
	//{
	//	*centre_black = (uint8)((leftedge + rightedge) / 2);
	//	centre_lastblack = *centre_black;
	//}
	//else if ((leftflag == 1) && (rightflag == 0))
	//{
	//	*centre_black = (uint8)((leftedge + rightedge) / 2);
	//	centre_lastblack = *centre_black;
	//}
	//else if ((leftflag == 1) && (rightflag == 1))
	//{
	//	*centre_black = centre_lastblack;
	//}

	//*leftblackedge = leftedge;
	//*rightblackedge = rightedge;

}


/*---------------------------------------------------------------
����    ����TFTSPI_Show_Cmera
����    �ܡ���TFT1.8�ϻ�������ͷ��ͼ��
����    ����xs:  ��ʼX   
����    ����ys:  ��ʼY  
����    ����w:   ͼƬ��� 
����    ����h:   ͼƬ�߶�  
����    ����ppic[IMAGEH][IMAGEW]:  ͼƬ������   
���� �� ֵ����
��ע�����
----------------------------------------------------------------*/	
void TFTSPI_Show_Cmera(uint8_t xs,uint8_t ys,uint8_t w,uint8_t h,uint8_t ppic[LCDH][LCDW]) 
{
#if 0    //�Ҷ���ʾ
    unsigned int i,j;	
    uint16_t color;
    TFTSPI_Set_Pos(xs,ys,xs+w-1,ys+h);
    for(i = 0; i < h; i += 1)     
    { 	
        for(j = 0; j < w; j += 1) 
        {
           
            color = 0;
            color=((ppic[i][j]>>3))<<11;
            color=color|(((ppic[i][j]>>2))<<5);
            color=color|(((ppic[i][j])>>3));
            TFTSPI_Write_Word(color);
        }
        
    }
#else    //��ֵ����ʾ
    unsigned int i,j;	
    Get_01_Value(1);
    TFTSPI_Set_Pos(xs,ys,xs+w-1,ys+h);
    for(i = 0; i < h; i += 1)     
    { 	
        for(j = 0; j < w; j += 1) 
        {
           
            if(Pixle[i][j])
            {
                TFTSPI_Write_Word(0xFFFF);
            }
            else
            {
                TFTSPI_Write_Word(0);
            }      
        }
        
    }
#endif
 }
/*---------------------------------------------------------------
����    ����Draw_Road
����    �ܡ���OLED�ϻ�������ͷ��ͼ��
����    ������
���� �� ֵ����
��ע�����
----------------------------------------------------------------*/
void Draw_Road(void)
{ 	 
  u8 i = 0, j = 0,temp=0;
  
  //����֡ͷ��־
  for(i=8;i<56;i+=8)//6*8=48�� 
  {
    OLED_Set_Pos(18,i/8+1);//��ʼλ��
    for(j=0;j<LCDW;j++)  //����
    { 
      temp=0; 
      if(Pixle[0+i][j]) temp|=1;
      if(Pixle[1+i][j]) temp|=2;
      if(Pixle[2+i][j]) temp|=4;
      if(Pixle[3+i][j]) temp|=8;
      if(Pixle[4+i][j]) temp|=0x10;
      if(Pixle[5+i][j]) temp|=0x20;
      if(Pixle[6+i][j]) temp|=0x40;
      if(Pixle[7+i][j]) temp|=0x80;
      OLED_WrDat(temp); 	  	  	  	  
    }
  }  
}

/*---------------------------------------------------------------
����    ����Draw_Road
����    �ܡ���OLED�ϻ���ʶ�����·����ͼ��
����    ������
���� �� ֵ����
��ע�����
----------------------------------------------------------------*/
void Draw_Road_alt(void)
{
	u8 i = 0, j = 0, temp = 0;

	//����֡ͷ��־
	for (i = 8; i < 56; i += 8)//6*8=48�� 
	{
		OLED_Set_Pos(18, i / 8 + 1);//��ʼλ��
		for (j = 0; j < LCDW; j++)  //����
		{
			temp = 0;
			if (Road[0 + i][j]) temp |= 1;
			if (Road[1 + i][j]) temp |= 2;
			if (Road[2 + i][j]) temp |= 4;
			if (Road[3 + i][j]) temp |= 8;
			if (Road[4 + i][j]) temp |= 0x10;
			if (Road[5 + i][j]) temp |= 0x20;
			if (Road[6 + i][j]) temp |= 0x40;
			if (Road[7 + i][j]) temp |= 0x80;
			OLED_WrDat(temp);
		}
	}
}

/*---------------------------------------------------------------
����    ����Pixle_Filter
����    �ܡ��������
����    ������
���� �� ֵ����
��ע�����
----------------------------------------------------------------*/
void Pixle_Filter(void)
{  
  int nr; //��
  int nc; //��
  
  for(nr=1; nr<LCDH-1; nr++)
  {  	    
    for(nc=1; nc<LCDW-1; nc=nc+1)
    {
      if((Pixle[nr][nc]==0)&&(Pixle[nr-1][nc]+Pixle[nr+1][nc]+Pixle[nr][nc+1]+Pixle[nr][nc-1]>2))         
      {
        Pixle[nr][nc]=1;
      }	
      else if((Pixle[nr][nc]==1)&&(Pixle[nr-1][nc]+Pixle[nr+1][nc]+Pixle[nr][nc+1]+Pixle[nr][nc-1]<2))         
      {
        Pixle[nr][nc]=0;
      }	
    }	  
  }  
}



/***************************************************************
* 
* �������ƣ�SendPicture 
* ����˵��������ͼ����λ�� ����ͬ����λ��ע���޸Ķ�Ӧ�����ݽ���Э��
* ����˵���� 
* �������أ�void 
* �޸�ʱ�䣺2018��3��27�� 
* �� ע�� 
***************************************************************/ 
void UARTSendPicture(uint8_t  tmImage[IMAGEH][IMAGEW]) 
{ 
  int i = 0, j = 0; 
  UART_PutChar(UART4,0x01); //����֡ͷ��־ WindowsFormsApplication1.exe
  UART_PutChar(UART4,0xfe); //����֡ͷ��־ WindowsFormsApplication1.exe
  for(i=0;i < IMAGEH; i++) 
  { 
    for(j=0;j <IMAGEW ;j++) 
    { 
      if(tmImage[i][j]==0xfe) 
      { 
        tmImage[i][j]=0xff; //��ֹ���ͱ�־λ 
      } 
      UART_PutChar(UART4,tmImage[i][j]); 
    } 
  }
  UART_PutChar(UART4,0xfe); //����֡β��־ 
  UART_PutChar(UART4,0x01); //����֡β��־ 
} 

/*************************************************************** 
* 
* �������ƣ�uint8_t GetOSTU(uint8_t tmImage[IMAGEH][IMAGEW]) 
* ����˵��������ֵ��С 
* ����˵���� 
* �������أ���ֵ��С 
* �޸�ʱ�䣺2018��3��27�� 
* �� ע�� 
�ο���https://blog.csdn.net/zyzhangyue/article/details/45841255
      https://www.cnblogs.com/moon1992/p/5092726.html
      https://www.cnblogs.com/zhonghuasong/p/7250540.html     
Ostu������������������ͨ��ͳ������ͼ���ֱ��ͼ������ʵ��ȫ����ֵT���Զ�ѡȡ�����㷨����Ϊ��
1) �ȼ���ͼ���ֱ��ͼ������ͼ�����е����ص㰴��0~255��256��bin��ͳ������ÿ��bin�����ص�����
2) ��һ��ֱ��ͼ��Ҳ����ÿ��bin�����ص����������ܵ����ص�
3) i��ʾ�������ֵ��Ҳ��һ���Ҷȼ�����0��ʼ����
4) ͨ����һ����ֱ��ͼ��ͳ��0~i �Ҷȼ�������(��������ֵ�ڴ˷�Χ�����ؽ���ǰ������) ��ռ����ͼ��ı���w0����ͳ��ǰ�����ص�ƽ���Ҷ�u0��ͳ��i~255�Ҷȼ�������(��������ֵ�ڴ˷�Χ�����ؽ�����������) ��ռ����ͼ��ı���w1����ͳ�Ʊ������ص�ƽ���Ҷ�u1��
5) ����ǰ�����غͱ������صķ��� g = w0*w1*(u0-u1) (u0-u1)
6) i++��ת��4)��ֱ��iΪ256ʱ��������
7�������g��Ӧ��iֵ��Ϊͼ���ȫ����ֵ
ȱ��:OSTU�㷨�ڴ�����ղ����ȵ�ͼ���ʱ��Ч�������Բ��ã���Ϊ���õ���ȫ��������Ϣ��
������ղ�����  https://blog.csdn.net/kk55guang2/article/details/78475414
                https://blog.csdn.net/kk55guang2/article/details/78490069
				https://wenku.baidu.com/view/84e5eb271a37f111f0855b2d.html
***************************************************************/ 
int GetOSTU(uint8_t tmImage[LCDH][LCDW]) 
{ 
/**
  * @brief    δ�Ż����Ĵ��
  *
  * @param    ����ʱ��Ƚϳ�
  *
  * @return   ʵ��120*160��ͼ��
  *
  * @note     K66 220MHz��Ҫ9ms
  *
  * @example  
  *
  * @date     2019/4/16 ���ڶ�
  */
//	int width = LCDW;
//	int height = LCDH;
//	int x = 0, y = 0;
//	int pixelCount[256];
//	float pixelPro[256];
//	int i, j, pixelSum = width * height, threshold = 0;
//	
//	
//	//��ʼ��
//	for (i = 0; i < 256; i++)
//	{
//		pixelCount[i] = 0;
//		pixelPro[i] = 0;
//	}
//	
//	//ͳ�ƻҶȼ���ÿ������������ͼ���еĸ���
//	for (i = y; i < height; i++)
//	{
//		for (j = x; j <width; j++)
//		{
//			pixelCount[tmImage[i][j]]++;
//		}
//	}
//	
//	//����ÿ������������ͼ���еı���
//	for (i = 0; i < 256; i++)
//	{
//		pixelPro[i] = (float)(pixelCount[i]) / (float)(pixelSum);
//	}
//	
//	//����ostu�㷨,�õ�ǰ���ͱ����ķָ�
//	//�����Ҷȼ�[0,255],������������ĻҶ�ֵ,Ϊ�����ֵ
//	float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;
//	for (i = 0; i < 256; i++)
//	{
//		w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
//		
//		for (j = 0; j < 256; j++)
//		{
//			if (j <= i) //��������
//			{
//				//��iΪ��ֵ���࣬��һ���ܵĸ���
//				w0 += pixelPro[j];
//				u0tmp += j * pixelPro[j];
//			}
//			else       //ǰ������
//			{
//				//��iΪ��ֵ���࣬�ڶ����ܵĸ���
//				w1 += pixelPro[j];
//				u1tmp += j * pixelPro[j];
//			}
//		}
//		
//		u0 = u0tmp / w0;        //��һ���ƽ���Ҷ�
//		u1 = u1tmp / w1;        //�ڶ����ƽ���Ҷ�
//		u = u0tmp + u1tmp;        //����ͼ���ƽ���Ҷ�
//		//������䷽��
//		deltaTmp = w0 * (u0 - u)*(u0 - u) + w1 * (u1 - u)*(u1 - u);
//		//�ҳ������䷽���Լ���Ӧ����ֵ
//		if (deltaTmp > deltaMax)
//		{
//			deltaMax = deltaTmp;
//			threshold = i;
//		}
//	}
//	//���������ֵ;
//	return threshold;
	
/**
  * @brief    �Ż����Ĵ��
  *
  * @param    ����������ʱ��
  *
  * @return   ʵ��K66 220MHz 120*160��ͼ��
  *
  * @note     ֻ��Ҫ1.5ms
  *
  * @example  δ�Ż��Ĵ����Ҫ9ms
  *
  * @date     2019/4/16 ���ڶ�
  */ 
	int16_t i,j; 
	uint32_t Amount = 0; 
	uint32_t PixelBack = 0; 
	uint32_t PixelIntegralBack = 0; 
	uint32_t PixelIntegral = 0; 
	int32_t PixelIntegralFore = 0; 
	int32_t PixelFore = 0; 
	float OmegaBack, OmegaFore, MicroBack, MicroFore, SigmaB, Sigma; // ��䷽��; 
	int16_t MinValue, MaxValue; 
	uint8_t Threshold = 0;
	uint8_t HistoGram[256];              //  
	
	for (j = 0; j < 256; j++)  HistoGram[j] = 0; //��ʼ���Ҷ�ֱ��ͼ 
	
	for (j = 0; j < LCDH; j++) 
	{ 
		for (i = 0; i < LCDW; i++) 
		{ 
			HistoGram[tmImage[j][i]]++; //ͳ�ƻҶȼ���ÿ������������ͼ���еĸ���
		} 
	} 
	
	for (MinValue = 0; MinValue < 256 && HistoGram[MinValue] == 0; MinValue++) ;        //��ȡ��С�Ҷȵ�ֵ
	for (MaxValue = 255; MaxValue > MinValue && HistoGram[MinValue] == 0; MaxValue--) ; //��ȡ���Ҷȵ�ֵ
	
	if (MaxValue == MinValue)     return MaxValue;         // ͼ����ֻ��һ����ɫ    
	if (MinValue + 1 == MaxValue)  return MinValue;        // ͼ����ֻ�ж�����ɫ
	
	for (j = MinValue; j <= MaxValue; j++)    Amount += HistoGram[j];        //  ��������
	
	PixelIntegral = 0;
	for (j = MinValue; j <= MaxValue; j++)
	{
		PixelIntegral += HistoGram[j] * j;//�Ҷ�ֵ����
	}
	SigmaB = -1;
	for (j = MinValue; j < MaxValue; j++)
	{
		PixelBack = PixelBack + HistoGram[j];   //ǰ�����ص���
		PixelFore = Amount - PixelBack;         //�������ص���
		OmegaBack = (float)PixelBack / Amount;//ǰ�����ذٷֱ�
		OmegaFore = (float)PixelFore / Amount;//�������ذٷֱ�
		PixelIntegralBack += HistoGram[j] * j;  //ǰ���Ҷ�ֵ
		PixelIntegralFore = PixelIntegral - PixelIntegralBack;//�����Ҷ�ֵ
		MicroBack = (float)PixelIntegralBack / PixelBack;   //ǰ���ҶȰٷֱ�
		MicroFore = (float)PixelIntegralFore / PixelFore;   //�����ҶȰٷֱ�
		Sigma = OmegaBack * OmegaFore * (MicroBack - MicroFore) * (MicroBack - MicroFore);//������䷽��
		if (Sigma > SigmaB)                    //����������䷽��g //�ҳ������䷽���Լ���Ӧ����ֵ
		{
			SigmaB = Sigma;
			Threshold = j;
		}
	}
	return Threshold;                        //���������ֵ;
} 

void Catch(void)
{
	if (Field_Over_Flag)       //���һ��ͼ��ɼ�
	{

		Get_Use_Image();      //�Ӳɼ�ͼ��������ȡ���Լ���Ҫʹ�õĴ�С
		Get_01_Value(1);      //��ֵ��ͼ������

		Draw_Road();          //����OLEDģ����ʾ��̬ͼ��
		Field_Over_Flag = 0;
	}
	LED_Reverse(1);
}
