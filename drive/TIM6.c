/******************************************************************/
/* ???TIM3 PWM                                                */
/* Ч?ú                                                        */
/* ??ú????200HZ ????ú60.9% ????ú30.9%?PWM      */
/* ??ún?                                                    */
/* j???úQQ:363116119                                        */
/******************************************************************/
#include "my_register.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx.h"
#include "tim6.h"
#include "MainTask.h"
#include "ssd1963.h"
/*****************************************************************/
/*****************************************************************/

vu16 battery_c;
float bc_raw;
extern vu8 pow_sw;
extern vu8 cdc_sw;
extern vu8 load_sw;
extern vu8 oct_sw; 
extern vu8 oc_test;
extern vu8 c_rec;
vu8 resetflag;
vu8 resdone;
float watch;
//????? 3 ?????
//arrú?????c pscú??????
//???????????:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=??????ê,?λ:Mhz
//?o???ˇ??? 3!
void TIM4_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE); //ù?? TIM3 ??
    TIM_TimeBaseInitStructure.TIM_Period = arr; //??????
    TIM_TimeBaseInitStructure.TIM_Prescaler=psc; //?????
    TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //в?????
    TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);// ú?????? TIM3
    TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE); //?????? 3 ?т??
    NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn; //??? 3 ??
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //????? 1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //Ь???? 3
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStructure);// ü??? NVIC
    TIM_Cmd(TIM4,ENABLE); //Y????? 3
}
//??? 3 ??????
void TIM4_IRQHandler(void)
{
    static vu16 resetcount;
    static vu8 read1963;
    static vu16 scancount;
//     static float crec1,crec2;
    
    if(TIM_GetITStatus(TIM4,TIM_IT_Update)==SET) //????
    {
        TIM_ClearITPendingBit(TIM4,TIM_IT_Update); //清除中断标志位
        
        if(page_sw != face_starter)
        {
             if(resetflag == 1)
             {
                 if(resetcount == 3000)
                 {
//                     sLCD_GPIO_Config();
                     sLCD_Init();
//                     sLCD_WR_REG(0xf1);
                     GUI_Init();
                     if(page_sw == face_menu)
                     {
                         ResetPow();
                     }else if(page_sw == face_cdc){
                         ResetCDC();
                     }else if(page_sw == face_r){
                         ResetR();
                     }else if(page_sw == face_load){
                         ResetLoad();
                     }else if(page_sw == face_graph){
                         ResetG();
                     }else if(page_sw == face_set){
                         ResetSET();
                     }
//                     resdone = 1;
//                     resetflag = 0;
                     resetcount = 0;
                 }else{
                     resetcount++;
                 }                
             }
         }
         
        if(oct_sw == oct_on)
        {
            OC_CHECK();
            if(oct_sw == oct_on)
            {
                stepcount ++;
                if(stepcount == steptime*10)
                {
                    OC_ADD();
                    stepcount = 0;
                }
            }
        }
//         if(page_sw == face_load)
//         {
//             if(c_rec == 1)
//             {
//                 crec2 = crec1;
//                 crec1 = DISS_Current;
//                 if(crec2 > crec1)
//                 {
//                     watch = crec2;
//                     c_rec = 0;
//                     crec2 = crec1 = 0;
//                 }
//             }
//         }
    }    
}

void TIM6_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	/* TIM3 ??? ---------------------------------------------------
   TIM3 ????(TIM3CLK) ??? APB2 ?? (PCLK2)    
    => TIM3CLK = PCLK2 = SystemCoreClock
   TIM3CLK = SystemCoreClock, Prescaler = 0, TIM3 counter clock = SystemCoreClock
   SystemCoreClock ?48 MHz */
  /* TIM16 ???? */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
	
  /* Time ??????*/
  TIM_TimeBaseStructure.TIM_Prescaler = 4800-1;//?????
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  /* Time ????????????*/
  TIM_TimeBaseStructure.TIM_Period = 5000;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
//  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM6,&TIM_TimeBaseStructure);
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);//??????т??
	TIM_SetAutoreload(TIM6, 0xFF);//??PWM??ê
	TIM6_NVIC_Config();
  /* TIM3 ?????*/
  TIM_Cmd(TIM6, ENABLE);
}
/***********************************************************************/
static void TIM6_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	
	/* Enable the USART1 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

//????? 3 ?????
//arrú?????c pscú??????
//???????????:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=??????ê,?λ:Mhz
//?o???ˇ??? 3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE); //ù?? TIM3 ??
    TIM_TimeBaseInitStructure.TIM_Period = arr; //??????
    TIM_TimeBaseInitStructure.TIM_Prescaler=psc; //?????
    TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //в?????
    TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);// ú?????? TIM3
    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //?????? 3 ?т??
    NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //??? 3 ??
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //????? 1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //Ь???? 3
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStructure);// ü??? NVIC
    TIM_Cmd(TIM3,ENABLE); //Y????? 3
}
//??? 3 ??????
void TIM3_IRQHandler(void)
{
    static vu8 calert = 0;
    static vu16 resetcount;
    static vu8 read1963;
    static vu16 scancount;
    
    if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //????
    {
        TIM_ClearITPendingBit(TIM3,TIM_IT_Update); //??????λ
//         if(page_sw != face_starter)
//         {
//              if(resetflag == 1)
//              {
//                  if(resetcount == 1)
//                  {
//                      LCD_Initializtion();
//                      sLCD_WR_REG(0xf1);
//                      GUI_Init();
//                      if(page_sw == face_menu)
//                      {
//                          ResetPow();
//                      }else if(page_sw == face_cdc){
//                          ResetCDC();
//                      }else if(page_sw == face_r){
//                          ResetR();
//                      }else if(page_sw == face_load){
//                          ResetLoad();
//                      }else if(page_sw == face_graph){
//                          ResetG();
//                      }else if(page_sw == face_set){
//                          ResetSET();
//                      }
//                      resdone = 1;
// //                     resetflag = 0;
//                      resetcount = 0;
//                  }else{
//                      resetcount++;
//                  }                
//              }
//          }
        switch(page_sw)
        {
            case face_menu:
            {
                if(pow_sw == pow_on)
                {
                    bc_raw += DISS_POW_Current * 1000 * 1/3600;
                }else{
                    bc_raw = 0;
                }
            }break;
            case face_cdc:
            {
                if(mode_sw == mode_pow && cdc_sw == cdc_on)
                {
                    bc_raw += DISS_POW_Current * 1000 * 1/3600;
                }else if(cdc_sw == cdc_off){
                    bc_raw = 0;
                }
            }break;
            case face_load:
            {
                if(load_sw == load_on)
                {
                    if(alert_flag == 1)
                    {
                        calert ++;
                        if(calert == 3)
                        {
                            t_onoff = 0;
                            GPIO_SetBits(GPIOA,GPIO_Pin_15);//????OFF
                            mode_sw = 0;
                            load_sw = load_off;
                            calert = 0;                                
                        }
                    }
                    bc_raw += DISS_Current * 1000 * 1/3600;
                }else{
                    bc_raw = 0;
                }
            }break;
            case face_graph:
            {
                if(mode_sw == mode_pow)
                {
                    if(pow_sw == pow_on)
                    {
                        bc_raw += DISS_POW_Current * 1000 * 1/3600;
                    }else if(mode_sw == mode_pow && cdc_sw == cdc_on)
                    {
                        bc_raw += DISS_POW_Current * 1000 * 1/3600;
                    }
                    else{
                        bc_raw = 0;
                    }
                }               
                
                if(mode_sw == mode_load)
                {
                    if(load_sw == load_on)
                    {
                        bc_raw += DISS_Current * 1000 * 1/3600;
                    }else{
                        bc_raw = 0;
                    }
                }
            }break;
            case face_r:
            {
                if(oct_sw == oct_on)
                {
                    if(alert_flag == 1)
                    {
                        calert ++;
                        if(calert == 3)
                        {
                            ocstop = 1;
                            calert = 0;
                        }
                    }
                }
            }break;
        }
//         GPIO_ResetBits(GPIOD,GPIO_Pin_12);
//         TM1650_SET_LED(0x48,0x71);
//         TM1650_SET_LED(0x68,0xF2);//PASS?
    }    
    
}

