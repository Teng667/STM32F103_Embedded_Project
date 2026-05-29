#include "stm32f10x.h"                  
#include "Delay.h"
#include "OLED.h"
#include "MPU6050.h"
#include "usart.h"	
#include "SG90.h"
#include "MyI2C.h"

int16_t AX, AY, AZ, GX, GY, GZ;
// 跟随控制参数
#define ALPHA    0.2f    // 滤波系数，响应更快
#define KP       1.0f    // 比例系数，提高响应速度
#define KD       0.15f   // 微分项阻尼
#define SERVO_MID 90

// 全局滤波变量
float Roll_Filt = 0;
float Roll_Last = 0;

int main(void)
{
	
	
	OLED_Init();		//OLED初始化
	MPU6050_Init();		//MPU6050初始化
	USART1_Init(9600);
	SG90_Init();
	MyI2C_Init();
	OLED_Clear();
    OLED_ShowString(1,1,"MPU6050 Ready");
    Delay_ms(1000);
    OLED_Clear();	
	static uint16_t print_cnt = 0;
	
	while (1)
	{
		
		MPU6050_GetData(&AX, &AY, &AZ, &GX, &GY, &GZ);		//获取MPU6050的数据
		MPU6050_CalcAngle();
	// 1. 一阶低通滤波
        Roll_Filt = ALPHA * Roll + (1 - ALPHA) * Roll_Filt;

        // 2. PD 控制
        float D = (Roll_Filt - Roll_Last) / 0.01f;	//偏差变化速率
        Roll_Last = Roll_Filt;
        float servo = KP * Roll_Filt - KD * D;

        // 映射舵机角度 + 限幅
        servo += SERVO_MID;
        if(servo < 0)   servo = 0;
        if(servo > 180) servo = 180;

        SG90_SetAngle((int)servo);

        // OLED显示姿态角
        OLED_ShowString(1,1,"Pitch:");
        OLED_ShowFloat(1,8,Pitch,6);
        OLED_ShowString(2,1,"Roll:");
        OLED_ShowFloat(2,8,Roll,6);

       // 串口 200ms 打印一次
        print_cnt++;
        if(print_cnt >= 20)
        {
            printf("Pitch:%.2f  Roll:%.2f  Servo:%d\r\n",Pitch,Roll,(int)servo);
            print_cnt = 0;
        }
        Delay_ms(10);
	}
}