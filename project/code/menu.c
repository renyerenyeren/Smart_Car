#include "menu.h"


uint8 openart_bright_scale = 20;
//������ǲ��Բ�������ʵ�ʹ����У������������жϵõ�
bool magnet_state = false;
uint8 datapage = 0;//0����ʾID��Ӧ�ĺ�����1���޸�arrow��Ӧ�Ĳ���
float mul = 1;

//������ʾ�����˵�
uint8 pagenum = 0;//��ʾpage����Ŀ
int8 arrow = 0;//���������ڵ��У���ʼ���ڵ�һ��

uint8 which_Page = 0;
uint8 actualPage = 0;
int8 Page_num[totalPages];


uint8 OBJECT_mode = DEFAULT_OBJECT_MODE, COLOR_mode = DEFAULT_COLOR_MODE;
uint8 OnTrackmode = DEFAULT_ON_TRACK_MODE;


void UI()
{
	if(!datapage)
	{
		UI_Content();//��ʾID��Ӧ�ĺ���
		UI_ContentKey();//����ID
	}
	else
	{
		UI_Datapage();  //����ҳ
        UI_DatapageKey();   //���ݰ�������
	}
	if(pagenum!=4 && pagenum!=7)
	{
		DisplayCursor();//�����ʾ
	}
}


void UI_Content(void)
{
	//tft180_clear();//����ʾ��һ���˵�ʱ����Ҫ����
	// ����pagenumȥ��ʾ�˵�
	switch(pagenum)
    {
        case 0://��ʾ���˵�
		{
			DisplayMain();
		}break;
		case 1://��ʾ��װ���ع����
		{
			DisplayMt9v03Para();
		}break;
		case 2://��ʾ�ٶȲ���
		{
			DisplaySpeedPara();
		}break;
		case 3://��ʾIMU����
		{
			DisplayIMUPara();
		}break;
		case 4://��ʾԪ�ز���
		{
			DisplayElementPara();
		}break;
		case 5://Start
		{
			DisplayStartPara();
		}break;
		case 6://OpenArt brightness
		    DisplayOpenArtPara();
			break;
		case 7:
		    DisplayImage();
			break;
		case 8:
		    DisplaySensitiveparameters();
			break;
		default:
		{
			DisplayMain();	
		}break;
	}
}

//���հ���ֵ��������arrow
void UI_ContentKey()
{
	//uint8 key = Get_Key_flag();//��ȡ����ֵ
    if(KEY_SHORT_PRESS == key_get_state(KEY_1))
	{//���� up
        ips200_show_string(0,arrow*16," ");  //��Ҫ����ǰ�еĹ�����أ���Ȼ��arrow����֮�󣬳���������ʾ���
		arrow--;//�����ƶ���arrow��С
	}
    if(KEY_SHORT_PRESS == key_get_state(KEY_2))
	{//���� down
        ips200_show_string(0,arrow*16," "); 
		arrow++;
	}//��upͬ��
	
/**********�����key1,key2�����������ƶ�����***********/
/**********�����key3,key4������������Ӧ�ĸ�/�Ӳ˵���***********/

    if(KEY_SHORT_PRESS == key_get_state(KEY_3))
	{//������ҳ��  enter
        ips200_clear();//��Ҫ����ǰ��ʾ������������ʾ�ӽ���    
		pagenum = pagenumdown();//�����ӽ��� 
		arrow = 0;//�ӵ�0�п�ʼ
	}
    if(KEY_SHORT_PRESS == key_get_state(KEY_4))
	{//������һ��ҳ�� back
        ips200_clear();    
		pagenum = pagenumup();
		arrow = 0;
	}
	if(KEY_LONG_PRESS == key_get_state(KEY_1))//���Ĳ���
	{
//		buzzer_set_delay(100);      //������
		system_delay_ms(500);
		ips200_clear();
		   
		datapage = 1;    
		mul = 1;
	}
	
	//��arrow�޷���OLED��ֻ����ʾ8�У�������Ҫ���� 0 <= arrow <8
	if(arrow < 0)
	{
		arrow = 7;//��˼�ǣ�������up�������һֱ�����ƶ������˶�����һ�м�arrow=0��
				  //�ٰ���upʱ�����ֱ���������һ�У�arrow = 7
		if(pagenum==6)
		{
			which_Page --;
			which_Page = CLAMP(which_Page, 0, actualPage-1);
			ips200_clear();
		}
	}
		
	else if(arrow >7)
	{
		arrow = 0;
		if(pagenum==6)
		{
			which_Page ++;
			which_Page = CLAMP(which_Page, 0, actualPage-1);
			ips200_clear();
		}
	}
		
	key_clear_all_state();
}

//������һ���˵�
uint8 pagenumup(void)
{
	switch(pagenum)
	{
		case 0://��һ���˵�
			return 0;
		case 1://��һ���˵�
			return 0;
		case 2://��һ���˵�
			return 0;
		case 3://��һ���˵�
			return 0;
		case 4://��һ���˵�
			return 0;
		case 5://��һ���˵�
			return 0;
		case 6://��һ���˵�
			return 0;
		case 7:
			return 0;
		case 8:
			return 0;
		default:
			return 0;
	}
}

//������һ���˵�
//��Ҫ�жϵ�ǰID��֪������һҳ����Щʲô����
//����Ҫ�жϹ�������У�֪��Ҫ������һ����������
uint8 pagenumdown(void)
{
    uint8 ret = 0; // Ĭ�Ϸ���ֵ
    
    switch(pagenum)
    {
        case 0: // ��������
            switch(arrow) // �ж��ڵڼ���
            {
                case 0: ret = 1; break; // ��ӦMt9v03Para
                case 1: ret = 2; break; // ��ӦSpeedPara
                case 2: ret = 3; break; // ��ӦIMUPara
                case 3: ret = 4; break; // ��ӦElemnetPara
                case 4: ret = 5; break; // ��ӦStartPara
                case 5: ret = 6; break; // ��ӦopenartPara
                case 6: ret = 7; break; // ��ӦImage
				case 7: ret = 8; break; // ��ӦSensitive_Parameters
                default: ret = 0; break;
            }
            break;            
        case 1: // ��Mt9v03Para����
            ret = 1;
            break;           
        case 2: // ��SpeedPara����
            ret = 2;
            break;          
        case 3: // IMUPara����
            ret = 3;
            break;            
        case 4: // ElemnetPara����
            switch(arrow)
            {
                // ��־λ�����ܸ���
                default: ret = 4; break; // �޸�Ϊ���ص�ǰ����ID
            }
            break;            
        default:
            ret = 0;
            break;
    }    
    return ret;
}

//��ʾ����Ĳ���
void UI_Datapage()
{
	uint8 x = 20,y = 160;
//	ips200_clear();
	switch(pagenum)
	{
		case 0:break;//��IDΪ0�Ľ���û����Ҫ�޸ĵĲ���
		case 1://��Ӧexposure time
		{
			switch(arrow)
			{
				case 0://kp
				{
					ips200_show_string(x,0,"exposure time is:"); 
					ips200_show_int(y,0,mt9v03X_light,4);
					ips200_show_string(x,1*16,"white value:"); 
					ips200_show_int(y,1*16,find_white_point(mt9v03x_image),3);
				}break;
				case 2://kp
				{	
					ips200_show_string(x,0,"exposure time is:"); 
					ips200_show_int(y,0,mt9v03X_light,4);
					ips200_show_string(x,1*16,"white value:"); 
					ips200_show_int(y,1*16,find_white_point(mt9v03x_image),3);
					ips200_show_string(x,2*16,"OBJECT_ARTexposure:"); 
					ips200_show_int(y+10,2*16,OBJECT_light,4);
				}break;
				case 3://kp
				{
					ips200_show_string(x,0,"exposure time is:"); 
					ips200_show_int(y,0,mt9v03X_light,4);
					ips200_show_string(x,1*16,"white value:"); 
					ips200_show_int(y,1*16,find_white_point(mt9v03x_image),3);
					ips200_show_string(x,3*16,"COLOR_ARTexposure:"); 
					ips200_show_int(y+10,3*16,COLOR_light,4);
				}break;
				case 4://kp
				{
					ips200_show_string(x,0,"exposure time is:"); 
					ips200_show_int(y,0,mt9v03X_light,4);
					ips200_show_string(x,1*16,"white value:"); 
					ips200_show_int(y,1*16,find_white_point(mt9v03x_image),3);
					ips200_show_string(x,4*16,"OBJECT_ARTmode:"); 
					ips200_show_int(y+10,4*16,OBJECT_mode,4);
				}break;
				case 5://kp
				{
					ips200_show_string(x,0,"exposure time is:"); 
					ips200_show_int(y,0,mt9v03X_light,4);
					ips200_show_string(x,1*16,"white value:"); 
					ips200_show_int(y,1*16,find_white_point(mt9v03x_image),3);
					ips200_show_string(x,5*16,"COLOR_ARTmode:"); 
					ips200_show_int(y+10,5*16,COLOR_mode,4);
				}break;
				case 6://kp
				{
					ips200_show_string(x,0,"exposure time is:"); 
					ips200_show_int(y,0,mt9v03X_light,4);
					ips200_show_string(x,1*16,"white value:"); 
					ips200_show_int(y,1*16,find_white_point(mt9v03x_image),3);
					ips200_show_string(x,6*16,"OnTrackmode:"); 
					ips200_show_int(y+10,6*16,OnTrackmode,4);
				}break;
				
				default:break;
			}
			ips200_show_float(y,7*16,mul,4,3);//��ʾ����
		}break;
		
		case 2://��ӦSpeedPara
		{
			switch(arrow)
			{
				case 0://kp
				{
					ips200_show_string(x,2*16,"Max_box"); 
					ips200_show_int(y,2*16,get_max_box(),3);				
				}break;
				case 1://kp
				{
					ips200_show_string(x,2*16,"Max_box"); 
					ips200_show_int(y,2*16,get_max_box(),3);
				}break;
				case 2://kp
				{
					ips200_show_string(x,2*16,"Max_box"); 
					ips200_show_int(y,2*16,get_max_box(),3);
				}break;
				default:break;
			}
			ips200_show_float(y,7*16,mul,4,3);//��ʾ����
		}break;		
		case 3:break;//��ӦIMUPara,��֧���޸�
		case 4:break;//��ӦElementPara,��֧���޸�
		case 5:
		{
			if(SPEED_ENABLE)
			{
				ips200_show_string(x,0,"Start Sucess!"); 
			}
			else
			{
				ips200_show_string(x,0,"Long press Key3 to start!"); 
			}
			break;
		}
		case 6:
		    DisplayOpenArtPara();
			break;
		case 7:
		{
			DisplayImage();
		}
		default:break;
	}
}

//���Ĳ���
void UI_DatapageKey()
{
	//uint8 key = Get_Key_flag();
	if(KEY_SHORT_PRESS == key_get_state(KEY_1))//   +
	{
		switch(pagenum)//�ж�����һҳ
		{
			case 1://mt9v03x_set_exposure_time
			{
				switch(arrow)//��һ��
				{
					case 0:
						mt9v03X_light += mul;
						mt9v03x_set_exposure_time(mt9v03X_light);
						break;
					case 2:
						OBJECT_light += mul;
						uart_send_int16_with_delimiter(OBJECT_UART,OBJECT_light);
						break;
					case 3:
						COLOR_light += mul;
						uart_send_int16_with_delimiter(COLOR_UART,COLOR_light);
						break;
					case 4:
						OBJECT_mode ++;OBJECT_mode = CLAMP(OBJECT_mode, 0, 1);
						uart_send_int16_with_delimiter(OBJECT_UART,OBJECT_VALUE(OBJECT_mode));
						break;
					case 5:
						COLOR_mode ++;COLOR_mode = CLAMP(COLOR_mode, 0, 1);
						uart_send_int16_with_delimiter(COLOR_UART,COLOR_VALUE(COLOR_mode));
						break;
					case 6:
						OnTrackmode ++;OnTrackmode = CLAMP(OnTrackmode, 0, 1);
						uart_send_int16_with_delimiter(COLOR_UART,TRACK_VALUE(OnTrackmode));
						break;
					//����������ӣ�������һ��ʾ��
					default:break;
				}
			}break;
			case 2://SpeedPara
			{
				switch(arrow)
				{
					case 2:{uint8_t max_box = get_max_box();
						   max_box += mul;
						   set_max_box(max_box);
					       break;}
					default:break;
				}
			}break;
			// .....����ľ��Լ�����������
			default:break;
		}
	}
	if(KEY_SHORT_PRESS == key_get_state(KEY_2))// -
	{
		switch(pagenum)//�ж�����һҳ
		{
			case 1://Mt9v03Para
			{
				switch(arrow)//��һ��
				{
					case 0:
						if(mt9v03X_light>=mul)
							mt9v03X_light -= mul;
						mt9v03x_set_exposure_time(mt9v03X_light);
						break;
					case 2:
						if(OBJECT_light>=mul)
						OBJECT_light -= mul;
						uart_send_int16_with_delimiter(OBJECT_UART,OBJECT_light);
						break;
					case 3:
						if(COLOR_light>=mul)
						COLOR_light -= mul;
						uart_send_int16_with_delimiter(COLOR_UART,COLOR_light);
						break;
					case 4:
						OBJECT_mode --;OBJECT_mode = CLAMP(OBJECT_mode, 0, 1);
						uart_send_int16_with_delimiter(OBJECT_UART,OBJECT_VALUE(OBJECT_mode));
						break;
					case 5:
						COLOR_mode --;COLOR_mode = CLAMP(COLOR_mode, 0, 1);
						uart_send_int16_with_delimiter(COLOR_UART,COLOR_VALUE(COLOR_mode));
						break;
					case 6:
						OnTrackmode --;OnTrackmode = CLAMP(OnTrackmode, 0, 1);
						uart_send_int16_with_delimiter(COLOR_UART,TRACK_VALUE(OnTrackmode));
					//����������ӣ�������һ��ʾ��
					default:break;
				}
			}break;
			case 2://RightMotorPara
			{
				switch(arrow)
				{
					case 2:{uint8_t max_box = get_max_box();
						   if(max_box>=mul)
						   max_box -= mul;
						   set_max_box(max_box);
					       break;}
					default:break;
				}	
			}break;
			// .....����ľ��Լ�����������
			default:break;
		}
	}
	if(KEY_SHORT_PRESS == key_get_state(KEY_3))//���ı���
    {
        mul = mul / 10;
    }
    if(KEY_SHORT_PRESS == key_get_state(KEY_4))// ȷ�ϸ��Ĳ���
    {
        mul = mul * 10;
    }
	if(KEY_LONG_PRESS == key_get_state(KEY_1))
	{
//		buzzer_set_delay(100);         //������
		system_delay_ms(500);
		ips200_clear();
		datapage = 0;
	}
	if(KEY_LONG_PRESS == key_get_state(KEY_3))
	{
//		buzzer_set_delay(100);         //������
		system_delay_ms(1500);
		SPEED_ENABLE = true;
		ips200_clear();
		datapage = 0;
	}
	
	key_clear_all_state();
}

void DisplayCursor()//��ʾ���
{
	ips200_show_string(0,arrow*16,">");//�ڲ���ǰ����ʾ>
}

void DisplayMain(void)
{
	ips200_show_string(7,0*16,"1.exposure time is:"); 		
    ips200_show_string(7,1*16,"2.SpeedPara");   	   	 
	ips200_show_string(7,2*16,"3.IMUPara");  	 
	ips200_show_string(7,3*16,"4.ElementPara");//Ԫ�ز���         	
	ips200_show_string(7,4*16,"5.Start");//���� 
	ips200_show_string(7,5*16,"6.OpenArtPara");//ֹͣ
	ips200_show_string(7,6*16,"7.Show_Image");//ͼ����ʾ
	ips200_show_string(7,7*16,"8.Sensitive_Parameters");//ͼ����ʾ
}
void DisplayMt9v03Para(void)
{
	ips200_show_string(7,0*16,"exposure time");       	ips200_show_int(160,0,mt9v03X_light,4);
	ips200_show_string(7,1*16,"white value");			ips200_show_int(160,1*16,find_white_point(mt9v03x_image),3);
	
	ips200_show_string(7,2*16,"OBJECT_ARTexposure"); ips200_show_int(160,2*16,OBJECT_light,4);
	ips200_show_string(7,3*16,"COLOR_ARTexposure");  ips200_show_int(160,3*16,COLOR_light,4);
	
	ips200_show_string(7,4*16,"OBJECT_ARTmode"); ips200_show_int(160,4*16,OBJECT_mode,4);	
	ips200_show_string(7,5*16,"COLOR_ARTmode");  ips200_show_int(160,5*16,COLOR_mode,4);
	
	ips200_show_string(7,6*16,"OnTrackmode");  ips200_show_int(160,6*16,OnTrackmode,4);
}
void DisplaySpeedPara(void)
{
	ips200_show_string(7,2*16,"Max_box");      	ips200_show_int(160,2*16,get_max_box(),3);
}
void DisplayIMUPara(void)
{
//	ips200_show_string(7,0*16,"pitch_angle");       	ips200_show_float(160,0,Gyro_Angle.Ydata,4,3);
	ips200_show_string(7,1*16,"yaw_angle");      		ips200_show_float(160,1*16,Gyro_Angle.Zdata,4,3);
//    ips200_show_string(7,2*16,"roll_angle");      		ips200_show_float(160,2*16,Gyro_Angle.Xdata,4,3);
}
void DisplayElementPara(void)
{
	//��������
	ips200_show_string(0,16*0,"right_down_guai[0]");
	ips200_show_int(0, 16*1, right_down_guai[0], 3);
	ips200_show_string(0,16*2,"Island_State");
	ips200_show_int(0, 16*3, Island_State, 3);
	ips200_show_string(0,16*4,"Boundry_Start_Right");
	ips200_show_int(200, 16*4, Boundry_Start_Right, 3);
	ips200_show_string(0,16*5,"Boundry_Start_Left");
	ips200_show_int(200, 16*5, Boundry_Start_Left, 3);
	ips200_show_string(0,16*6,"Both_Lost_Time");
	ips200_show_int(0, 16*7, Both_Lost_Time, 3);
	ips200_show_string(0,16*8,"monotonicity_change_left_flag");
	ips200_show_int(0, 16*9, monotonicity_change_left_flag, 3);
	ips200_show_string(0,16*10,"continuity_change_left_flag");
	ips200_show_int(0, 16*11, continuity_change_left_flag, 3);
	ips200_show_string(0,16*12,"continuity_change_right_flag");
	ips200_show_int(0, 16*13, continuity_change_right_flag, 3);
	ips200_show_string(0,16*14,"lose_point_num_R");
	ips200_show_uint(0, 16*15, lose_point_num_R, 3);
	ips200_show_string(0,16*16,"lose_point_num_L");
	ips200_show_uint(0, 16*17, lose_point_num_L, 3);
	ips200_show_string(0,16*18,"longest");
	ips200_show_int(0, 16*19, longest, 3);
}

void DisplayStartPara(void)
{
	ips200_show_string(7,0*16,"is_start_enable");		ips200_show_int(160,0*16,SPEED_ENABLE,1);
}


const uint8 ART_show[15][8][16] =
{
	{
		{0x01,0x00,0x01,0x00,0x01,0x00,0x3F,0xF8,0x21,0x08,0x21,0x08,0x21,0x08,0x3F,0xF8},
		{0x21,0x08,0x21,0x08,0x21,0x08,0x3F,0xF8,0x21,0x0A,0x01,0x02,0x01,0x02,0x00,0xFE},/*"��",0*/

		{0x10,0x20,0x10,0x20,0x3C,0x20,0x20,0x20,0x40,0x3E,0xBC,0x20,0x10,0x20,0x10,0x20},
		{0xFD,0xFC,0x11,0x04,0x11,0x04,0x11,0x04,0x15,0x04,0x19,0x04,0x11,0xFC,0x01,0x04},/*"��",1*/
	},
	{
		{0x00,0x00,0x7F,0xFC,0x10,0x10,0x10,0x10,0x1F,0xF0,0x10,0x10,0x10,0x10,0x1F,0xF0},
		{0x10,0x10,0x10,0x10,0x10,0x3E,0x17,0xD0,0xF8,0x10,0x00,0x10,0x00,0x10,0x00,0x10},/*"��",0*/

		{0x10,0x00,0x11,0xF0,0x11,0x10,0x11,0x10,0xFD,0x10,0x11,0x10,0x31,0x10,0x39,0x10},
		{0x55,0x10,0x55,0x10,0x91,0x10,0x11,0x12,0x11,0x12,0x12,0x12,0x12,0x0E,0x14,0x00},/*"��",1*/
	},
	{
		{0x00,0x00,0x00,0x00,0xFF,0xFE,0x04,0x00,0x04,0x00,0x04,0x00,0x07,0xF0,0x04,0x10},
		{0x04,0x10,0x08,0x10,0x08,0x10,0x10,0x10,0x10,0x10,0x20,0x10,0x40,0xA0,0x80,0x40},/*"��",0*/

		{0x00,0x00,0x3F,0xF8,0x21,0x08,0x21,0x08,0x21,0x08,0x3F,0xF8,0x21,0x08,0x21,0x08},
		{0x21,0x08,0x3F,0xF8,0x21,0x08,0x21,0x08,0x21,0x08,0x41,0x08,0x41,0x28,0x80,0x10},/*"��",1*/

		{0x01,0x00,0x01,0x00,0x7F,0xFC,0x01,0x00,0x01,0x00,0x3F,0xF8,0x01,0x00,0x01,0x00},
		{0xFF,0xFE,0x05,0x00,0x08,0x88,0x18,0x50,0x28,0x20,0xC9,0x18,0x0A,0x06,0x0C,0x00},/*"��",2*/
	},
	{
		{0x20,0x10,0x20,0x10,0x3B,0x7C,0x21,0x14,0x41,0xFE,0x7A,0x14,0xA2,0x7C,0x27,0x10},
		{0xF9,0x7C,0x25,0x10,0x25,0xFE,0x22,0x10,0x2A,0x10,0x35,0x00,0x28,0xFE,0x00,0x00},/*"��",0*/

		{0x02,0x00,0x04,0x00,0x1F,0xF0,0x11,0x10,0x10,0x90,0xFF,0xFE,0x10,0x10,0x12,0x10},
		{0x21,0x50,0x40,0x20,0x3F,0xF8,0x24,0x48,0x24,0x48,0x24,0x48,0xFF,0xFE,0x00,0x00},/*"��",1*/
	},
	{
		{0x00,0x00,0x3F,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFE,0x01,0x00},
		{0x01,0x00,0x11,0x10,0x11,0x08,0x21,0x04,0x41,0x02,0x81,0x02,0x05,0x00,0x02,0x00},/*"ʾ",0*/

		{0x00,0x20,0x20,0x20,0x10,0x20,0x13,0xFE,0x82,0x22,0x42,0x24,0x4A,0x20,0x0B,0xFC},
		{0x12,0x84,0x12,0x88,0xE2,0x48,0x22,0x50,0x22,0x20,0x24,0x50,0x24,0x88,0x09,0x06},/*"��",1*/

		{0x00,0x00,0x3E,0x7C,0x22,0x44,0x22,0x44,0x3E,0x7C,0x01,0x20,0x01,0x10,0xFF,0xFE},
		{0x02,0x80,0x0C,0x60,0x30,0x18,0xC0,0x06,0x3E,0x7C,0x22,0x44,0x22,0x44,0x3E,0x7C},/*"��",2*/
	},
	{
		{0x00,0x10,0x00,0xF8,0x3F,0x00,0x01,0x00,0x01,0x00,0x3F,0xF8,0x01,0x00,0x01,0x00},
		{0x01,0x00,0xFF,0xFE,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x05,0x00,0x02,0x00},/*"��",0*/

		{0x10,0x00,0x11,0xF0,0x11,0x10,0x11,0x10,0xFD,0x10,0x11,0x10,0x31,0x10,0x39,0x10},
		{0x55,0x10,0x55,0x10,0x91,0x10,0x11,0x12,0x11,0x12,0x12,0x12,0x12,0x0E,0x14,0x00},/*"��",1*/
	},
	{
		{0x10,0x88,0x10,0x88,0x3C,0x88,0x20,0x88,0x43,0xFE,0xBC,0x88,0x10,0x88,0x10,0x88},
		{0xFC,0x88,0x10,0xF8,0x10,0x88,0x10,0x88,0x14,0x88,0x18,0x88,0x10,0xF8,0x00,0x88},/*"ǯ",0*/

		{0x00,0x00,0x7F,0xF8,0x00,0x10,0x00,0x20,0x00,0x40,0x01,0x80,0x01,0x00,0xFF,0xFE},
		{0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x05,0x00,0x02,0x00},/*"��",1*/
	},
	{

		{0x00,0x00,0x1F,0xF0,0x10,0x10,0x10,0x10,0x1F,0xF0,0x10,0x10,0x10,0x10,0x1F,0xF0},
		{0x04,0x40,0x44,0x44,0x24,0x44,0x14,0x48,0x14,0x50,0x04,0x40,0xFF,0xFE,0x00,0x00},/*"��",0*/

		{0x00,0x00,0x3F,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFE,0x01,0x00},
		{0x01,0x00,0x11,0x10,0x11,0x08,0x21,0x04,0x41,0x02,0x81,0x02,0x05,0x00,0x02,0x00},/*"ʾ",1*/

		{0x00,0x00,0x3F,0xF8,0x20,0x08,0x20,0x08,0x3F,0xF8,0x24,0x10,0x22,0x20,0x2F,0xF8},
		{0x22,0x20,0x22,0x20,0x3F,0xFC,0x22,0x20,0x42,0x20,0x44,0x20,0x84,0x20,0x08,0x20},/*"��",2*/
	},
	{

		{0x10,0x00,0x11,0xFC,0x11,0x24,0x7D,0xFC,0x55,0x24,0x55,0xFC,0x54,0x40,0x54,0x88},
		{0x7D,0xF0,0x50,0x20,0x10,0x44,0x17,0xFE,0x1C,0x22,0xE5,0x24,0x42,0x22,0x00,0x60},/*"��",0*/

		{0x08,0x10,0x08,0x10,0x10,0x20,0x10,0x20,0x22,0x44,0x42,0x84,0x7C,0xF8,0x04,0x08},
		{0x08,0x10,0x10,0x20,0x20,0x40,0x7E,0xFC,0x00,0x00,0x00,0x00,0xFF,0xFE,0x00,0x00},/*"˿",1*/

		{0x00,0x00,0x00,0x00,0x3F,0xF8,0x02,0x08,0x02,0x08,0x02,0x08,0x02,0x08,0x02,0x08},
		{0x02,0x08,0x04,0x08,0x04,0x08,0x08,0x08,0x08,0x08,0x10,0x08,0x20,0x50,0x40,0x20},/*"��",2*/	
	},
	{
		{0x04,0x00,0x38,0x78,0x20,0x08,0x3C,0x78,0x20,0x08,0x20,0x08,0x3F,0xF8,0x00,0x00},
		{0x29,0x48,0x25,0x28,0x21,0x08,0x29,0x48,0x25,0x2A,0x21,0x0A,0x29,0x46,0x31,0x82},/*"��",0*/

		{0x10,0x00,0x11,0xFC,0x10,0x00,0x10,0x00,0xFC,0x00,0x13,0xFE,0x30,0x20,0x38,0x20},
		{0x55,0x28,0x55,0x24,0x92,0x24,0x12,0x22,0x14,0x22,0x10,0x20,0x10,0xA0,0x10,0x40},/*"��",1*/

	},
	{
		{0x01,0x00,0x01,0x00,0x01,0x00,0x3F,0xF8,0x21,0x08,0x21,0x08,0x21,0x08,0x3F,0xF8},
		{0x21,0x08,0x21,0x08,0x21,0x08,0x3F,0xF8,0x21,0x0A,0x01,0x02,0x01,0x02,0x00,0xFE},/*"��",0*/

		{0x10,0x80,0x10,0x80,0x10,0xF8,0x15,0x08,0x5B,0x10,0x54,0xA0,0x50,0x40,0x90,0xA0},
		{0x11,0x18,0x12,0x06,0x15,0xF8,0x29,0x08,0x25,0x08,0x45,0x08,0x41,0xF8,0x81,0x08},/*"��",1*/

		{0x10,0x20,0x10,0x20,0x3D,0x20,0x21,0x20,0x41,0xFC,0xBD,0x20,0x12,0x20,0x10,0x20},
		{0xFD,0xFE,0x10,0x20,0x10,0x50,0x10,0x50,0x14,0x88,0x18,0x88,0x11,0x04,0x02,0x02},/*"��",2*/
	},
	{
		{0x10,0x00,0x10,0x00,0x13,0xFE,0x10,0x20,0xFC,0x20,0x10,0x20,0x10,0x20,0x14,0x20},
		{0x18,0x20,0x30,0x20,0xD0,0x20,0x10,0x20,0x10,0x20,0x10,0x20,0x50,0xA0,0x20,0x40},/*"��",0*/

		{0x00,0x00,0x06,0x00,0x78,0xFC,0x40,0x84,0x40,0x84,0x40,0x84,0x7E,0x84,0x40,0x84},
		{0x40,0x84,0x40,0x84,0x40,0x84,0x4E,0xA8,0x70,0x90,0x00,0x80,0x00,0x80,0x00,0x80},/*"ӡ",1*/

		{0x10,0x00,0x11,0xF0,0x11,0x10,0x11,0x10,0xFD,0x10,0x11,0x10,0x31,0x10,0x39,0x10},
		{0x55,0x10,0x55,0x10,0x91,0x10,0x11,0x12,0x11,0x12,0x12,0x12,0x12,0x0E,0x14,0x00},/*"��",2*/
	},
	{
		{0x01,0x00,0x11,0x10,0x09,0x20,0x3F,0xF8,0x02,0x00,0x02,0x00,0x7F,0xFC,0x08,0x20},
		{0x10,0x10,0x2F,0xE8,0xC8,0x26,0x08,0x20,0x08,0xA8,0x08,0x48,0x08,0x08,0x07,0xF8},/*"��",0*/

		{0x00,0x00,0x1F,0xF8,0x10,0x08,0x10,0x08,0x10,0x08,0x10,0x08,0x1F,0xF8,0x10,0x80},
		{0x10,0x80,0x10,0x80,0x10,0x40,0x10,0x40,0x20,0x20,0x20,0x10,0x40,0x08,0x80,0x06},/*"��",1*/
	},
	{
		{0x02,0x00,0x01,0x00,0x3F,0xF8,0x00,0x00,0x08,0x20,0x04,0x40,0xFF,0xFE,0x00,0x00},
		{0x1F,0xF0,0x10,0x10,0x10,0x10,0x1F,0xF0,0x10,0x10,0x10,0x10,0x1F,0xF0,0x10,0x10},/*"��",0*/

		{0x00,0x40,0x00,0x40,0x78,0x80,0x4B,0xFC,0x4A,0x04,0x4A,0x04,0x4A,0xF4,0x4A,0x94},
		{0x4A,0x94,0x4A,0x94,0x7A,0x94,0x4A,0xF4,0x02,0x04,0x02,0x04,0x02,0x14,0x02,0x08},/*"��",1*/
	},
	{
		{0x10,0x08,0x10,0x1C,0x11,0xE0,0x11,0x00,0xFD,0x00,0x11,0xFC,0x11,0x44,0x15,0x44},
		{0x19,0x44,0x31,0x28,0xD1,0x28,0x11,0x10,0x11,0x10,0x12,0x28,0x52,0x44,0x24,0x82},/*"��",0*/

		{0x00,0x10,0x00,0xF8,0x3F,0x00,0x01,0x00,0x01,0x00,0x3F,0xF8,0x01,0x00,0x01,0x00},
		{0x01,0x00,0xFF,0xFE,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x05,0x00,0x02,0x00},/*"��",1*/
	}
};


//��ʾARTʶ�𵽵����
void DisplayOpenArtPara(void)
{
	int8 num_max = tail-1-head;   //ʵ�ʸ���Ϊnum_max+1
	
    static bool first_call = false;
    if (!first_call) 
    {
        // ����ʵ��ÿҳ������ע�⣺�����pageSize�Ѿ���"����-1"��
        uint8 actualPageSize = pageSize + 1;  // ����pageSize=15 �� ʵ��ÿҳ16��Ԫ��
        
        if(num_max > pageSize)
        {
            // ������ҳ��������ȡ����
            actualPage = (num_max + 1 + actualPageSize - 1) / actualPageSize;
            
            // ��ʼ��Page_num���飨�洢"ʵ��Ԫ����-1"��
            for(int i = 0; i < actualPage; i++) {
                int startIdx = i * actualPageSize;
                int endIdx = startIdx + actualPageSize;
                
                // ���㵱ǰҳ��ʵ��Ԫ����
                uint8 actualElements = (endIdx <= (num_max + 1)) 
                                      ? actualPageSize 
                                      : (num_max + 1 - startIdx);
                
                // �洢"ʵ��Ԫ����-1"
                Page_num[i] = actualElements - 1;
            }
        }
        else
        {
            // ����һҳʱ��ʵ��Ԫ����Ϊnum_max+1���洢Ϊ(num_max+1)-1 = num_max
            Page_num[0] = num_max;
            actualPage = 1;
        }
        
        first_call = true;
    }
    
	
	for(int i=0; i <= Page_num[which_Page]; i++)
	{
		if(ART[i+ which_Page*(pageSize+1)]>=100)
		{
			ips200_show_int(20,i*16+2*(i+1),i+1 + which_Page*(pageSize+1),2);
			ips200_show_chinese(100, i*16+2*(i+1), 16, ART_show[ART[i+ which_Page*(pageSize+1)]-100][0], 4, RGB565_BLACK);
			ips200_draw_line(10, (i+1)*16+2*(i+1), 180, (i+1)*16+2*(i+1), RGB565_BLACK);
		}
		else
		{
			ips200_show_int(20,i*16+2*(i+1),i+1 + which_Page*(pageSize+1),2);    
			ips200_show_int(100, i*16+2*(i+1),ART[i+ which_Page*(pageSize+1)],3);
			ips200_draw_line(10, (i+1)*16+2*(i+1), 180, (i+1)*16+2*(i+1), RGB565_BLACK);
		}
	}
	if(Page_num>=0)
	{
		ips200_draw_line(10, 0, 180, 0, RGB565_BLACK);
		ips200_draw_line(10, 0, 10, (Page_num[which_Page]+1)*16+2*(Page_num[which_Page]+1), RGB565_BLACK);
		ips200_draw_line(45, 0, 45, (Page_num[which_Page]+1)*16+2*(Page_num[which_Page]+1), RGB565_BLACK);
		ips200_draw_line(180, 0, 180, (Page_num[which_Page]+1)*16+2*(Page_num[which_Page]+1), RGB565_BLACK);
	}	
}

void DisplayImage(void)
{
	if(mt9v03x_finish_flag)
	{
		mt9v03x_finish_flag = 0;
		if(Image_Mode == 7)
		{
			ips200_show_gray_image(0, 0, (const uint8 *)mt9v03x_image, MT9V03X_W, MT9V03X_H, 188, 120, Threshold);
		}
		else
		{
			ips200_show_gray_image(0, 0, (const uint8 *)mt9v03x_image, MT9V03X_W, MT9V03X_H, 188, 120, 0);	
			show_boder_line();
		}

	}
}

void DisplaySensitiveparameters(void)
{
	ips200_show_string(7,16*0,"Symmetry_Ratio");
	ips200_show_float(7, 16*1, Symmetry_Ratio, 3, 3);
	ips200_show_string(7,16*2,"Now_Angle_X");
	ips200_show_int(7, 16*3, Now_Angle_X, 3);
	ips200_show_string(7,16*4,"Now_Position_Y");
	ips200_show_uint(7, 16*5, Now_Position_Y, 3);
	ips200_show_string(7,16*6,"find_times");
	ips200_show_uint(7, 16*7, get_find_times(), 3);
	ips200_show_string(7,16*8,"test_is_Continuous");
	ips200_show_int(7, 16*9, is_Continuous(), 3);
}
