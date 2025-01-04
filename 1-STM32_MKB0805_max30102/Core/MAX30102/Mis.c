#include "MIS.h"
#include "oled.h"




//等待检测界面
uint16_t  Count =0;   //计数
uint8_t   show_flag=0;
void Max30102_Wait_Interface(void)
{

   OLED_ShowChinese(0,16*2,"等待检测");
   Count++;
   if(Count>100)
   {
      Count=0;
      show_flag++;
      if(show_flag>4)  
      {
         OLED_Clear();
         show_flag=0; 
         Count=0;
      }
    }
   
   switch(show_flag)
   {
      case 0: 
              OLED_ShowString(64,16*2,".",OLED_8X16);
      break;
      
      case 1:
              OLED_ShowString(64,16*2,"..",OLED_8X16);
      break;
            
      case 2:
              
              OLED_ShowString(64,16*2,"...",OLED_8X16);
      break;
                  
      case 3:
             
              OLED_ShowString(64,16*2,"....",OLED_8X16);
      break;
   
      default :
         break;
   }
	 
	 OLED_Update();
   
}


//检测时显示
uint8_t heart_data[3]={0};
uint8_t sp02_data[3]={0};

void OLED_Start_Interface(uint8_t heart_rate, uint8_t sp02)
{
		OLED_ShowString(0,0,"HR:",OLED_8X16);
		OLED_ShowString(8*6,0,"/",OLED_8X16);
		OLED_ShowString(8*7,0,"SP02:",OLED_8X16);		

		OLED_ShowNum(8*3,0,heart_rate,3,OLED_8X16);
		OLED_ShowNum(8*12,0,sp02,2,OLED_8X16);
	
		OLED_ShowString(8*14,0,"%",OLED_8X16);
	
		OLED_Update();
}


