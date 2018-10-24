/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "bsp_SysTick.h"
#include "my_register.h"

extern __IO int32_t OS_TimeMS;

extern struct bitDefine
{
	unsigned bit0: 1;
	unsigned bit1: 1;
	unsigned bit2: 1;
	unsigned bit3: 1;
	unsigned bit4: 1;
	unsigned bit5: 1;
	unsigned bit6: 1;
	unsigned bit7: 1;
} flagA,flagB,flagC,flagD,flagE,flagF;
/** @addtogroup Template_Project
  * @{
  */
float temp;
vu8 Rec_buff[9];
vu16 NTCR;
vu8 Runit;
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	TimingDelay_Decrement();
	OS_TimeMS++;
	t_KeyScan++;
	if(t_KeyScan>10)
	{
		t_KeyScan=0;
		Flag_Key_Scan=1;//д?л???
	}
	t_0_5S++;
	if(t_0_5S>20)
	{
		t_0_5S=0;
		Flag_0_5S =1;//???Ь????
	}
//--------------------------------------------
	if(flag_Tim_USART==1)//???￡??
	{
		t_USART++;
	}
	if(t_USART>40)//??2.6ms
	{
		t_USART=0;
		flag_Tim_USART=0;
		UART_Buffer_Size=0;	
	}
}

void  DMA2_Stream0_IRQHandler(void)
{
	DMA_ClearITPendingBit(DMA2_Stream0,DMA_IT_TCIF0);
	Flag_ADC_Full=1;
}

void  TIM6_DAC_IRQHandler (void)
{
	TIM_ClearITPendingBit(TIM6,TIM_IT_Update);//?????λ
}

void USART1_IRQHandler(void)
{
	flag_Tim_USART=1;
 	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		UART_Buffer_Rece[UART_Buffer_Size]=USART_ReceiveData(USART1);
		UART_Buffer_Size++;
		if (UART_Buffer_Size > 7)
		{
			if (UART_Buffer_Rece[1] == 16)
			{	
				if (UART_Buffer_Size > (UART_Buffer_Rece[6]+8))
				{
					UART_Buffer_Size=0;	  	   		   
					UART_Buffer_Rece_flag=1;
					flag_Tim_USART=0;
					t_USART=0;
				}
			}
			else
			{
				UART_Buffer_Size=0;	  	   	   
				UART_Buffer_Rece_flag=1;  
				flag_Tim_USART=0;
				t_USART=0;
			}	  
		}
	}
}
void USART3_IRQHandler(void)
{
    static vu8 count = 0;
    u8 res; 
    static vu8 i = 0;
    static float sum;
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        
        res =USART_ReceiveData(USART3);//(USART1->DR); //????????
        if(count == 0)
        {
            if(res == 0x03)
            {
                count = 1;
            }else{
                count = 0;
            }
        }else if(count == 1){
             if(res == 0xff)
            {
                count = 2;
            }else{
                count = 0;
            }
            
        }else if(count > 1 && count <4)
        {
            UART_Buffer_Rece[count] = res;               
//            temp = (UART_Buffer_Rece[2] * 256 + UART_Buffer_Rece[3])/10.0;
            count ++;
        }else if(count == 4)
        {
            if(i < 10)
            {
             sum += (UART_Buffer_Rece[2] * 256 + UART_Buffer_Rece[3])/10.0;
            }else{
                temp = sum/10.0;
                sum = 0;
                i = 0;
            }
             count =0;
             i ++;
        }
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
     }
}

uint16_t CRC16(uint8_t *puchMsg, uint8_t Len)
{
	uint8_t t, m,n,p;
	uint8_t uchCRCHi=0xFF; /* 高CRC字节初始化*/ 
	uint8_t uchCRCLo =0xFF; /* 低CRC 字节初始化*/ 
	for(t=0;t<Len;t++)
	{	
		uchCRCLo=uchCRCLo^puchMsg[t];
		for(n=0;n<8;n++)
		{
			m=uchCRCLo&1;p=uchCRCHi&1;uchCRCHi>>=1;
			uchCRCLo>>=1;

			if(p)
			{
				uchCRCLo|=0x80;
			}
			if(m)	
			{
				uchCRCHi=uchCRCHi^0xa0;
				uchCRCLo=uchCRCLo^1;
			}
		}
	}
	return (uchCRCHi<<8|uchCRCLo);
}

void UART5_IRQHandler(void)
{
    static vu8 count;
    u8 res,i;
    static u16 crc;
    static vu8 check[7];
    static vu16 cal;
//    u16 crch;
    if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)
    {
        res = USART_ReceiveData(UART5);
        Rec_buff[count] = res;
        count ++;
        if(count > 8)
        {
            for(i=0;i<7;i++)
            {
                check[i] = Rec_buff[i];
            }
            crc = Rec_buff[8]<<8;
            crc = crc + Rec_buff[7];
            cal = CRC16(check,7);
            if(crc == cal)
            {
                if(Rec_buff[5] == 0x0C && Rec_buff[6] == 0x45 && crc == 0x9D82)
                {
                    NTCR = 0;
                }else{
                    NTCR = Rec_buff[5]<<8;
                    NTCR = NTCR + Rec_buff[6];
                    Runit = Rec_buff[4]&0x0F;
                }                               
            }
            count = 0;
        }
        USART_ClearITPendingBit(UART5, USART_IT_RXNE);
    }
}


/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
