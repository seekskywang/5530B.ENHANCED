/******************************************************************/
/* 名称：                                                  */
/* 效果：                                                        */
/* 内容：负载界面                                                     */
/* 作者：zhan                                                  */
/* 联系方式QQ:363116119                                        */
/******************************************************************/
#include "MainTask.h"
#include  "gui.h"
#include "my_register.h" 
#include "tm1650.h"
#include "stdio.h"
#include "stdlib.h"
#include "key.h"
#include "string.h"
#include "beep.h"


WM_HWIN load_wind;//负载界面句柄
vu8 load_sw = load_off;
extern vu16 load_time;
void test_load(void);
extern vu8 clear_flag3;
extern vu16 battery_c;
extern vu8 mode_sw;

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
} flagA, flagB,flagC,flagD,flagE,flagF,flagG;


#define ID_WINDOW_2      	(GUI_ID_USER + 0x09)
#define ID_BUTTON_6     	(GUI_ID_USER + 0x0A)
#define ID_BUTTON_7     	(GUI_ID_USER + 0x0B)
#define ID_BUTTON_8     	(GUI_ID_USER + 0x0C)
#define ID_BUTTON_9     	(GUI_ID_USER + 0x0D)
#define ID_BUTTON_10     	(GUI_ID_USER + 0x0E)
#define ID_BUTTON_11     	(GUI_ID_USER + 0x0F)
#define ID_TEXT_11    	 	(GUI_ID_USER + 0x10)
#define ID_TEXT_12     		(GUI_ID_USER + 0x11)
#define ID_TEXT_13     		(GUI_ID_USER + 0x1E)
#define ID_TEXT_14     		(GUI_ID_USER + 0x1F)
#define ID_TEXT_48    	    (GUI_ID_USER + 0x6D)
#define ID_TEXT_49    	    (GUI_ID_USER + 0x6E)
#define ID_TEXT_84          (GUI_ID_USER + 0x91)
#define ID_TEXT_85          (GUI_ID_USER + 0x92)
#define ID_TEXT_85          (GUI_ID_USER + 0x92)
#define ID_TEXT_89          (GUI_ID_USER + 0x96)
#define ID_TEXT_116         (GUI_ID_USER + 0x10D)
#define ID_TEXT_120         (GUI_ID_USER + 0x10E)
#define ID_TEXT_121         (GUI_ID_USER + 0x10F)
#define ID_TEXT_122         (GUI_ID_USER + 0x110)
#define ID_TEXT_123         (GUI_ID_USER + 0x111)
#define ID_TEXT_126         (GUI_ID_USER + 0x112)

#define ID_TimerTime1    2

static const GUI_WIDGET_CREATE_INFO _aDialogCreate2[] = {
    { WINDOW_CreateIndirect, "Window", ID_WINDOW_2, 0, 0, 480, 272, 0, 0x0, 0 },
//     { BUTTON_CreateIndirect, "Button", ID_BUTTON_6, 3, 226, 77, 45, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Button", ID_BUTTON_7, 83, 226, 77, 45, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Button", ID_BUTTON_8, 163, 226, 77, 45, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Button", ID_BUTTON_9, 243, 226, 77, 45, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Button", ID_BUTTON_10, 323, 226, 77, 45, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Button", ID_BUTTON_11, 403, 226, 77, 45, 0, 0x0, 0 },
    { TEXT_CreateIndirect,   "Text",   ID_TEXT_11, 28, 50, 64, 32, 0, 0x64, 0 },
    { TEXT_CreateIndirect,   "Text",   ID_TEXT_12, 28, 120, 64, 32, 0, 0x64, 0 },
    { TEXT_CreateIndirect,   "Text",   ID_TEXT_13, 240, 50, 32, 30, 0, 0x0, 0 },
    { TEXT_CreateIndirect,   "Text",   ID_TEXT_14, 240, 121, 32, 30, 0, 0x0, 0 },
    { TEXT_CreateIndirect,   "Text",   ID_TEXT_48, 290, 75, 80, 20, 0, 0x0, 0 },
    { TEXT_CreateIndirect,   "Text",   ID_TEXT_49, 380, 75, 65, 20, 0, 0x0, 0 },
    { TEXT_CreateIndirect,   "Text",   ID_TEXT_120, 290, 125, 80, 20, 0, 0x0, 0 },
    { TEXT_CreateIndirect,   "Text",   ID_TEXT_121, 380, 125, 65, 20, 0, 0x0, 0 },
    { TEXT_CreateIndirect,   "Text",   ID_TEXT_122, 290, 100, 80, 20, 0, 0x0, 0 },
    { TEXT_CreateIndirect,   "Text",   ID_TEXT_123, 380, 100, 65, 20, 0, 0x0, 0 },
    { TEXT_CreateIndirect,   "Text",   ID_TEXT_84, 95, 48, 150, 40, 0, 0x0, 0 },
    { TEXT_CreateIndirect,   "Text",   ID_TEXT_85, 95, 116, 150, 40, 0, 0x0, 0 },
    { TEXT_CreateIndirect,   "Text",   ID_TEXT_89, 400, 4, 50, 20, 0, 0x0, 0 },
    { TEXT_CreateIndirect,   "Text",   ID_TEXT_116, 95, 184, 150, 40, 0, 0x0, 0 },
    { TEXT_CreateIndirect,   "Text",   ID_TEXT_126, 300, 2, 80, 20, 0, 0x0, 0 },
  // USER START (Optionally insert additional widgets)
  // USER END
};


static void _cbDialog2(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;
  char    buf[6];
  static vu8 status_flash;
  float   dis_load_c = (float)SET_Current_Laod/1000;
  float   dis_load_v = (float)SET_Voltage_Laod/1000;
  // USER START (Optionally insert additional variables)
  // USER END

  switch (pMsg->MsgId) {

	case WM_PAINT://无效化重绘
		GUI_SetColor(GUI_WHITE);//设置画笔颜色
        GUI_FillRect(0,25,480,26);//画一条实线
        GUI_SetColor(GUI_ORANGE);
        GUI_SetFont(&GUI_Fontsymbol);
        GUI_UC_SetEncodeUTF8();
        GUI_SetTextMode(GUI_TM_TRANS);//设置文本模式为底色透明
        GUI_DispStringAt("Jinko", 5, 1);//SET
        GUI_SetColor(GUI_WHITE);
        GUI_SetFont(&GUI_FontHZ20S);
        GUI_UC_SetEncodeUTF8();
        GUI_SetTextMode(GUI_TM_TRANS);//设置文本模式为底色透明
        GUI_DispStringAt("测量显示", 130, 3);//SET
        GUI_SetColor(GUI_WHITE);//设置前景色为白色
//         GUI_SetFont(&GUI_FontD24x32);//
// 		GUI_GotoXY(95,72);//设置显示坐标
// 		GUI_DispFloatFix(DISS_Voltage,5,2);//显示电压值
// 		GUI_GotoXY(95,140);//设置显示坐标
// 		GUI_DispFloatFix(DISS_Current,5,2);//显示电流值
        GUI_SetFont(&GUI_Font24_1);
        GUI_DispStringAt("A",450,75);
        GUI_DispStringAt("V",450,100.);
    
        GUI_SetFont(&GUI_FontEN40);
        GUI_SetColor(GUI_LIGHTGRAY);
        GUI_DispStringAt("C:", 28, 190);
        GUI_SetColor(GUI_LIGHTGRAY);
        GUI_DispStringAt("m", 217, 196);
        GUI_DispStringAt("AH", 238, 187);
        
        GUI_SetColor(GUI_GREEN);
        GUI_SetFont(&GUI_Fontunit);
        GUI_DispStringAt("°",342, 2);
        GUI_SetFont(&GUI_Font24_1);
        GUI_DispStringAt("C",350, 2);
		break;
	case WM_KEY://接受按键消息来处理按键功能
		switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
		{ 
			case GUI_KEY_TAB:

			break;
			
			case GUI_KEY_ENTER:
			break;
		}
	break;
		
	case WM_TIMER://定时模块消息
	if(WM_GetTimerId(pMsg->Data.v) == ID_TimerTime1)
	{
        if(clear_flag3 == 1)
        {
            if(DISS_Voltage <= clear_lv)
            {
                hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_84);
                sprintf(buf,"%.3f",0);       
                TEXT_SetText(hItem,buf);
            }else{
                hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_84);
                sprintf(buf,"%.3f",DISS_Voltage);       
                TEXT_SetText(hItem,buf);
            }
        }else{
            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_84);
            sprintf(buf,"%.3f",DISS_Voltage);       
            TEXT_SetText(hItem,buf);
        }
        if((float)SET_Current_Laod/1000 * DISS_Voltage > 200){
            SET_Current_Laod = 0;
            hItem = WM_GetDialogItem(load_wind, ID_TEXT_49);
            TEXT_SetBkColor(hItem,GUI_INVALID_COLOR);//选项背景色设为透明
            TEXT_SetTextColor(hItem, GUI_WHITE);            
            if(SET_Current_Laod > 55000 || (float)SET_Current_Laod/1000 * DISS_Voltage > 200){
                SET_Current_Laod = 0;
            }
            dis_load_c = (float)SET_Current_Laod/1000;
            sprintf(buf,"%.3f",dis_load_c);
            TEXT_SetText(hItem,buf);
        }
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_85);
        sprintf(buf,"%.3f",DISS_Current);        
		TEXT_SetText(hItem,buf);
        
//        test_load();
        
        if(load_sw == load_on)
        {
            if(/*DISS_Current >= 30*/SET_Current_Laod >= 30000)
            {
                alert_flag = 1;
            }else{
                alert_flag = 0;
            }
            battery_c = (int)bc_raw;
            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_116);
            sprintf(buf,"%05d",battery_c);    
            TEXT_SetText(hItem,buf);
            if(status_flash == 0){
                hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_89);
                TEXT_SetTextColor(hItem, GUI_RED);//设置字体颜色
                TEXT_SetFont(hItem,&GUI_FontHZ16);//设定文本字体
                GUI_UC_SetEncodeUTF8();        
                TEXT_SetText(hItem,"放电中");
                status_flash = 1;
            }else{
                hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_89);
                TEXT_SetText(hItem,"");
                status_flash = 0;
            }

            
        }else{
            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_89);
            TEXT_SetText(hItem,"");
        }
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_126);       
        sprintf(buf,"%.1f",temp);
        TEXT_SetText(hItem,buf);
        
		WM_RestartTimer(pMsg->Data.v, 50);
	}
	break;
	
    case WM_INIT_DIALOG://重绘外观
    //
    // Initialization of 'Window'
    //
        hItem = pMsg->hWin;
        WINDOW_SetBkColor(hItem, GUI_BLACK);
		WM_CreateTimer(hItem,ID_TimerTime1,500,0);//创建本窗口定时器
    //
    // Initialization of 'Button'
    //
//         hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_6);
// 		BUTTON_SetTextColor(hItem,0,GUI_BLACK);//设置字体颜色为黑色
// 		BUTTON_SetFont      (hItem,    &GUI_FontHZ16);//设定按钮文本字体
// 		GUI_UC_SetEncodeUTF8();
// 		BUTTON_SetText(hItem,"充放电");
    //
    // Initialization of 'Button'
    //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_7);
//		BUTTON_SetTextColor(hItem,0,GUI_BLACK);//设置字体颜色为黑色
		BUTTON_SetFont      (hItem,    &GUI_FontHZ16);//设定按钮文本字体
		GUI_UC_SetEncodeUTF8();
        BUTTON_SetText(hItem, "内阻测试");
    //
    // Initialization of 'Button'
    //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_8);
        BUTTON_SetPressed(hItem,1);
//        BUTTON_SetTextColor(hItem,0,GUI_BLACK);//设置字体颜色为黑色
		BUTTON_SetFont      (hItem,    &GUI_FontHZ16);//设定按钮文本字体
		GUI_UC_SetEncodeUTF8();
        BUTTON_SetText(hItem, "程控负载");
    //
    // Initialization of 'Button'
    //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_9);
//        BUTTON_SetTextColor(hItem,0,GUI_BLACK);//设置字体颜色为黑色
		BUTTON_SetFont      (hItem,    &GUI_FontHZ16);//设定按钮文本字体
		GUI_UC_SetEncodeUTF8();
        BUTTON_SetText(hItem, "程控电源");
    //
    // Initialization of 'Button'
    //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_10);
//        BUTTON_SetTextColor(hItem,0,GUI_BLACK);//设置字体颜色为黑色
		BUTTON_SetFont      (hItem,    &GUI_FontHZ16);//设定按钮文本字体
		GUI_UC_SetEncodeUTF8();
        BUTTON_SetText(hItem, "充放电");
    //
    // Initialization of 'Button'
    //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_11);
//        BUTTON_SetTextColor(hItem,0,GUI_BLACK);//设置字体颜色为黑色
		BUTTON_SetFont      (hItem,    &GUI_FontHZ16);//设定按钮文本字体
		GUI_UC_SetEncodeUTF8();
        BUTTON_SetText(hItem, "曲线");
		
		
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_11);
		TEXT_SetTextColor(hItem, GUI_LIGHTGRAY);//设置字体颜色
        TEXT_SetFont(hItem,&GUI_FontEN40);//设定文本字体
		GUI_UC_SetEncodeUTF8();
		TEXT_SetText(hItem,"V:");
		
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_12);
		TEXT_SetTextColor(hItem, GUI_LIGHTGRAY);//设置字体颜色
        TEXT_SetFont(hItem,&GUI_FontEN40);//设定文本字体
		GUI_UC_SetEncodeUTF8();
		TEXT_SetText(hItem,"I:");
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_13);
		TEXT_SetTextColor(hItem, GUI_LIGHTGRAY);//设置字体颜色
        TEXT_SetFont(hItem,&GUI_FontEN40);//设定文本字体
		GUI_UC_SetEncodeUTF8();
		TEXT_SetText(hItem,"V");
		
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_14);
		TEXT_SetTextColor(hItem, GUI_LIGHTGRAY);//设置字体颜色
        TEXT_SetFont(hItem,&GUI_FontEN40);//设定文本字体
		GUI_UC_SetEncodeUTF8();
		TEXT_SetText(hItem,"A");
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_48);
		TEXT_SetTextColor(hItem, GUI_BLACK);//设置字体颜色
        TEXT_SetBkColor(hItem,0x00BFFFFF);
        TEXT_SetFont(hItem,&GUI_Fontset_font);//设定文本字体
		GUI_UC_SetEncodeUTF8();
		TEXT_SetText(hItem,"负载电流");
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_49);
        sprintf(buf,"%.3f",dis_load_c);
		TEXT_SetTextColor(hItem, GUI_WHITE);//设置字体颜色
        TEXT_SetFont(hItem,&GUI_Font24_1);//设定文本字体
		GUI_UC_SetEncodeUTF8();     
		TEXT_SetText(hItem,buf);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_84);
        sprintf(buf,"%.3f",0);
		TEXT_SetTextColor(hItem, GUI_GREEN);//设置字体颜色
        TEXT_SetFont(hItem,&GUI_FontD24x32);//设定文本字体
		GUI_UC_SetEncodeUTF8();        
		TEXT_SetText(hItem,buf);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_85);
        sprintf(buf,"%.3f",0);
		TEXT_SetTextColor(hItem, GUI_GREEN);//设置字体颜色
        TEXT_SetFont(hItem,&GUI_FontD24x32);//设定文本字体
		GUI_UC_SetEncodeUTF8();        
		TEXT_SetText(hItem,buf);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_116);
        sprintf(buf,"%05d",0);
        TEXT_SetTextColor(hItem, GUI_GREEN);//设置字体颜色
        TEXT_SetFont(hItem,&GUI_FontD24x32);//设定文本字体
        GUI_UC_SetEncodeUTF8();        
        TEXT_SetText(hItem,buf);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_120);
        TEXT_SetTextColor(hItem, GUI_WHITE);//设置字体颜色
        TEXT_SetFont(hItem,&GUI_Fontset_font);//设定文本字体
        GUI_UC_SetEncodeUTF8();        
        TEXT_SetText(hItem,"负载模式");
        
        if(flag_Load_CC == 1)
        {
            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_121);
            TEXT_SetTextColor(hItem, GUI_WHITE);//设置字体颜色
            TEXT_SetFont(hItem,&GUI_Font24_1);//设定文本字体
            GUI_UC_SetEncodeUTF8();        
            TEXT_SetText(hItem,"CC");
        }else{
            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_121);
            TEXT_SetTextColor(hItem, GUI_WHITE);//设置字体颜色
            TEXT_SetFont(hItem,&GUI_Font24_1);//设定文本字体
            GUI_UC_SetEncodeUTF8();        
            TEXT_SetText(hItem,"CV");
        }
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_122);
		TEXT_SetTextColor(hItem, GUI_WHITE);//设置字体颜色
        TEXT_SetFont(hItem,&GUI_Fontset_font);//设定文本字体
		GUI_UC_SetEncodeUTF8();
		TEXT_SetText(hItem,"负载电压");
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_123);
        sprintf(buf,"%.3f",dis_load_v);
		TEXT_SetTextColor(hItem, GUI_WHITE);//设置字体颜色
        TEXT_SetFont(hItem,&GUI_Font24_1);//设定文本字体
		GUI_UC_SetEncodeUTF8();     
		TEXT_SetText(hItem,buf);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_126);
        TEXT_SetTextColor(hItem, GUI_GREEN);//设置字体颜色
        TEXT_SetFont(hItem,&GUI_Font24_1);//设定文本字体       
        sprintf(buf,"%.1f",temp);
        TEXT_SetText(hItem,buf);
        

    // USER START (Optionally insert additional code for further widget initialization)
    // USER END
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case ID_BUTTON_6: // Notifications sent by 'Button'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_7: // Notifications sent by 'Button'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
				
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_8: // Notifications sent by 'Button'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_9: // Notifications sent by 'Button'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_10: // Notifications sent by 'Button'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_11: // Notifications sent by 'Button'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    // USER START (Optionally insert additional code for further Ids)
    // USER END
    }
    break;
  // USER START (Optionally insert additional message handling)
  // USER END
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}

WM_HWIN CreateWindow2(void) {
  page_sw = face_load;
  track = face_load;
  set_sw = set_24;
  SET_Current_Laod = load_c;
  pass = 0;
  IO_OFF();
  mode_sw = mode_load;
//   GPIO_SetBits(GPIOA,GPIO_Pin_15);//电子负载OFF
//   GPIO_ResetBits(GPIOC,GPIO_Pin_1);//关闭电源输出
//   GPIO_SetBits(GPIOC,GPIO_Pin_13);//关闭电源输出继电器
  TM1650_SET_LED(0x68,0x70);
  GPIO_ResetBits(GPIOD,GPIO_Pin_12);//灭灯
  load_wind = GUI_CreateDialogBox(_aDialogCreate2, GUI_COUNTOF(_aDialogCreate2), _cbDialog2, WM_HBKWIN, 0, 0);
  return load_wind;
}

WM_HWIN ResetLoad(void) {
  load_wind = GUI_CreateDialogBox(_aDialogCreate2, GUI_COUNTOF(_aDialogCreate2), _cbDialog2, WM_HBKWIN, 0, 0);
  return load_wind;
}

void LOAD_OP_DOWN(void);
void LOAD_OP_DOWN(void)
{
    switch(set_sw)
    {
        case set_24:
        {
            WM_HWIN hItem;
            hItem = WM_GetDialogItem(load_wind, ID_TEXT_48);
            TEXT_SetBkColor(hItem,GUI_INVALID_COLOR);//选项背景色设为透明
            TEXT_SetTextColor(hItem, GUI_WHITE);
            
            hItem = WM_GetDialogItem(load_wind, ID_TEXT_122);
            TEXT_SetBkColor(hItem,0x00BFFFFF);//选项背景色设为米色
            TEXT_SetTextColor(hItem, GUI_BLACK);

            set_sw = set_66;
            break;
        }
        case set_66:
        {
            WM_HWIN hItem;
            hItem = WM_GetDialogItem(load_wind, ID_TEXT_122);
            TEXT_SetBkColor(hItem,GUI_INVALID_COLOR);//选项背景色设为透明
            TEXT_SetTextColor(hItem, GUI_WHITE);
            
            hItem = WM_GetDialogItem(load_wind, ID_TEXT_120);
            TEXT_SetBkColor(hItem,0x00BFFFFF);//选项背景色设为米色
            TEXT_SetTextColor(hItem, GUI_BLACK);

            set_sw = set_67;
            break;
        }
    }   
}

void LOAD_OP_UP(void);
void LOAD_OP_UP(void)
{
    switch(set_sw)
    {
        case set_66:
        {
            WM_HWIN hItem;
            hItem = WM_GetDialogItem(load_wind, ID_TEXT_122);
            TEXT_SetBkColor(hItem,GUI_INVALID_COLOR);//选项背景色设为透明
            TEXT_SetTextColor(hItem, GUI_WHITE);
            
            hItem = WM_GetDialogItem(load_wind, ID_TEXT_48);
            TEXT_SetBkColor(hItem,0x00BFFFFF);//选项背景色设为米色
            TEXT_SetTextColor(hItem, GUI_BLACK);

            set_sw = set_24;
            break;
        }
        case set_67:
        {
            WM_HWIN hItem;
            hItem = WM_GetDialogItem(load_wind, ID_TEXT_120);
            TEXT_SetBkColor(hItem,GUI_INVALID_COLOR);//选项背景色设为透明
            TEXT_SetTextColor(hItem, GUI_WHITE);
            
            hItem = WM_GetDialogItem(load_wind, ID_TEXT_122);
            TEXT_SetBkColor(hItem,0x00BFFFFF);//选项背景色设为米色
            TEXT_SetTextColor(hItem, GUI_BLACK);

            set_sw = set_66;
            break;
        }
    }   
}

//负载界面设置确认
void LOAD_SET(void);
void LOAD_SET(void) {
    char    buf[6];
    float   dis_load_c;
    float   dis_load_v;
    vu8 i;
    switch(set_sw)
    {
        case set_24:
        {
            WM_HWIN hItem;
//            WM_InvalidateWindow(load_wind);
            hItem = WM_GetDialogItem(load_wind, ID_TEXT_48);
            TEXT_SetBkColor(hItem,GUI_INVALID_COLOR);//选项背景色设为透明
            TEXT_SetTextColor(hItem, GUI_WHITE);
                   
            hItem = WM_GetDialogItem(load_wind, ID_TEXT_49);
            TEXT_SetBkColor(hItem,0x00BFFFFF);//选项背景色设为米色
            TEXT_SetTextColor(hItem, GUI_BLACK);
            for(i=0;i<5;i++)
            {
                set_limit[i] = '\0';
            }
            set_sw = set_25;
            break;
        }
        case set_25:
        {
            WM_HWIN hItem;
//            WM_InvalidateWindow(load_wind);
            hItem = WM_GetDialogItem(load_wind, ID_TEXT_49);
            TEXT_SetBkColor(hItem,GUI_INVALID_COLOR);//选项背景色设为透明
            TEXT_SetTextColor(hItem, GUI_WHITE);  
            SET_Current_Laod = load_c;            
            if(SET_Current_Laod > 55000 || (float)SET_Current_Laod/1000 * DISS_Voltage > 200){
                SET_Current_Laod = 0;
            }
            dis_load_c = (float)SET_Current_Laod/1000;
            sprintf(buf,"%.3f",dis_load_c);
            TEXT_SetText(hItem,buf);
            
            hItem = WM_GetDialogItem(load_wind, ID_TEXT_48);
            TEXT_SetBkColor(hItem,0x00BFFFFF);//选项背景色设为米色
            TEXT_SetTextColor(hItem, GUI_BLACK);
            Write_Limits();
            
            set_sw = set_24;
            bit = 1;
            dot_flag = 0;
            break;
        }
        case set_66:
        {
            WM_HWIN hItem;
//            WM_InvalidateWindow(load_wind);
            hItem = WM_GetDialogItem(load_wind, ID_TEXT_122);
            TEXT_SetBkColor(hItem,GUI_INVALID_COLOR);//选项背景色设为透明
            TEXT_SetTextColor(hItem, GUI_WHITE);
                   
            hItem = WM_GetDialogItem(load_wind, ID_TEXT_123);
            TEXT_SetBkColor(hItem,0x00BFFFFF);//选项背景色设为米色
            TEXT_SetTextColor(hItem, GUI_BLACK);
            for(i=0;i<5;i++)
            {
                set_limit[i] = '\0';
            }
            set_sw = set_68;
            break;
        }
        case set_68:
        {
            WM_HWIN hItem;
            hItem = WM_GetDialogItem(load_wind, ID_TEXT_123);
            TEXT_SetBkColor(hItem,GUI_INVALID_COLOR);//选项背景色设为透明
            TEXT_SetTextColor(hItem, GUI_WHITE);            
            if(SET_Voltage_Laod > 80000 || (float)SET_Voltage_Laod/1000 * DISS_Current > 200){
                SET_Voltage_Laod = 0;
            }
            dis_load_v = (float)SET_Voltage_Laod/1000;
            sprintf(buf,"%.3f",dis_load_v);
            TEXT_SetText(hItem,buf);
            
            hItem = WM_GetDialogItem(load_wind, ID_TEXT_122);
            TEXT_SetBkColor(hItem,0x00BFFFFF);//选项背景色设为米色
            TEXT_SetTextColor(hItem, GUI_BLACK);
            Write_Limits();
            
            set_sw = set_66;
            bit = 1;
            dot_flag = 0;
            break;
        }
        case set_67:
        {
            switch(flag_Load_CC)
            {
                case 1:
                {
                    WM_HWIN hItem;
                    hItem = WM_GetDialogItem(load_wind, ID_TEXT_121);
                    TEXT_SetText(hItem,"CV");
                    GPIO_SetBits(GPIOC,GPIO_Pin_10);//CV
                    flag_Load_CC = 0;
                    Write_LOAD();
                    break;
                }
                case 0:
                {
                     WM_HWIN hItem;
                     hItem = WM_GetDialogItem(load_wind, ID_TEXT_121);
                     TEXT_SetText(hItem,"CC");
                     GPIO_ResetBits(GPIOC,GPIO_Pin_10);//CC
                     flag_Load_CC = 1;
                     Write_LOAD();
                     break;
                }
            }   
        }break;
    }
}


void INPUT_LOAD(char* num);            
void INPUT_LOAD(char* num){    
    switch(set_sw)
    {
        case set_25:
        {
            WM_HWIN hItem;
 //           WM_InvalidateWindow(load_wind);
            hItem = WM_GetDialogItem(load_wind, ID_TEXT_49);
            switch(bit){
                case 1:
                {
                    load_c = atoi(num) * 1000;
                    strcat(set_limit,num);            
                    TEXT_SetText(hItem,set_limit);
                    
                    bit = 2;
                    break;
                }
                case 2:
                {
                    strcat(set_limit,num);            
                    TEXT_SetText(hItem,set_limit);
                    if(set_limit[1] == 0x2e)//判断输入是否为小数点
                    {
                        dot_flag = 1;
                    }else{
                        load_c = load_c * 10 + atoi(num) * 1000;
                    }
                    bit = 3;
                    break;
                }
                case 3:
                {
                    strcat(set_limit,num);
                    TEXT_SetText(hItem,set_limit);
                    if(dot_flag == 0)
                    {
                        if(set_limit[2] == 0x2e)//判断输入是否为小数点
                        {                            
                            dot_flag = 2;
                        }else{
                            load_c = 0;
                        }
                    }else{
                        load_c = load_c + atoi(num) * 100;
                    }                  
                    bit = 4;
                    break;
                }
                case 4:
                {
                    strcat(set_limit,num);
                    TEXT_SetText(hItem,set_limit);
                    if(dot_flag == 0)
                    {
                        load_c = 0;
                    }else if(dot_flag == 2){
                        load_c = load_c + atoi(num) * 100;
                    }else{
                        load_c = load_c + atoi(num) * 10;
                    }                  
                    bit = 5;
                    break;
                }
                case 5:
                {
                    strcat(set_limit,num);
                    TEXT_SetText(hItem,set_limit);
                    if(dot_flag == 0)
                    {
                        load_c = 0;
                    }else if(dot_flag == 1){
                        load_c = load_c + atoi(num);
                    }else{
                        load_c = load_c + atoi(num) * 10;
                    }                 
                    bit = 6;
                    break;
                }
                case 6:
                {
                    strcat(set_limit,num);
                    TEXT_SetText(hItem,set_limit);
                    if(dot_flag == 0)
                    {
                        load_c = 0;
                    }else if(dot_flag == 2){
                        load_c = load_c + atoi(num);
                    }                 
                    bit = 1;
                    break;
                }
            }
            break;
            
        }
        case set_68:
        {
            WM_HWIN hItem;
 //           WM_InvalidateWindow(load_wind);
            hItem = WM_GetDialogItem(load_wind, ID_TEXT_123);
            switch(bit){
                case 1:
                {
                    SET_Voltage_Laod = atoi(num) * 1000;
                    strcat(set_limit,num);            
                    TEXT_SetText(hItem,set_limit);
                    
                    bit = 2;
                    break;
                }
                case 2:
                {
                    strcat(set_limit,num);            
                    TEXT_SetText(hItem,set_limit);
                    if(set_limit[1] == 0x2e)//判断输入是否为小数点
                    {
                        dot_flag = 1;
                    }else{
                        SET_Voltage_Laod = SET_Voltage_Laod * 10 + atoi(num) * 1000;
                    }
                    bit = 3;
                    break;
                }
                case 3:
                {
                    strcat(set_limit,num);
                    TEXT_SetText(hItem,set_limit);
                    if(dot_flag == 0)
                    {
                        if(set_limit[2] == 0x2e)//判断输入是否为小数点
                        {                            
                            dot_flag = 2;
                        }else{
                            SET_Voltage_Laod = 0;
                        }
                    }else{
                        SET_Voltage_Laod = SET_Voltage_Laod + atoi(num) * 100;
                    }                  
                    bit = 4;
                    break;
                }
                case 4:
                {
                    strcat(set_limit,num);
                    TEXT_SetText(hItem,set_limit);
                    if(dot_flag == 0)
                    {
                        SET_Voltage_Laod = 0;
                    }else if(dot_flag == 2){
                        SET_Voltage_Laod = SET_Voltage_Laod + atoi(num) * 100;
                    }else{
                        SET_Voltage_Laod = SET_Voltage_Laod + atoi(num) * 10;
                    }                  
                    bit = 5;
                    break;
                }
                case 5:
                {
                    strcat(set_limit,num);
                    TEXT_SetText(hItem,set_limit);
                    if(dot_flag == 0)
                    {
                        SET_Voltage_Laod = 0;
                    }else if(dot_flag == 1){
                        SET_Voltage_Laod = SET_Voltage_Laod + atoi(num);
                    }else{
                        SET_Voltage_Laod = SET_Voltage_Laod + atoi(num) * 10;
                    }                 
                    bit = 6;
                    break;
                }
                case 6:
                {
                    strcat(set_limit,num);
                    TEXT_SetText(hItem,set_limit);
                    if(dot_flag == 0)
                    {
                        SET_Voltage_Laod = 0;
                    }else if(dot_flag == 2){
                        SET_Voltage_Laod = SET_Voltage_Laod + atoi(num);
                    }                 
                    bit = 1;
                    break;
                }
            }
            break;
            
        }
        default:break;
    }
}

void test_load(void)
{

    if(load_sw == load_on && para_set1 == set_1_on)
    {
        if(DISS_Voltage*100 > set_max_v || DISS_Voltage*100 < set_min_v)
        {
           if(para_set4 == set_4_on){
                BEEP_Tiggr();
            }
            TM1650_SET_LED(0x68,0x70);//FAIL灯
            GPIO_SetBits(GPIOD,GPIO_Pin_12);//            
        }else{
            GPIO_ResetBits(GPIOD,GPIO_Pin_12);
            TM1650_SET_LED(0x48,0x71);
            TM1650_SET_LED(0x68,0xF2);//PASS灯
        }        
    }else
    {
        TM1650_SET_LED(0x68,0x70);
        GPIO_ResetBits(GPIOD,GPIO_Pin_12);//灭灯
    }
}
