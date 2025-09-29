#include "zf_common_headfile.h"



int main(void)
{	
    clock_init(SYSTEM_CLOCK_600M);  // ����ɾ��
    debug_init();                   // ���Զ˿ڳ�ʼ��


	#if ENABLE_ZFASS
	My_ZFASSISTANT_Init();        //��ʼ���������
	#endif
	    
    // �Ƽ��ȳ�ʼ������ͷ�����ʼ���������	
	my_imu660ra_init();	
	imu660ra_zeroBias();
	
	ips200_init(IPS200_TYPE_SPI);
	key_init(5);
		
	Gray_Init();
	my_uart_init();        //ע�⣬δ���봮��ʱ��Ҫ���ã�������
	
	//����������ʼ���������77��Ϊ����pit��ʼ��
	motor_Init();
	PID_Init();
	Encoder_Init();
	my_image_init();       //ע�⣬δ��������ͷʱ��Ҫ���ã�������
	
  
	interrupt_set_priority(PIT_IRQn, 2);        // ���� PIT1 �������жϵ��ж����ȼ�Ϊ2
	interrupt_set_priority(LPUART1_IRQn,1);     //ʶ�����ӵ��ж����ȼ�Ϊ1
	interrupt_set_priority(LPUART4_IRQn,0);     //����ģ�͵��ж����ȼ�Ϊ0
	pit_ms_init(PIT_CH0, 3);    //��ȡ�ٶ�                                              
	pit_ms_init(PIT_CH1, 5);    //��ȡ������
	pit_ms_init(PIT_CH2, 5);	//�����
	pit_ms_init(PIT_CH3, 7);	//�ж��ж�
	pit_disable(PIT_CH3);
	interrupt_global_enable(0);
	
	SPEED_ENABLE = 0;
	Motor_Control_Mode = 0;
	Image_Mode = 0;
	
    // �˴���д�û����� ���������ʼ�������
    while(1)
    {

		
		start_finish_line_control();
		Push_Control();
		change_speed();
		
		key_scanner();
		UI();
		
	
		#if ENABLE_ZFASS==1
		send_Image();        //����ͼ��
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
        
        // ���ñ�����Ҫ���ͼ���ͨ��������
        seekfree_assistant_oscilloscope_data.channel_num = 6;
        
        // �����������4��ͨ�������ݣ����֧��8ͨ��
        seekfree_assistant_oscilloscope_send(&seekfree_assistant_oscilloscope_data);
		#endif

        }


}