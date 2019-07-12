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
//------------------------------------------------------------------------------------------------    
//����VL53ģ��   ����Ļ����ʾ����   ����SCL->PTE7  SDA->PTE8  VIN-> 3.3V  GND->GND
//PTE9 �� ����һ�� VL53�� XSƬѡ   ��һ��VL53�� XS���ռ���
//------------------------------------------------------------------------------------------------   
-------------------------------------------------------------*/

#include "include.h"
/**
  * @brief    ����VL53
  *
  * @param    ��
  *
  * @return   ��
  *
  * @note     ��
  *
  * @example  
  *
  * @date     2019/4/17 ������
  */
void Test_Vl53(void)
{
    LED_Init();
    UART_Init(UART4, 115200);
	systime.init();
	printf("\r\nLQ VL53 Test");
	
#ifdef LQ_OLED
	OLED_Init();
    OLED_CLS();
	OLED_P8x16Str(5,0,"LQ VL53 Test");
#else
	TFTSPI_Init(1);                //TFT1.8��ʼ��  0��������ʾ  1��������ʾ  
    TFTSPI_CLS(u16BLUE);           //����
	TFTSPI_P8X16Str(0,0,"LQ VL53 Test",u16RED,u16BLUE);
#endif
	char txt[16];
	
	IIC_Init();       
    
    /* PTE9 �� ����һ�� VL53�� XSƬѡ*/
    GPIO_PinInit(PTE9, GPO, 0);
    GPIO_PinWrite(PTE9, 0U);//ʧ��PTE9���ӵ�VL53 
    delayms(100);
    /* �޸�IIC��ַ�󣬽�VL53��ַ��Ϊ 0x60 */
    uint8_t iic_new_addr = (VL53NEWADDR/2);
    VL53_Write_nByte(VL53ADDR, VL53L0X_REG_I2C_SLAVE_DEVICE_ADDRESS, 1, &iic_new_addr);
    
    GPIO_PinWrite(PTE9, 1U);//ʹ��PTE9���ӵ�VL53 
    delayms(100);
    
    
    uint8_t VL53_STAR = 0x01;
    uint8_t dis_buff[2];
    uint16_t dis1, dis2;
    while(1)
    {
        /* ��ȡ�������� */
		VL53_Read_nByte(VL53NEWADDR, VL53_REG_DIS, 2, dis_buff);
        
        /* ��ʼһ�β�� */
        VL53_Write_nByte(VL53NEWADDR, VL53L0X_REG_SYSRANGE_START, 1, &VL53_STAR);
        
        /* ת������ */
        dis1 = (dis_buff[0]<<8) | (dis_buff[1]);
        if(dis1 > 8000)
        {
            dis1 = 0;
        }
        
        /* ��ȡ�������� */
		VL53_Read_nByte(VL53ADDR, VL53_REG_DIS, 2, dis_buff);
        
        /* ��ʼһ�β�� */
        VL53_Write_nByte(VL53ADDR, VL53L0X_REG_SYSRANGE_START, 1, &VL53_STAR);
        
        /* ת������ */
        dis2 = (dis_buff[0]<<8) | (dis_buff[1]);
        if(dis1 > 8000)
        {
            dis2 = 0;
        }
        
		sprintf(txt, "DIS %5d mm",dis1);
#ifdef LQ_OLED
		OLED_P8x16Str(0,3,txt);
#else
		TFTSPI_P8X16Str(2,5,txt,u16RED,u16BLUE);
#endif
        sprintf(txt, "DIS %5d mm",dis2);
#ifdef LQ_OLED
		OLED_P8x16Str(0,5,txt);
#else
		TFTSPI_P8X16Str(2,7,txt,u16RED,u16BLUE);
#endif
        
		delayms(50);
		LED_Reverse(1);
    
    }
    
}


/**
  * @brief    VL53 дn���Ĵ���
  *
  * @param    dev��    ��ַ
  * @param    reg��    �Ĵ���
  * @param    length;  ����
  * @param    data��   ָ��д������
  *
  * @return   
  *
  * @note     
  *
  * @example  
  *
  * @date     2019/4/29 ����һ
  */
void VL53_Write_nByte(uint8_t dev, uint8_t reg, uint8_t length, uint8_t* data)
{
    IIC_WriteMultByteToSlave( dev,  reg,  length,  data);
}

/**
  * @brief    VL53 ��n���Ĵ���
  *
  * @param    dev��    ��ַ
  * @param    reg��    �Ĵ���
  * @param    length;  ����
  * @param    data��   ָ���Ŷ�����
  *
  * @return   
  *
  * @note     
  *
  * @example  
  *
  * @date     2019/4/29 ����һ
  */
void VL53_Read_nByte(uint8_t dev, uint8_t reg, uint8_t length, uint8_t* data)
{
    IIC_ReadMultByteFromSlave(dev, reg, length, data);
}