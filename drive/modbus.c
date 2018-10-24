/******************** (C) COPYRIGHT 2014 KUNKIN********************
 * 文件名  :MODBUS.C
 * 作者   :
 * 描述    :MODBUS?????
 * 内容    :KL220A_Master
 * 硬件连接: 485
 * 修改日期:2014-12-22
********************************************************************/
#include "my_register.h" 
#include "usart.h" 
#include "modbus.h" 
#include "flash.h"
#include "stm32f4xx_gpio.h"
//===================================================================//
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
} flagA,flagB,flagC,flagD,flagE,flagG;
/*************************校准参数************************************/
vu16 ADJ_Write;//校准时需写入的实际值
vu8 DAC_Flag;//DAC是否加载标志
vu32 Modify_A_READ;
vu32 Modify_C_READ;
vu32 Modify_A_ACT;
	
vu32 Modify_B_READ;
vu32 Modify_D_READ;
vu32 Modify_B_ACT;
vu32 Correct_Parametet[13];//校准参数
vu32 Correct_Strong[13];//校准系数
vu8  correct_por[6];
/*************************变量定义***********************************/
vu32 Run_Control[42];
vu8 ADDR;
//============================函数声明===============================//
vu16 Hardware_CRC(vu8 *p_buffer,vu8 count);
//===========================MODBUS协议=============================//
void UART_Action(void)
{//RUT??ú
	//ADDR  ???  ??????????   ??????????  ?????????   ????????  CRC? CRC?
	//????úADDR ??? ???????  ???  ??? ..... CRC?  CRC?
	if (UART_Buffer_Rece[0] == ADDR)
	{
		if (UART_Buffer_Rece[1] == (0x03))	//??3 ???   
		{																		 
			vu8 i;
			vu16 crc_result;
			crc_result = (UART_Buffer_Rece[6] << 8) + UART_Buffer_Rece[7];
			if ((crc_result == Hardware_CRC(UART_Buffer_Rece,6)) ||(crc_result == 0) )
			{
				if (UART_Buffer_Rece[3] < 0x07)    								//?????????Χ?
				{
					if ((UART_Buffer_Rece[3] + UART_Buffer_Rece[5]) < 0x0F)		//??????????????????Χ?
					{							
						UART_Buffer_Send[0] = ADDR;
						UART_Buffer_Send[1] = 0x03;
						UART_Buffer_Send[2] = UART_Buffer_Rece[5]*2;
						for (i=0;i<UART_Buffer_Send[2];i++)
						{
							if ((i % 2) == 0) UART_Buffer_Send[3 + i] = Run_Control[UART_Buffer_Rece[3] + i / 2] >> 8;
							else UART_Buffer_Send[3 + i] = Run_Control[UART_Buffer_Rece[3] + i / 2];														
						}
						crc_result = Hardware_CRC(UART_Buffer_Send,UART_Buffer_Send[2] + 3);
						UART_Buffer_Send[3 + UART_Buffer_Send[2]] = crc_result >> 8;
						UART_Buffer_Send[4 + UART_Buffer_Send[2]] = crc_result;
						Transmit_BUFFERsize = UART_Buffer_Send[2] + 5;
						UART_SEND_flag=1;
					}
				}
			}	
		}
	} 
//===============================д???=================================
	if ((UART_Buffer_Rece[0] == 0) || (UART_Buffer_Rece[0] == ADDR) || (UART_Buffer_Rece[0] == ((ADDR-1)/4+100)))	 
	{
		vu8 var8;
		vu8 a=0;
		vu16 var16;
		vu16 crc_result;
//=========================????6 д?????===========================
		if (UART_Buffer_Rece[1] == 6)                                 //???????ˇ????6
		{
			if (UART_Buffer_Rece[3] < 0x05)							  //????д???ˇ???д?Χ?
			{
				crc_result = (UART_Buffer_Rece[6] << 8) + UART_Buffer_Rece[7];
				if ((crc_result == Hardware_CRC(UART_Buffer_Rece,6)) ||(crc_result == 0) )		  //??CRC
				{
					var16 = (UART_Buffer_Rece[4] << 8) + UART_Buffer_Rece[5];	//?5 6?????д????
					var8 = UART_Buffer_Rece[3];	        						//?3 4?????д????
					Run_Control[var8] = var16;			    //???д??????

					if (UART_Buffer_Rece[0] == ADDR)							//??????????
					{
						for (a=0;a<8;a++)
						{UART_Buffer_Send[a] = UART_Buffer_Rece[a];}
						Transmit_BUFFERsize = 8;						//??????ì???CRC
						UART_SEND_flag=1;
					}
				}
			}
		}
//=======================================??ˇ??16ìlд???===========================================
//???16??:
//     ?? ?? д??????  д?????? д????  д???? д????  ??? ??? ......CRC? CRC?
//??????:
//     ?? ?? д??????  д??????  д????? д?????  CRC?  CRC? 
		if (UART_Buffer_Rece[1] == 16)										  
		{	
			if ((UART_Buffer_Rece[6] == 6) && (UART_Buffer_Rece[3] == 0x00))	//??lд??????
			{
				crc_result = (UART_Buffer_Rece[13] << 8) + UART_Buffer_Rece[14];
				if ((crc_result == Hardware_CRC(UART_Buffer_Rece,13)) ||(crc_result == 0) )	   //??CRC
				{												
					for (var8=0;var8<3;var8++) Run_Control[var8] = (UART_Buffer_Rece[var8*2+7] << 8) + UART_Buffer_Rece[var8*2+8];

					if (UART_Buffer_Rece[0] == ADDR)					  //?????????
					{
						UART_Buffer_Send[0] = ADDR;
						UART_Buffer_Send[1] = 16;
						UART_Buffer_Send[2] = UART_Buffer_Rece[2];
						UART_Buffer_Send[3] = UART_Buffer_Rece[3];
						UART_Buffer_Send[4] = UART_Buffer_Rece[4];
						UART_Buffer_Send[5] = UART_Buffer_Rece[5];
						crc_result = Hardware_CRC(UART_Buffer_Send,6);	 //??CRC?
						UART_Buffer_Send[6] = crc_result>>8;
						UART_Buffer_Send[7] = crc_result;				 
						Transmit_BUFFERsize = 8;					     //?????????
						UART_SEND_flag=1;
					}
				}
			}			 
		}
	}
/*************************************???У???**************************************************************************/
	if (((UART_Buffer_Rece[0] == 0x01)&&(UART_Buffer_Rece[2] == 0xA5))||(flag_ADJ_ON==1))			   //??У?
	{ 
		if(UART_Buffer_Rece[1] == 0x01)
		{
			flag_ADJ_VL=0;
			Modify_A_READ = Vmon1_value;//????
			Modify_A_ACT = (UART_Buffer_Rece[3] << 8) + UART_Buffer_Rece[4];//????
		}
		if (UART_Buffer_Rece[1] == 0x02)			   //???У???
		{
			vu32 var16;
			vu32 var32a;
			vu32 var32b;
			
			vu32 var16a;
			vu32 var32c;
			vu32 var32d;
			Modify_B_READ =Vmon1_value;//????
			Modify_B_ACT = (UART_Buffer_Rece[3] << 8) + UART_Buffer_Rece[4];//????
			var32a = Modify_B_ACT;
			var32a = var32a - Modify_A_ACT;
			var32a = var32a << 12;
			var16 = Modify_B_READ - Modify_A_READ;
			var32a = var32a / var16;
			REG_CorrectionV = var32a;
			var32a=0;
			var32a = Modify_B_ACT;
			var32a = var32a << 12;
			var32b = Modify_B_READ;
			var32b = var32b * REG_CorrectionV;
			if (var32a < var32b)
			{
				var32b = var32b - var32a;
				REG_ReadV_Offset = var32b;
				Polar |= 0x01;
			}
			else 
			{
				var32a = var32a - var32b;
				REG_ReadV_Offset = var32a;
				Polar &= ~0x01;
			}			
			Flash_Write_all();	//??д?FLASH
			Flag_DAC_OFF=0;
		}
		
		
/************************************???·????У?*****************************************************************/
		if (UART_Buffer_Rece[1] == 0x03)			   //CC??У?
		{
			Modify_A_READ = Imon1_value;//
			Modify_C_READ = Contr_Laod;//
			Modify_A_ACT = (UART_Buffer_Rece[3] << 8) + UART_Buffer_Rece[4];
			Flag_DAC_OFF=1;//
		}

		if (UART_Buffer_Rece[1] == 0x04)			   //
		{
			vu32 var16;
			vu32 var32a;
			vu32 var32b;
			
			vu32 var16a;
			vu32 var32c;
			vu32 var32d;
			
			Modify_B_READ = Imon1_value;
			Modify_D_READ = Contr_Laod;
			Modify_B_ACT = (UART_Buffer_Rece[3] << 8) + UART_Buffer_Rece[4];
			
			var32a = Modify_B_ACT;
			var32a = var32a - Modify_A_ACT;
			var32a = var32a << 12;
			var16 = Modify_B_READ - Modify_A_READ;
			var32a = var32a / var16;
			REG_Load_A = var32a;
			var32a = Modify_B_ACT;
			var32a = var32a << 12;
			var32b = Modify_B_READ;
			var32b = var32b * REG_Load_A;
			if (var32a < var32b)
			{
				var32b = var32b - var32a;
				REG_LoadA_Offset = var32b;
				Polar1 |= 0x01;
			}
			else 
			{
				var32a = var32a - var32b;
				REG_LoadA_Offset = var32a;
				Polar1 &= ~0x01;					
			}
//---------------------------------------------------------------------------------//
			var32c = Modify_B_ACT; 
			var32c = var32c - Modify_A_ACT;
			var32c = var32c << 12;
			var16a=Modify_D_READ-Modify_C_READ;
			var16a=var16a*2;
			var32c=var32c/var16a;
			SET_LoadA = var32c;
			var32c = Modify_B_ACT;
			var32c = var32c << 12;
			var32d = SET_LoadA;
			var32d = var32d * (Modify_D_READ*2);
			if (var32c < var32d)
			{
				var32d = var32d - var32c;
				SET_LoadA_Offset = var32d;
				Polar1 |= 0x04;
			}
			else 
			{
				var32c = var32c - var32d;
				SET_LoadA_Offset = var32c;
				Polar1 &= ~0x04;
			}
			Flash_Write_all ();	
			Flag_DAC_OFF =0;
		}
/*************************************??CV??????У?**************************************************************/
		if(UART_Buffer_Rece[1] == 0x05)
		{
			Modify_A_READ = Vmon1_value;//????
			Modify_C_READ = Contr_Laod;//?????
			Modify_A_ACT = (UART_Buffer_Rece[3] << 8) + UART_Buffer_Rece[4];//????
		}
		if (UART_Buffer_Rece[1] == 0x06)			   //???У???
		{
			vu32 var16;
			vu32 var32a;
			vu32 var32b;
			
			vu32 var16a;
			vu32 var32c;
			vu32 var32d;
			
			Modify_B_READ =Vmon1_value;//????
			Modify_D_READ =Contr_Laod;//?????
			Modify_B_ACT = (UART_Buffer_Rece[3] << 8) + UART_Buffer_Rece[4];//????
			var32a = Modify_B_ACT;
			var32a = var32a - Modify_A_ACT;
			var32a = var32a << 12;
			var16 = Modify_B_READ - Modify_A_READ;
			var32a = var32a / var16;
			REG_LoadV = var32a;
			var32a=0;
			var32a = Modify_B_ACT;
			var32a = var32a << 12;
			var32b = Modify_B_READ;
			var32b = var32b * REG_LoadV;
			if (var32a < var32b)
			{
				var32b = var32b - var32a;
				REG_LoadV_Offset = var32b;
				Polar2 |= 0x01;
			}
			else 
			{
				var32a = var32a - var32b;
				REG_LoadV_Offset = var32a;
				Polar2 &= ~0x01;
			}
//---------------------------------------------------------------------------------------//			
			var32c = Modify_B_ACT; //CV??????У?
			var32c = var32c - Modify_A_ACT;
			var32c = var32c << 12;
			var16a=Modify_D_READ-Modify_C_READ;
			var16a=(var16a*2);
			var32c=var32c/var16a;
			SET_LoadV = var32c;
			var32c = Modify_B_ACT;
			var32c = var32c << 12;
			var32d = SET_LoadV;
			var32d = var32d * (Modify_D_READ*2);
			if (var32c < var32d)
			{
				var32d = var32d - var32c;
				SET_LoadV_Offset = var32d;
				Polar2 |= 0x04;
			}
			else 
			{
				var32c = var32c - var32d;
				SET_LoadV_Offset = var32c;
				Polar2 &= ~0x04;
			}
//---------------------------------------------------------------------------------------//
		  Flash_Write_all();	//??д?FLASH
			DAC_Flag=0;
		}
/*************************************??У?**************************************************************************/
		if(UART_Buffer_Rece[1] == 0x07||flag_ADJ_VL==1)
		{
			Modify_A_READ = Rmon_value;//????
			Modify_A_ACT = (UART_Buffer_Rece[3] << 8) + UART_Buffer_Rece[4];//????
		}
		if (UART_Buffer_Rece[1] == 0x08||flag_ADJ_VH==1)			   //???У???
		{
			vu16 var16;
			vu32 var32a;
			vu32 var32b;
			
			vu16 var16a;
			vu32 var32c;
			vu32 var32d;
			Modify_B_READ =Rmon_value;//????
			flag_OverV=1;
			Modify_B_ACT = (UART_Buffer_Rece[3] << 8) + UART_Buffer_Rece[4];//????
			if(flag_OverV==1)//??????д?????У???д?FLASH
			{
				var32a = Modify_B_ACT;
				var32a = var32a - Modify_A_ACT;
				var32a = var32a << 12;
				var16 = Modify_B_READ - Modify_A_READ;
				var32a = var32a / var16;
                if(r_raly == 1)
                {
                    REG_CorrectionR = var32a;
                    var32a=0;
                    var32a = Modify_B_ACT;
                    var32a = var32a << 12;
                    var32b = Modify_B_READ;
                    var32b = var32b * REG_CorrectionR;
                    if (var32a < var32b)
                    {
                        var32b = var32b - var32a;
                        REG_ReadR_Offset = var32b;
                        Polar3 |= 0x01;
                    }
                    else 
                    {
                        var32a = var32a - var32b;
                        REG_ReadR_Offset = var32a;
                        Polar3 &= ~0x01;
                    }
                }else{
                    REG_CorrectionRL = var32a;
                    var32a=0;
                    var32a = Modify_B_ACT;
                    var32a = var32a << 12;
                    var32b = Modify_B_READ;
                    var32b = var32b * REG_CorrectionRL;
                    if (var32a < var32b)
                    {
                        var32b = var32b - var32a;
                        REG_ReadRL_Offset = var32b;
                        Polar3 |= 0x01;
                    }
                    else 
                    {
                        var32a = var32a - var32b;
                        REG_ReadR_Offset = var32a;
                        Polar3 &= ~0x01;
                    }
                }
	//---------------------------------------------------------------------------------------//
				Flash_Write_all();	//??д?FLASH
				flag_OverV=0;
				Flag_DAC_OFF=0;
			}
			flag_ADJ_VH=0;//????λ??????
		}		
/*******************************??CC???·????У?******************************************/	
		if (UART_Buffer_Rece[1] == 0x09||flag_ADJ_ALCC==1)			   //?·?У?
		{
			Modify_A_READ = Imon_value;//??·
			Modify_C_READ = Contr_Current;//???·
			Modify_A_ACT = (UART_Buffer_Rece[3] << 8) + UART_Buffer_Rece[4];
		}

		if (UART_Buffer_Rece[1] == 0x0A||flag_ADJ_AHCC==1)			   //?·?У???
		{
			vu16 var16;
			vu32 var32a;
			vu32 var32b;
			
			vu16 var16a;
			vu32 var32c;
			vu32 var32d;
			
			Modify_D_READ = Contr_Current;
			Modify_B_READ = Imon_value;
			Modify_B_ACT = (UART_Buffer_Rece[3] << 8) + UART_Buffer_Rece[4];
			var32a = Modify_B_ACT;
			var32a = var32a - Modify_A_ACT;
			var32a = var32a << 14;
			var16 = Modify_B_READ - Modify_A_READ;
			var32a = var32a / var16;
			REG_POWERA = var32a;
			var32a = Modify_B_ACT;
			var32a = var32a << 14;
			var32b = Modify_B_READ;
			var32b = var32b * REG_POWERA;
			if (var32a < var32b)
			{
				var32b = var32b - var32a;
				REG_POWERA_Offset = var32b;
				Polar4 |= 0x01;
			}
			else 
			{
				var32a = var32a - var32b;
				REG_POWERA_Offset = var32a;
				Polar4 &= ~0x01;					//?·????￡?У???
			}
	//---------------------------------------------------------------------------------//
			var32c = Modify_B_ACT; //???·У?
			var32c = var32c - Modify_A_ACT;
			var32c = var32c << 14;
			var16a=Modify_D_READ-Modify_C_READ;
			var16a=var16a*2;
			var32c=var32c/var16a;
			SET_POWERA = var32c;
			var32c = Modify_B_ACT;
			var32c = var32c << 14;
			var32d = SET_POWERA;
			var32d = var32d * (Modify_D_READ*2);
			if (var32c < var32d)
			{
				var32d = var32d - var32c;
				SET_POWERA_Offset = var32d;
				Polar4 |= 0x04;
			}
			else 
			{
				var32c = var32c - var32d;
				SET_POWERA_Offset = var32c;
				Polar4 &= ~0x04;
			}
			Flash_Write_all ();	
			Flag_DAC_OFF=0;
		}
/*******************************????????У?******************************************/	
		if (UART_Buffer_Rece[1] == 0x0B)			   //?·?У?
		{
			Modify_A_READ = Vmon_value;//??·
			Modify_C_READ = Contr_Voltage;//???·
			Modify_A_ACT = (UART_Buffer_Rece[3] << 8) + UART_Buffer_Rece[4];
		}

		if (UART_Buffer_Rece[1] == 0x0C)			   //?·?У???
		{
			vu16 var16;
			vu32 var32a;
			vu32 var32b;
			
			vu16 var16a;
			vu32 var32c;
			vu32 var32d;
			
			Modify_D_READ = Contr_Voltage;
			Modify_B_READ = Vmon_value;
			Modify_B_ACT = (UART_Buffer_Rece[3] << 8) + UART_Buffer_Rece[4];
			var32a = Modify_B_ACT;
			var32a = var32a - Modify_A_ACT;
			var32a = var32a << 14;
			var16 = Modify_B_READ - Modify_A_READ;
			var32a = var32a / var16;
			REG_POWERV = var32a;
			var32a = Modify_B_ACT;
			var32a = var32a << 14;
			var32b = Modify_B_READ;
			var32b = var32b * REG_POWERV;
			if (var32a < var32b)
			{
				var32b = var32b - var32a;
				REG_POWERV_Offset = var32b;
				Polar5 |= 0x01;
			}
			else 
			{
				var32a = var32a - var32b;
				REG_POWERV_Offset = var32a;
				Polar5 &= ~0x01;					
			}
	//---------------------------------------------------------------------------------//
			var32c = Modify_B_ACT; //????У?
			var32c = var32c - Modify_A_ACT;
			var32c = var32c << 14;
			var16a=Modify_D_READ-Modify_C_READ;
			var16a=var16a*2;
			var32c=var32c/var16a;
			SET_POWERV = var32c;
			var32c = Modify_B_ACT;
			var32c = var32c << 14;
			var32d = SET_POWERV;
			var32d = var32d * (Modify_D_READ*2);
			if (var32c < var32d)
			{
				var32d = var32d - var32c;
				SET_POWERV_Offset = var32d;
				Polar5 |= 0x04;
			}
			else 
			{
				var32c = var32c - var32d;
				SET_POWERV_Offset = var32c;
				Polar5 &= ~0x04;
			}
			Flash_Write_all ();	
			Flag_DAC_OFF=0;
		}
/****************???·?У?**********************************/
		if (UART_Buffer_Rece[1] == 0x0D)			  
		{ 
			Modify_A_READ = Imon_value;
			Modify_A_ACT = (UART_Buffer_Rece[3] << 8) + UART_Buffer_Rece[4];
		}

		if (UART_Buffer_Rece[1] == 0x0E)			   
		{
			vu16 var16;
			vu32 var32a;
			vu32 var32b;
			
			vu16 var16a;
			vu32 var32c;
			vu32 var32d;
			
			Modify_B_READ = Imon_value;
			Modify_B_ACT = (UART_Buffer_Rece[3] << 8) + UART_Buffer_Rece[4];
			var32a = Modify_B_ACT;
			var32a = var32a - Modify_A_ACT;
			var32a = var32a << 14;
			var16 = Modify_B_READ - Modify_A_READ;
			var32a = var32a / var16;
			CON_POWERA = var32a;
			var32a = Modify_B_ACT;
			var32a = var32a << 14;
			var32b = Modify_B_READ;
			var32b = var32b * CON_POWERA;
			if (var32a < var32b)
			{
				var32b = var32b - var32a;
				CON_POWERA_Offset = var32b;
				Polar3 |= 0x04;
			}
			else 
			{
				var32a = var32a - var32b;
				CON_POWERA_Offset = var32a;
				Polar3 &= ~0x04;					
			}
			Flash_Write_all ();	
			Flag_DAC_OFF=0;
		}
/****************漏电流校准**********************************/
		if (UART_Buffer_Rece[1] == 0x22)			  
		{ 
			Modify_A_READ = LowImon_value - 180;
			Modify_A_ACT = (UART_Buffer_Rece[3] << 8) + UART_Buffer_Rece[4];
		}

		if (UART_Buffer_Rece[1] == 0x23)			   
		{
			vu32 var16;
			vu32 var32a;
			vu32 var32b;
			
			vu32 var16a;
			vu32 var32c;
			vu32 var32d;
			Modify_B_READ =LowImon_value - 180;//????
			Modify_B_ACT = (UART_Buffer_Rece[3] << 8) + UART_Buffer_Rece[4];//????
			var32a = Modify_B_ACT;
			var32a = var32a - Modify_A_ACT;
			var32a = var32a << 14;
			var16 = Modify_B_READ - Modify_A_READ;
			var32a = var32a / var16;
			REG_LEAKI = var32a;
			var32a=0;
			var32a = Modify_B_ACT;
			var32a = var32a << 14;
			var32b = Modify_B_READ;
			var32b = var32b * REG_LEAKI;
			if (var32a < var32b)
			{
				var32b = var32b - var32a;
				REG_LEAKI_Offset = var32b;
				Polar6 |= 0x01;
			}
			else 
			{
				var32a = var32a - var32b;
				REG_LEAKI_Offset = var32a;
				Polar6 &= ~0x01;
			}			
			Flash_Write_all();	//??д?FLASH
			Flag_DAC_OFF=0;
		}
/***********??DAC*******************************************/
		if (UART_Buffer_Rece[1] == 0x0F)			   
		{
			Contr_Laod = (UART_Buffer_Rece[3] << 8) + UART_Buffer_Rece[4];
			Flag_DAC_OFF=1;
		}
		if (UART_Buffer_Rece[1] == 0x20)			   
		{
			Contr_Voltage = (UART_Buffer_Rece[3] << 8) + UART_Buffer_Rece[4];
			Flag_DAC_OFF=1;
		}
		if (UART_Buffer_Rece[1] == 0x21)			   
		{
			Contr_Current = (UART_Buffer_Rece[3] << 8) + UART_Buffer_Rece[4];
			Flag_DAC_OFF=1;
		}
	}
//===================================================================================
}
//===============================AD值转换成测量值============================================//
void Transformation_ADC(void)  
{
	vu32 var32;
	vu32 var32a;
/*****************************内阻测量电压转换*******************************************/
	var32 = Vmon1_value;
	var32 = var32 * REG_CorrectionV;  
	if ((Polar & 0x01) == 0x01)		  
	{
		if (var32 < REG_ReadV_Offset) 
		{
			var32 = 0;
		}
		else var32 = var32 - REG_ReadV_Offset;
	}
	else var32 = var32 + REG_ReadV_Offset;
	var32 = var32 >> 12;
	if (var32 < 30) var32 = 0;				  //40mV以下清零
	Voltage = var32;
	DISS_Voltage=Voltage;
	DISS_Voltage=DISS_Voltage/1000;//计算显示电压
	var32 = 0;
	/*******************负载测量电流转换**************************************/
	var32 = Imon1_value;
	var32 = var32 * REG_Load_A;  
	if ((Polar1 & 0x01) == 0x01)		  
	{
		if (var32 < REG_LoadA_Offset) 
		{
			var32 = 0;
		}
		else var32 = var32 - REG_LoadA_Offset;
	}
	else var32 = var32 + REG_LoadA_Offset;
	var32 = var32 >> 12;
	Laod_Current = var32;
	DISS_Current=Laod_Current;
	DISS_Current=DISS_Current/1000;//计算显示电流
	var32 = 0;	
	/*************************负载电压和电流控制转换**************************************/
	if(flag_Load_CC==1)
	{
		var32 = SET_Current_Laod;
		var32=var32<<12;   
		if ((Polar1 & 0x04) == 0)			   
		{
			if (var32 < SET_LoadA_Offset) var32 = 0;
			else var32 = var32 - SET_LoadA_Offset;
		}
		else var32 = var32 + SET_LoadA_Offset;
		var32 = var32/SET_LoadA;
		var32=var32>>1;
		if(Flag_DAC_OFF==0)
		{
			Contr_Laod = var32;
		}
		if(SET_Current_Laod==0)
		{
			Contr_Laod=0;
		}
		var32 = 0;
	}
	else
	{
		var32 = SET_Voltage_Laod;
		var32=var32<<12;   
		if ((Polar2 & 0x04) == 0)			   
		{
			if (var32 < SET_LoadA_Offset) var32 = 0;
			else var32 = var32 - SET_LoadV_Offset;
		}
		else var32 = var32 + SET_LoadV_Offset;
		var32 = var32/SET_LoadV;
		var32=var32>>1;
		if(Flag_DAC_OFF==0)
		{
			Contr_Laod = var32;
		}
		if(SET_Voltage_Laod==0)
		{
			Contr_Laod=0;
		}
		var32 = 0;
	}
/*****************************内阻值转换*******************************************/
	var32 = Rmon_value;
    if(r_raly == 1)
    {
        var32 = var32 * REG_CorrectionR;  
        if ((Polar3 & 0x01) == 0x01)		  
        {
            if (var32 < REG_ReadR_Offset) 
            {
                var32 = 0;
            }
            else var32 = var32 - REG_ReadR_Offset;
        }
        else var32 = var32 + REG_ReadR_Offset;
        var32 = var32 >> 12;
        if (var32 < 1)
        {
            var32 = 0;				  //?￡
        }
        R_VLUE = var32;
        var32 = 0;
    }else{
        var32 = var32 * REG_CorrectionRL;  
        if ((Polar3 & 0x01) == 0x01)		  
        {
            if (var32 < REG_ReadRL_Offset) 
            {
                var32 = 0;
            }
            else var32 = var32 - REG_ReadRL_Offset;
        }
        else var32 = var32 + REG_ReadRL_Offset;
        var32 = var32 >> 12;
        if (var32 < 1)
        {
            var32 = 0;				  //?￡
        }
        R_VLUE = var32;
        var32 = 0;
    }  	
	/*****************************稳压电源测量电压转换*******************************************/
	var32 = Vmon_value;
	var32 = var32 * REG_POWERV;  
	if ((Polar5 & 0x01) == 0x01)		  
	{
		if (var32 < REG_POWERV_Offset) 
		{
			var32 = 0;
		}
		else var32 = var32 - REG_POWERV_Offset;
	}
	else var32 = var32 + REG_POWERV_Offset;
	var32 = var32 >> 14;
	if (var32 < 40) var32 = 0;				  //40mV以下清零
	POW_Voltage = var32;
	DISS_POW_Voltage=POW_Voltage;
	DISS_POW_Voltage=DISS_POW_Voltage/100;//计算显示电压
	var32 = 0;
/*****************************稳压电源测量电流转换*******************************************/
	var32 = Imon_value;
	if(flag_CC_MODE==1)
	{
		var32 = var32 * REG_POWERA;	   
		if ((Polar4 & 0x01) == 0x01)			   
		{
			if (var32 < REG_POWERA_Offset) var32 = 0;
			else var32 = var32 - REG_POWERA_Offset;
		}
		else
		{
			var32 = var32 + REG_POWERA_Offset;
		}	
		var32 = var32 >> 14;
		Current = var32;;
		DISS_POW_Current=Current;
		DISS_POW_Current=DISS_POW_Current/1000;//计算显示电流
	}
	else
	{
		var32 = var32 * CON_POWERA;	   
		if ((Polar3 & 0x04) == 0x04)			   
		{
			if (var32 < CON_POWERA_Offset) var32 = 0;
			else var32 = var32 - CON_POWERA_Offset;
		}
		else
		{
			var32 = var32 + CON_POWERA_Offset;
		}	
		var32 = var32 >> 14;
		Current = var32;;
		DISS_POW_Current=Current;
		DISS_POW_Current=DISS_POW_Current/1000;//计算显示电流
	}
/**************************稳压电源设置电压转换******************************************/
	var32 = SET_Voltage;
	var32=var32<<14;   
	if ((Polar5 & 0x04) == 0)			   
	{
		if (var32 < SET_POWERV_Offset) var32 = 0;
		else var32 = var32 - SET_POWERV_Offset;
	}
	else var32 = var32 + SET_POWERV_Offset;
	var32 = var32/SET_POWERV;
	var32=var32>>1;
	Contr_Voltage = var32;
	if(SET_Voltage==0)
	{
		Contr_Voltage=0;
	}
	var32 = 0;
/**************************稳压电源设置电流转换**************************************/
	var32 = SET_Current;
	var32=var32<<14;   
	if ((Polar4 & 0x04) == 0)			   
	{
		if (var32 < SET_POWERA_Offset) var32 = 0;
		else var32 = var32 - SET_POWERA_Offset;
	}
	else var32 = var32 + SET_POWERA_Offset;
	var32 = var32/SET_POWERA;
	var32=var32>>1;
	Contr_Current = var32;
	if(SET_Current==0)
	{
		Contr_Current=0;
	}
	
	var32 = 0;
/*******************测量漏电流转换**************************************/
	var32 = LowImon_value-180;
	var32 = var32 * REG_LEAKI/*REG_LEAKI*/;  
	if ((Polar6 & 0x01) == 0x01)		  
	{
		if (var32 < REG_LEAKI_Offset) 
		{
			var32 = 0;
		}
		else var32 = var32 - REG_LEAKI_Offset/*REG_LEAKI_Offset*/;
	}
	else var32 = var32 + REG_LEAKI_Offset/*REG_LEAKI_Offset*/;
	var32 = var32 >> 14;
	Leak_I = var32;
    if(Leak_I > 200000)
    {
        DISS_Leak_Current = 0;
    }else{
        DISS_Leak_Current=Leak_I;
    }	
	DISS_Leak_Current=DISS_Leak_Current/100;//计算显示电流
	var32 = 0;
}
/********************************************************************************
  * 名称 :   MODBUS_Conrtl
  * 作者 :  zhan
  * 版本 :  V1.0
  * 日期 :  2016-xx-xx
  * 作用 :  MODBUS解析函数
  ******************************************************************************/ 
//-----------------------------CRC检测--------------------------------------------//
vu16 Hardware_CRC(vu8 *p_buffer,vu8 count)    //CRC16
{
	vu16 CRC_Result=0xffff;
	vu8 i;
	if(count==0)
	{
		count=1;
	}
	while(count--)
	{
		CRC_Result^=*p_buffer;
		for(i=0;i<8;i++)
		{
			if(CRC_Result&1)
			{
				CRC_Result>>=1;
				CRC_Result^=0xA001;
			}
			else 
			{
				CRC_Result>>=1;
			}
		}
		p_buffer++;
	}
	return CRC_Result;
}
