#ifndef INCLUDE_H_
#define INCLUDE_H_

/*���Կ���*/
#define DebugImage 0
#define DebugEnc 0
#define DebugCamera 0
#define DebugSD5 0
#define DebugRoad 1



/*
 * ����Cortex-M�ں˵�ͨ��ͷ�ļ�
 */
#include    <stdio.h>                       //printf
#include    <string.h>                      //memcpy
#include    <stdlib.h>                      //malloc
#include "math.h"
#include "common.h" 
#include "MK60N512VMD100.h"   /* �Ĵ���ӳ��ͷ�ļ� */
#include "core_cm4.h"         /* �ں��ļ����������ж����ȼ� */

/*----------------------------------------------------------------
                   �ײ������ļ�
------------------------------------------------------------------*/
#include "MK60_ADC.h"              /* ����ADCģ�� */
#include "MK60_GPIO.h"             /* ����GPIOģ�� */
#include "MK60_GPIO_Cfg.h"         /* ����GPIOģ�� ��51��IO���� */
#include "MK60_PLL.h"              /* ����ʱ��Ƶ������ */
#include "MK60_UART.h"             /* ���ڴ���ģ�� */
#include "MK60_PIT.h"              /* ����PIT��ʱ��ģ�� */
#include "MK60_FTM.h"              /* ����FTM��ʱ��ģ�� */
#include "MK60_CMT.h"              /* ����CMT��ʱ��ĳ�� */
#include "MK60_IIC.h"              /* ����IICģ�� */
#include "MK60_DMA.h"              /* ����DMAģ�� */
#include "MK60_LPTMR.h"            /* ����LPTMR��ʱ��ģ�� */
#include "MK60_WDOG.h"             /* ���ڿ��Ź� */
#include "MK60_SYSTICK.h"          /* systick �ں˶�ʱ�� */
#include "MK60_FLASH.h"            /* Flash ��д */
#include "MK60_SPI.h"              /* SPI ���� */
/*----------------------------------------------------------------
                    ����ģ������
------------------------------------------------------------------*/
#include "LQ_LED.h"                /* LED         ���� */
#include "LQ_KEY.h"                /* KEY         ���� */
#include "LQ_PIT.h"                /* PIT         ���� */
#include "LQ_UART.h"               /* UART        ���� */
#include "LQ_LPTMR.h"              /* LPTMR       ���� */
#include "LQ_SYSTICK.h"            /* SYSTICK     ���� */
#include "LQ_12864.h"              /* OLED        ���� */
#include "LQ_SGP18T.h"             /* TFT1.8      ���� */
#include "LQ_ADC.h"                /* ADC         ���� */
#include "LQ_WDOG.h"               /* WDOG        ���� */
#include "LQ_SD5.h"                /* ���        ���� */
#include "LQ_MOTOR.h"              /* ���        ���� */
#include "LQ_MPU6050.h"            /* 6050        ���� */
#include "LQ_9AX.h"                /* ����        ���� */
#include "LQ_MT9V034.h"            /* ����        ���� */
#include "LQ_CAMERA.h"             /* ����ͷ      ���� */
#include "LQ_FLASH.h"              /* FLASH       ���� */
#include "LQ_CCD.h"                /* CCD         ���� */
#include "LQ_Ultrasonic.h"         /* ���������  ���� */
#include "ANO_DT.h"
#include "LQ_VL53.h"               /* VL53        ���� */
#include "LQ_ICM20602.h"


#include "status.h"
#include "fsl_edma.h"
#include "Uart_DMA.h"

/* �����ж����ȼ���  0: 0����ռ���ȼ�16λ�������ȼ�
 * 1: 2����ռ���ȼ� 8�������ȼ� 2: 4����ռ���ȼ� 4�������ȼ�
 * 3: 8����ռ���ȼ� 2�������ȼ� 4: 16����ռ���ȼ� 0�������ȼ�
 */

/* ����ICM20602 Ӳ��SPI�ӿ�,��ȡ�ٶȱȽϿ죬���20us
�°�K60ĸ��VE2�汾��Ԥ������һ��Ӳ��SPI�ӿڵ����ţ���CCD���ӿڴ������öŰ�������
GND   ------   GND
3-5V  ------   3.3V
SCL   ------   PTE2
SDA   ------   PTE1
SA    ------   PTE3
CS    ------   PTE0*/

/* ��OLED ����ʾ������TFT1.8��ʾ */
#define LQ_OLED     
//#define LQ_TFT1_8    

#endif