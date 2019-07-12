/*---------------------------------------------------------------
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
----------------------------------------------------------------*/
#include "include.h"

uint8_t Image_Data[IMAGEH][IMAGEW];      //图像原始数据存放
uint8_t Image_Use[LCDH][LCDW];           //压缩后之后用于存放屏幕显示数据
uint8_t Pixle[LCDH][LCDW];               //二值化后用于OLED显示的数据
uint8_t Road[LCDH][LCDW];               //二值化后用于OLED显示的数据
uint8_t LineLeft[LCDH];
uint8_t LineRight[LCDH];
volatile uint8_t  Field_Over_Flag=0;              //场标识

/*---------------------------------------------------------------
【函    数】DMA_CH0_Handler
【功    能】DMA通道4的中断服务函数
【参    数】无
【返 回 值】无
【注意事项】注意进入后要清除中断标志位
----------------------------------------------------------------*/
void DMA0_IRQHandler(void)
{
    /* 清除通道传输中断标志位    (这样才能再次进入中断) */
    DMA_IRQ_CLEAN(DMA_CH0);          

    Field_Over_Flag=1;     //场结束标识                                      
}

/*---------------------------------------------------------------
【函    数】PORTC_Interrupt
【功    能】PORTC端口的中断服务函数 用于摄像头行场中断
【参    数】无
【返 回 值】无
【注意事项】注意进入后要清除中断标志位
----------------------------------------------------------------*/
void PORTC_IRQHandler(void)
{     
    
    if((PORTC_ISFR & 0x0080))//(1<<7)
    {    
        // 用户程序 
        if(0 == Field_Over_Flag)
        {          
            DMATransDataStart(DMA_CH0,(uint32_t)(&Image_Data[0][0]));   //开启DMA传输 
        } 
        PORTC_ISFR |= (0x0080);  //清除中断标识
    } 
}

/*---------------------------------------------------------------
【函    数】CAMERA_Init
【功    能】初始化摄像头 
【参    数】无
【返 回 值】无
【注意事项】注意不要使用 灯PD15
----------------------------------------------------------------*/
void CAMERA_Init(void)
{
    /*  摄像头初始化 */
    LQMT9V034_Init(0);            //摄像头初始化
    
    //摄像头有关接口初始化
    GPIO_ExtiInit(PTC7,rising_down);    //场中断   
    
    /* 优先级配置 抢占优先级0  子优先级2   越小优先级越高  抢占优先级可打断别的中断 */
    NVIC_SetPriority(PORTC_IRQn,NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0,2));
    NVIC_EnableIRQ(PORTC_IRQn);		   //使能PORTC_IRQn的中断 
    
    //八位数据输入口
    GPIO_PinInit(PTD8,GPI,0);                
    GPIO_PinInit(PTD9,GPI,0);
    GPIO_PinInit(PTD10,GPI,0);
    GPIO_PinInit(PTD11,GPI,0);
    GPIO_PinInit(PTD12,GPI,0);
    GPIO_PinInit(PTD13,GPI,0);
    GPIO_PinInit(PTD14,GPI,0);
    GPIO_PinInit(PTD15,GPI,0);     //注意不要使用灯PD15
    
    ///初始化DMA采集       通道0        PTD9-PTD15           目的地址      触发信号 每次一个BYTE  长度            下降沿沿触发
    DMA_PORTx2BUFF_Init (DMA_CH0, (void *)&PTD_BYTE1_IN,(void*)Image_Data, PTE6, DMA_BYTE1, ((IMAGEH)*(IMAGEW)), DMA_falling_down_keepon); 
    /* 优先级配置 抢占优先级0  子优先级2   越小优先级越高  抢占优先级可打断别的中断 */
    NVIC_SetPriority(DMA0_IRQn,NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0,2));
    NVIC_EnableIRQ(DMA0_IRQn);		   //使能DMA中断  
    /* 摄像头初始化结束 */
}


/*---------------------------------------------------------------
【函    数】Test_Camera
【功    能】测试摄像头 
【参    数】无
【返 回 值】无
【注意事项】注意不要使用 灯PD15
----------------------------------------------------------------*/
void Test_Camera(void)
{  
    LED_Init();
    OLED_Init();
    OLED_CLS();
    OLED_Show_Frame94();      //画图像 LCDW*LCDH 外框
    UART_Init(UART4, 115200);      //串口初始化
    /*  摄像头初始化 */
    LQMT9V034_Init(0);            //摄像头初始化
    
    //摄像头有关接口初始化
	GPIO_ExtiInit(PTC6, rising_down);   //行中断
    GPIO_ExtiInit(PTC7,falling_up);    //场中断   
    
    /* 优先级配置 抢占优先级0  子优先级2   越小优先级越高  抢占优先级可打断别的中断 */
    NVIC_SetPriority(PORTC_IRQn,NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0,2));
    NVIC_EnableIRQ(PORTC_IRQn);		   //使能PORTC_IRQn的中断 
    
    //八位数据输入口
    GPIO_PinInit(PTD8,GPI,0);                
    GPIO_PinInit(PTD9,GPI,0);
    GPIO_PinInit(PTD10,GPI,0);
    GPIO_PinInit(PTD11,GPI,0);
    GPIO_PinInit(PTD12,GPI,0);
    GPIO_PinInit(PTD13,GPI,0);
    GPIO_PinInit(PTD14,GPI,0);
    GPIO_PinInit(PTD15,GPI,0);     //注意不要使用灯PD15
    
    ///初始化DMA采集       通道0        PTD9-PTD15           目的地址      触发信号 每次一个BYTE  长度            下降沿沿触发
    DMA_PORTx2BUFF_Init (DMA_CH0, (void *)&PTD_BYTE1_IN,(void*)Image_Data, PTE6, DMA_BYTE1, ((IMAGEH)*(IMAGEW)), DMA_falling_down_keepon); 
    
	/* 优先级配置 抢占优先级0  子优先级2   越小优先级越高  抢占优先级可打断别的中断 */
    //NVIC_SetPriority(DMA0_IRQn,NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0,2));
    //NVIC_EnableIRQ(DMA0_IRQn);		   //使能DMA中断  
    /* 摄像头初始化结束 */
    
    while(1)
    { 
        LED_Reverse(1);           //LED指示程序运行状态
        if(Field_Over_Flag)       //完成一场图像采集
        { 

			Get_Use_Image();      //从采集图像数据中取出自己想要使用的大小
            Get_01_Value(1);      //二值化图像数据
            
            Draw_Road();          //龙邱OLED模块显示动态图像
            Field_Over_Flag= 0;  
        }    
    }
}


/*---------------------------------------------------------------
【函    数】Get_Use_Image
【功    能】获取需要使用的图像大小
【参    数】无
【返 回 值】无
【注意事项】
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
【函    数】Get_01_Value
【功    能】二值化
【参    数】mode  ：  0：使用大津法阈值    1：使用平均阈值
【返 回 值】无
【注意事项】
----------------------------------------------------------------*/
void Get_01_Value(uint8_t mode)
{
  int i = 0,j = 0;
  uint8_t Threshold;
  uint32_t  tv=0;
  char txt[16];
  if(mode)
  {
      //累加
      for(i = 0; i <LCDH; i++)
      {    
          for(j = 0; j <LCDW; j++)
          {                            
              tv+=Image_Use[i][j];   //累加  
          } 
      }
      Threshold=tv/LCDH/LCDW;        //求平均值,光线越暗越小，全黑约35，对着屏幕约160，一般情况下大约100
      Threshold=Threshold*7/10+10;   //此处阈值设置，根据环境的光线来设定 
  }
  else
  {
      Threshold = GetOSTU(Image_Use);//大津法阈值
      Threshold = (uint8_t)(Threshold * 0.5) + 70;
  } 
  if (DebugCamera)
  {
	  sprintf(txt, "%03d", Threshold);//前者为大津法求得的阈值，后者为平均值 
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
      if(Image_Use[i][j] >Threshold) //数值越大，显示的内容越多，较浅的图像也能显示出来    
        Pixle[i][j] =1;        
      else                                        
        Pixle[i][j] =0;
    }    
  }
}


//todo::识别黑线
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

	LINEWITH = 2;       //黑线宽度 
		LEFTLINEFAV = 100;  //左下降沿两点电压变化值最小值
		RINGTLINFAV = 100;  //右下降沿两点电压变化值最小值
		FAV = 100;          //二值化阈值 
	leftcount = 0;     //左黑线宽度计数值
	rightcount = 0;    //右黑线宽度计数值 
	leftedge = 0;      //左边界 
	rightedge = 0;     //右边界
	leftavg = 0;       //计算左黑线的平均值
	rightavg = 0;      //计算右黑线的平均值
	leftflag = 0;      //左丢线标志位
	rightflag = 0;     //右丢线标志位

	for (temp0 = 0; temp0 < 94; temp0++)
		Road[temp0] = 0;
	/*查找左边沿开始，找到边沿后跳出循环*/
	for (temp0 = centre_lastblack; temp0 > LINEWITH; temp0--)               //检测左侧边沿
	{
		for (temp1 = temp0; temp1 > (temp0 - LINEWITH); temp1--)      //寻找一点附近的几个点是否能构成下降沿
		{
			if (point[temp1] == 1) break;                                   //不满足实际条件跳出循环             

									             //比较白点附近的几个值
			if (point[temp1]==0)                          //判断是否满足要求
			{
				leftcount++;                                        //满足条件计数值加一
		   //     testarry[temp0]=temp_value1;
			}

		}

		if (leftcount >= LINEWITH - 1)                        //判断是否满足边沿条件
		{
			leftedge = temp0;                                  //满足条件边沿找到
			leftflag = 0;                                         //找到边沿，标志位置1 
			break;
		}
		else
		{
			leftcount = 0;                                        //未找到清除计数  
			leftflag = 1;                                         //标志位清除
			leftedge = 5;                                         // 边沿赋值 
		}

		if (leftedge < LINEWITH) leftedge = 5;
	}
	Road[leftedge] = 1;
	*LineLeft = leftedge;
	/*左边沿查找结束*/

	/*查找右边沿开始，找到边沿后跳出循环*/
	for (temp0 = centre_lastblack; temp0 < 95-LINEWITH; temp0++)          //右边沿查找方式和查找左边沿一样
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
	/*右边沿查找结束*/


	///*判断已找到边沿的可靠性*/

	///*左边沿的可靠性判断*/
	//if (leftflag == 0)
	//{
	//	/*计算黑线的平均值*/
	//	leftcount = 0;
	//	for (temp0 = leftedge; temp0 > (leftedge - LINEWITH); temp0--)
	//	{
	//		if (temp0 <= 0)break;
	//		leftcount++;
	//		leftavg += point[temp0 + 1];
	//	}

	//	leftavg = leftavg / leftcount;


	//	/*比较白底和黑线之间的差值*/
	//	leftcount = 0;
	//	for (temp0 = leftedge; temp0 < (leftedge + LINEWITH); temp0++)   //判断边沿附近的白点，是否满足条件
	//	{
	//		if (temp0 >= 155)break;

	//		temp_value1 = point[temp0 - 1] - leftavg;                  //白点与黑线平均值作比较
	//		if (temp_value1 >= LEFTLINEFAV)                         //判断是否满足要求
	//		{
	//			leftcount++;                                         //找到边沿，标志位置1 
	//		}
	//		else
	//		{
	//			leftcount = 0;                                         // 标志位清除
	//		}
	//	}

	//	if (leftcount > (LINEWITH - 2))                               //判断是否满足边沿条件
	//	{
	//		*leftblackedge = leftedge;                               //两个条件都满足赋值给边沿
	//		leftedge = 1;
	//	}
	//	else
	//	{
	//		*leftblackedge = 5;
	//		leftcount = 0;                                          //未找到清除计数 
	//		leftflag = 0;                                           //标志位清除
	//	}
	//}
	//else
	//{
	//	*leftblackedge = 5;
	//}
	///*左边界可靠性判断结束*/



	///*右边界可靠性判断开始*/
	//if (rightflag == 0)                      // 右边界判断可靠性，和左边界判断方式一样                  
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

	///*右边界可靠性判断结束*/

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
【函    数】TFTSPI_Show_Cmera
【功    能】在TFT1.8上画出摄像头的图像
【参    数】xs:  起始X   
【参    数】ys:  起始Y  
【参    数】w:   图片宽度 
【参    数】h:   图片高度  
【参    数】ppic[IMAGEH][IMAGEW]:  图片缓存区   
【返 回 值】无
【注意事项】
----------------------------------------------------------------*/	
void TFTSPI_Show_Cmera(uint8_t xs,uint8_t ys,uint8_t w,uint8_t h,uint8_t ppic[LCDH][LCDW]) 
{
#if 0    //灰度显示
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
#else    //二值化显示
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
【函    数】Draw_Road
【功    能】在OLED上画出摄像头的图像
【参    数】无
【返 回 值】无
【注意事项】
----------------------------------------------------------------*/
void Draw_Road(void)
{ 	 
  u8 i = 0, j = 0,temp=0;
  
  //发送帧头标志
  for(i=8;i<56;i+=8)//6*8=48行 
  {
    OLED_Set_Pos(18,i/8+1);//起始位置
    for(j=0;j<LCDW;j++)  //列数
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
【函    数】Draw_Road
【功    能】在OLED上画出识别出的路径的图像
【参    数】无
【返 回 值】无
【注意事项】
----------------------------------------------------------------*/
void Draw_Road_alt(void)
{
	u8 i = 0, j = 0, temp = 0;

	//发送帧头标志
	for (i = 8; i < 56; i += 8)//6*8=48行 
	{
		OLED_Set_Pos(18, i / 8 + 1);//起始位置
		for (j = 0; j < LCDW; j++)  //列数
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
【函    数】Pixle_Filter
【功    能】过滤噪点
【参    数】无
【返 回 值】无
【注意事项】
----------------------------------------------------------------*/
void Pixle_Filter(void)
{  
  int nr; //行
  int nc; //列
  
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
* 函数名称：SendPicture 
* 功能说明：发送图像到上位机 ，不同的上位机注意修改对应的数据接收协议
* 参数说明： 
* 函数返回：void 
* 修改时间：2018年3月27日 
* 备 注： 
***************************************************************/ 
void UARTSendPicture(uint8_t  tmImage[IMAGEH][IMAGEW]) 
{ 
  int i = 0, j = 0; 
  UART_PutChar(UART4,0x01); //发送帧头标志 WindowsFormsApplication1.exe
  UART_PutChar(UART4,0xfe); //发送帧头标志 WindowsFormsApplication1.exe
  for(i=0;i < IMAGEH; i++) 
  { 
    for(j=0;j <IMAGEW ;j++) 
    { 
      if(tmImage[i][j]==0xfe) 
      { 
        tmImage[i][j]=0xff; //防止发送标志位 
      } 
      UART_PutChar(UART4,tmImage[i][j]); 
    } 
  }
  UART_PutChar(UART4,0xfe); //发送帧尾标志 
  UART_PutChar(UART4,0x01); //发送帧尾标志 
} 

/*************************************************************** 
* 
* 函数名称：uint8_t GetOSTU(uint8_t tmImage[IMAGEH][IMAGEW]) 
* 功能说明：求阈值大小 
* 参数说明： 
* 函数返回：阈值大小 
* 修改时间：2018年3月27日 
* 备 注： 
参考：https://blog.csdn.net/zyzhangyue/article/details/45841255
      https://www.cnblogs.com/moon1992/p/5092726.html
      https://www.cnblogs.com/zhonghuasong/p/7250540.html     
Ostu方法又名最大类间差方法，通过统计整个图像的直方图特性来实现全局阈值T的自动选取，其算法步骤为：
1) 先计算图像的直方图，即将图像所有的像素点按照0~255共256个bin，统计落在每个bin的像素点数量
2) 归一化直方图，也即将每个bin中像素点数量除以总的像素点
3) i表示分类的阈值，也即一个灰度级，从0开始迭代
4) 通过归一化的直方图，统计0~i 灰度级的像素(假设像素值在此范围的像素叫做前景像素) 所占整幅图像的比例w0，并统计前景像素的平均灰度u0；统计i~255灰度级的像素(假设像素值在此范围的像素叫做背景像素) 所占整幅图像的比例w1，并统计背景像素的平均灰度u1；
5) 计算前景像素和背景像素的方差 g = w0*w1*(u0-u1) (u0-u1)
6) i++；转到4)，直到i为256时结束迭代
7）将最大g相应的i值作为图像的全局阈值
缺陷:OSTU算法在处理光照不均匀的图像的时候，效果会明显不好，因为利用的是全局像素信息。
解决光照不均匀  https://blog.csdn.net/kk55guang2/article/details/78475414
                https://blog.csdn.net/kk55guang2/article/details/78490069
				https://wenku.baidu.com/view/84e5eb271a37f111f0855b2d.html
***************************************************************/ 
int GetOSTU(uint8_t tmImage[LCDH][LCDW]) 
{ 
/**
  * @brief    未优化过的大津法
  *
  * @param    运算时间比较长
  *
  * @return   实测120*160的图像
  *
  * @note     K66 220MHz需要9ms
  *
  * @example  
  *
  * @date     2019/4/16 星期二
  */
//	int width = LCDW;
//	int height = LCDH;
//	int x = 0, y = 0;
//	int pixelCount[256];
//	float pixelPro[256];
//	int i, j, pixelSum = width * height, threshold = 0;
//	
//	
//	//初始化
//	for (i = 0; i < 256; i++)
//	{
//		pixelCount[i] = 0;
//		pixelPro[i] = 0;
//	}
//	
//	//统计灰度级中每个像素在整幅图像中的个数
//	for (i = y; i < height; i++)
//	{
//		for (j = x; j <width; j++)
//		{
//			pixelCount[tmImage[i][j]]++;
//		}
//	}
//	
//	//计算每个像素在整幅图像中的比例
//	for (i = 0; i < 256; i++)
//	{
//		pixelPro[i] = (float)(pixelCount[i]) / (float)(pixelSum);
//	}
//	
//	//经典ostu算法,得到前景和背景的分割
//	//遍历灰度级[0,255],计算出方差最大的灰度值,为最佳阈值
//	float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;
//	for (i = 0; i < 256; i++)
//	{
//		w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
//		
//		for (j = 0; j < 256; j++)
//		{
//			if (j <= i) //背景部分
//			{
//				//以i为阈值分类，第一类总的概率
//				w0 += pixelPro[j];
//				u0tmp += j * pixelPro[j];
//			}
//			else       //前景部分
//			{
//				//以i为阈值分类，第二类总的概率
//				w1 += pixelPro[j];
//				u1tmp += j * pixelPro[j];
//			}
//		}
//		
//		u0 = u0tmp / w0;        //第一类的平均灰度
//		u1 = u1tmp / w1;        //第二类的平均灰度
//		u = u0tmp + u1tmp;        //整幅图像的平均灰度
//		//计算类间方差
//		deltaTmp = w0 * (u0 - u)*(u0 - u) + w1 * (u1 - u)*(u1 - u);
//		//找出最大类间方差以及对应的阈值
//		if (deltaTmp > deltaMax)
//		{
//			deltaMax = deltaTmp;
//			threshold = i;
//		}
//	}
//	//返回最佳阈值;
//	return threshold;
	
/**
  * @brief    优化过的大津法
  *
  * @param    大大减少运算时间
  *
  * @return   实测K66 220MHz 120*160的图像
  *
  * @note     只需要1.5ms
  *
  * @example  未优化的大津法需要9ms
  *
  * @date     2019/4/16 星期二
  */ 
	int16_t i,j; 
	uint32_t Amount = 0; 
	uint32_t PixelBack = 0; 
	uint32_t PixelIntegralBack = 0; 
	uint32_t PixelIntegral = 0; 
	int32_t PixelIntegralFore = 0; 
	int32_t PixelFore = 0; 
	float OmegaBack, OmegaFore, MicroBack, MicroFore, SigmaB, Sigma; // 类间方差; 
	int16_t MinValue, MaxValue; 
	uint8_t Threshold = 0;
	uint8_t HistoGram[256];              //  
	
	for (j = 0; j < 256; j++)  HistoGram[j] = 0; //初始化灰度直方图 
	
	for (j = 0; j < LCDH; j++) 
	{ 
		for (i = 0; i < LCDW; i++) 
		{ 
			HistoGram[tmImage[j][i]]++; //统计灰度级中每个像素在整幅图像中的个数
		} 
	} 
	
	for (MinValue = 0; MinValue < 256 && HistoGram[MinValue] == 0; MinValue++) ;        //获取最小灰度的值
	for (MaxValue = 255; MaxValue > MinValue && HistoGram[MinValue] == 0; MaxValue--) ; //获取最大灰度的值
	
	if (MaxValue == MinValue)     return MaxValue;         // 图像中只有一个颜色    
	if (MinValue + 1 == MaxValue)  return MinValue;        // 图像中只有二个颜色
	
	for (j = MinValue; j <= MaxValue; j++)    Amount += HistoGram[j];        //  像素总数
	
	PixelIntegral = 0;
	for (j = MinValue; j <= MaxValue; j++)
	{
		PixelIntegral += HistoGram[j] * j;//灰度值总数
	}
	SigmaB = -1;
	for (j = MinValue; j < MaxValue; j++)
	{
		PixelBack = PixelBack + HistoGram[j];   //前景像素点数
		PixelFore = Amount - PixelBack;         //背景像素点数
		OmegaBack = (float)PixelBack / Amount;//前景像素百分比
		OmegaFore = (float)PixelFore / Amount;//背景像素百分比
		PixelIntegralBack += HistoGram[j] * j;  //前景灰度值
		PixelIntegralFore = PixelIntegral - PixelIntegralBack;//背景灰度值
		MicroBack = (float)PixelIntegralBack / PixelBack;   //前景灰度百分比
		MicroFore = (float)PixelIntegralFore / PixelFore;   //背景灰度百分比
		Sigma = OmegaBack * OmegaFore * (MicroBack - MicroFore) * (MicroBack - MicroFore);//计算类间方差
		if (Sigma > SigmaB)                    //遍历最大的类间方差g //找出最大类间方差以及对应的阈值
		{
			SigmaB = Sigma;
			Threshold = j;
		}
	}
	return Threshold;                        //返回最佳阈值;
} 

void Catch(void)
{
	if (Field_Over_Flag)       //完成一场图像采集
	{

		Get_Use_Image();      //从采集图像数据中取出自己想要使用的大小
		Get_01_Value(1);      //二值化图像数据

		Draw_Road();          //龙邱OLED模块显示动态图像
		Field_Over_Flag = 0;
	}
	LED_Reverse(1);
}
