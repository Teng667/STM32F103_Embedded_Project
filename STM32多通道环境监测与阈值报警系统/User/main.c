#include "stm32f10x.h"
#include "OLED.h"
#include "ADC.h"
#include "usart.h"
#include "Key.h"
#include "beep_led.h"
#include "Delay.h"

u8 menu_state = 0;       // 菜单状态 0~3
u8 set_state  = 0;       // 0=设置温度阈值 1=设置光照阈值
u16 temp_th   = 2500;    // 温度报警阈值
u16 light_th  = 2000;    // 光照报警阈值
// 临时缓冲
u16 temp_th_buf;
u16 light_th_buf;  
int main(void)
{
    u16 adc_pot, adc_temp, adc_light;
	u8 last_menu = 0;				// 记录上一次菜单，用于判断是否需要清屏

    // 所有外设初始化
    OLED_Init();
    USART1_Init(9600);
    AD_Init();
    Key_Init();
    BeepLed_Init();

    Delay_ms(1000);
    OLED_Clear();

    while(1)
    {
        // 采集三路ADC数据
        adc_pot   = AD_Value[0];
        adc_temp  = AD_Value[1];
        adc_light = AD_Value[2];

        // 按键处理
        if(Key_GetNum() == 1)
        {
            if(menu_state == 3)
            {
                // 菜单3内：第一次按切换设置项，第二次按退出菜单3
                if(set_state == 0)
                {
                    set_state = 1;
                }
                else
                {
                    set_state = 0;
					 // 退出设置页：缓冲值存入真实阈值 永久保存
					temp_th = temp_th_buf;
                    light_th = light_th_buf;
                    menu_state++;   // 退出设置页，回到主菜单
                }
                OLED_Clear();
            }
            else
            {
                // 正常菜单切换 0→1→2→3→0
                menu_state++;
                if(menu_state >= 4)
                {
                    menu_state = 0;
                }
				// 刚进入阈值设置菜单3，初始化缓冲
                if(menu_state == 3)
                {
                    temp_th_buf = temp_th;
                    light_th_buf = light_th;
                }
            }
        }

        // 只有菜单切换时才清屏，同菜单只刷新数据，不闪烁
        if(menu_state != last_menu)
        {
            OLED_Clear();
            last_menu = menu_state;
        }

        switch(menu_state)
        {
            case 0:
                OLED_ShowString(1,1,"Pot:");
                OLED_ShowNum(1,6,adc_pot,4);
                break;
            case 1:
                OLED_ShowString(1,1,"Temp:");
                OLED_ShowNum(1,6,adc_temp,4);
                break;
            case 2:
                OLED_ShowString(1,1,"Light:");
                OLED_ShowNum(1,6,adc_light,4);
                break;
            case 3:
                if(set_state == 0)
                {
                    OLED_ShowString(1,1,"Set Temp Th");
                    // 只修改缓冲，不改动真实阈值
                    temp_th_buf = adc_pot;
                    OLED_ShowNum(2,1,temp_th_buf,4);
                }
                else
                {
                    OLED_ShowString(1,1,"Set Light Th");
                    // 只修改缓冲，不改动真实阈值
                    light_th_buf = adc_pot;
                    OLED_ShowNum(2,1,light_th_buf,4);
                }
                break;
        }

        // 阈值超标声光报警
        if(adc_temp > temp_th || adc_light > light_th)
        {
            BEEP_ON;
            LED_ON;
        }
        else
        {
            BEEP_OFF;
            LED_OFF;
        }

        // 串口打印数据
        printf("Pot:%d Temp:%d Light:%d TempTh:%d LightTh:%d\r\n",adc_pot,adc_temp,adc_light,temp_th,light_th);
        Delay_ms(500);
    }
}
