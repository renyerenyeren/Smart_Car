#include "zf_common_headfile.h"



int main(void)
{	
    clock_init(SYSTEM_CLOCK_600M);  // 不可删除
    debug_init();                   // 调试端口初始化


	#if ENABLE_ZFASS
	My_ZFASSISTANT_Init();        //初始化逐飞助手
	#endif
	    
    // 推荐先初始化摄像头，后初始化逐飞助手	
	my_imu660ra_init();	
	imu660ra_zeroBias();
	
	ips200_init(IPS200_TYPE_SPI);
	key_init(5);
		
	Gray_Init();
	my_uart_init();        //注意，未接入串口时不要调用！！！！
	
	//下面三个初始化放在最后，77因为含有pit初始化
	motor_Init();
	PID_Init();
	Encoder_Init();
	my_image_init();       //注意，未接入摄像头时不要调用！！！！
	
  
	interrupt_set_priority(PIT_IRQn, 2);        // 设置 PIT1 对周期中断的中断优先级为2
	interrupt_set_priority(LPUART1_IRQn,1);     //识别到箱子的中断优先级为1
	interrupt_set_priority(LPUART4_IRQn,0);     //运行模型的中断优先级为0
	pit_ms_init(PIT_CH0, 3);    //读取速度                                              
	pit_ms_init(PIT_CH1, 5);    //读取陀螺仪
	pit_ms_init(PIT_CH2, 5);	//总钻风
	pit_ms_init(PIT_CH3, 7);	//判断中断
	pit_disable(PIT_CH3);
	interrupt_global_enable(0);
	
	SPEED_ENABLE = 0;
	Motor_Control_Mode = 0;
	Image_Mode = 0;
	
    // 此处编写用户代码 例如外设初始化代码等
    while(1)
    {

		
		start_finish_line_control();
		Push_Control();
		change_speed();
		
		key_scanner();
		UI();
		
	
		#if ENABLE_ZFASS==1
		send_Image();        //发送图像
		#endif
			

	  	#if ENABLE_ZFASS==2
	    seekfree_assistant_oscilloscope_data.data[0] = wheel[0];
	    seekfree_assistant_oscilloscope_data.data[1] = wheel[1];
        seekfree_assistant_oscilloscope_data.data[2] = wheel[2];
	    seekfree_assistant_oscilloscope_data.data[3] = Read_wheelspeed[0];
        seekfree_assistant_oscilloscope_data.data[4] = Read_wheelspeed[1];
        seekfree_assistant_oscilloscope_data.data[5] = Read_wheelspeed[2];
//		seekfree_assistant_oscilloscope_data.data[6] = w;
//		seekfree_assistant_oscilloscope_data.data[7] = Vx;		
        
        // 设置本次需要发送几个通道的数据
        seekfree_assistant_oscilloscope_data.channel_num = 6;
        
        // 这里进发送了4个通道的数据，最大支持8通道
        seekfree_assistant_oscilloscope_send(&seekfree_assistant_oscilloscope_data);
		#endif

        }


}