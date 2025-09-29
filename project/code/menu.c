#include "menu.h"


uint8 openart_bright_scale = 20;
//下面的是测试参数，在实际工程中，有其他函数判断得到
bool magnet_state = false;
uint8 datapage = 0;//0：显示ID对应的函数，1：修改arrow对应的参数
float mul = 1;

//用来显示各级菜单
uint8 pagenum = 0;//显示page的数目
int8 arrow = 0;//定义光标所在的行，初始化在第一行

uint8 which_Page = 0;
uint8 actualPage = 0;
int8 Page_num[totalPages];


uint8 OBJECT_mode = DEFAULT_OBJECT_MODE, COLOR_mode = DEFAULT_COLOR_MODE;
uint8 OnTrackmode = DEFAULT_ON_TRACK_MODE;


void UI()
{
	if(!datapage)
	{
		UI_Content();//显示ID对应的函数
		UI_ContentKey();//更新ID
	}
	else
	{
		UI_Datapage();  //数据页
        UI_DatapageKey();   //数据按键处理
	}
	if(pagenum!=4 && pagenum!=7)
	{
		DisplayCursor();//光标显示
	}
}


void UI_Content(void)
{
	//tft180_clear();//在显示新一级菜单时，需要清屏
	// 根据pagenum去显示菜单
	switch(pagenum)
    {
        case 0://显示主菜单
		{
			DisplayMain();
		}break;
		case 1://显示总装风曝光参数
		{
			DisplayMt9v03Para();
		}break;
		case 2://显示速度参数
		{
			DisplaySpeedPara();
		}break;
		case 3://显示IMU参数
		{
			DisplayIMUPara();
		}break;
		case 4://显示元素参数
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

//接收按键值，并更新arrow
void UI_ContentKey()
{
	//uint8 key = Get_Key_flag();//获取按键值
    if(KEY_SHORT_PRESS == key_get_state(KEY_1))
	{//向上 up
        ips200_show_string(0,arrow*16," ");  //需要将当前行的光标隐藏，不然当arrow更新之后，出现两行显示光标
		arrow--;//向上移动，arrow减小
	}
    if(KEY_SHORT_PRESS == key_get_state(KEY_2))
	{//向下 down
        ips200_show_string(0,arrow*16," "); 
		arrow++;
	}//和up同理
	
/**********上面的key1,key2是用来上下移动光标的***********/
/**********下面的key3,key4是用来进入相应的父/子菜单的***********/

    if(KEY_SHORT_PRESS == key_get_state(KEY_3))
	{//进入子页面  enter
        ips200_clear();//需要将当前显示清屏，用于显示子界面    
		pagenum = pagenumdown();//进入子界面 
		arrow = 0;//从第0行开始
	}
    if(KEY_SHORT_PRESS == key_get_state(KEY_4))
	{//返回上一个页面 back
        ips200_clear();    
		pagenum = pagenumup();
		arrow = 0;
	}
	if(KEY_LONG_PRESS == key_get_state(KEY_1))//更改参数
	{
//		buzzer_set_delay(100);      //蜂鸣器
		system_delay_ms(500);
		ips200_clear();
		   
		datapage = 1;    
		mul = 1;
	}
	
	//对arrow限幅，OLED屏只能显示8行，所以需要限制 0 <= arrow <8
	if(arrow < 0)
	{
		arrow = 7;//意思是：当按下up键，光标一直向上移动，当运动到第一行即arrow=0，
				  //再按下up时，光标直接跳到最后一行，arrow = 7
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

//返回上一级菜单
uint8 pagenumup(void)
{
	switch(pagenum)
	{
		case 0://在一级菜单
			return 0;
		case 1://在一级菜单
			return 0;
		case 2://在一级菜单
			return 0;
		case 3://在一级菜单
			return 0;
		case 4://在一级菜单
			return 0;
		case 5://在一级菜单
			return 0;
		case 6://在一级菜单
			return 0;
		case 7:
			return 0;
		case 8:
			return 0;
		default:
			return 0;
	}
}

//进入下一级菜单
//需要判断当前ID：知道在那一页，有些什么参数
//还需要判断光标所在行：知道要进入哪一个变量里面
uint8 pagenumdown(void)
{
    uint8 ret = 0; // 默认返回值
    
    switch(pagenum)
    {
        case 0: // 在主界面
            switch(arrow) // 判断在第几行
            {
                case 0: ret = 1; break; // 对应Mt9v03Para
                case 1: ret = 2; break; // 对应SpeedPara
                case 2: ret = 3; break; // 对应IMUPara
                case 3: ret = 4; break; // 对应ElemnetPara
                case 4: ret = 5; break; // 对应StartPara
                case 5: ret = 6; break; // 对应openartPara
                case 6: ret = 7; break; // 对应Image
				case 7: ret = 8; break; // 对应Sensitive_Parameters
                default: ret = 0; break;
            }
            break;            
        case 1: // 在Mt9v03Para界面
            ret = 1;
            break;           
        case 2: // 在SpeedPara界面
            ret = 2;
            break;          
        case 3: // IMUPara界面
            ret = 3;
            break;            
        case 4: // ElemnetPara界面
            switch(arrow)
            {
                // 标志位，不能更改
                default: ret = 4; break; // 修改为返回当前界面ID
            }
            break;            
        default:
            ret = 0;
            break;
    }    
    return ret;
}

//显示具体的参数
void UI_Datapage()
{
	uint8 x = 20,y = 160;
//	ips200_clear();
	switch(pagenum)
	{
		case 0:break;//在ID为0的界面没有需要修改的参数
		case 1://对应exposure time
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
			ips200_show_float(y,7*16,mul,4,3);//显示倍率
		}break;
		
		case 2://对应SpeedPara
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
			ips200_show_float(y,7*16,mul,4,3);//显示倍率
		}break;		
		case 3:break;//对应IMUPara,不支持修改
		case 4:break;//对应ElementPara,不支持修改
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

//更改参数
void UI_DatapageKey()
{
	//uint8 key = Get_Key_flag();
	if(KEY_SHORT_PRESS == key_get_state(KEY_1))//   +
	{
		switch(pagenum)//判断在哪一页
		{
			case 1://mt9v03x_set_exposure_time
			{
				switch(arrow)//哪一行
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
					//其他自行添加，这里做一个示范
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
			// .....后面的就自己根据情况添加
			default:break;
		}
	}
	if(KEY_SHORT_PRESS == key_get_state(KEY_2))// -
	{
		switch(pagenum)//判断在哪一页
		{
			case 1://Mt9v03Para
			{
				switch(arrow)//哪一行
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
					//其他自行添加，这里做一个示范
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
			// .....后面的就自己根据情况添加
			default:break;
		}
	}
	if(KEY_SHORT_PRESS == key_get_state(KEY_3))//更改倍率
    {
        mul = mul / 10;
    }
    if(KEY_SHORT_PRESS == key_get_state(KEY_4))// 确认更改参数
    {
        mul = mul * 10;
    }
	if(KEY_LONG_PRESS == key_get_state(KEY_1))
	{
//		buzzer_set_delay(100);         //蜂鸣器
		system_delay_ms(500);
		ips200_clear();
		datapage = 0;
	}
	if(KEY_LONG_PRESS == key_get_state(KEY_3))
	{
//		buzzer_set_delay(100);         //蜂鸣器
		system_delay_ms(1500);
		SPEED_ENABLE = true;
		ips200_clear();
		datapage = 0;
	}
	
	key_clear_all_state();
}

void DisplayCursor()//显示光标
{
	ips200_show_string(0,arrow*16,">");//在参数前面显示>
}

void DisplayMain(void)
{
	ips200_show_string(7,0*16,"1.exposure time is:"); 		
    ips200_show_string(7,1*16,"2.SpeedPara");   	   	 
	ips200_show_string(7,2*16,"3.IMUPara");  	 
	ips200_show_string(7,3*16,"4.ElementPara");//元素参数         	
	ips200_show_string(7,4*16,"5.Start");//启动 
	ips200_show_string(7,5*16,"6.OpenArtPara");//停止
	ips200_show_string(7,6*16,"7.Show_Image");//图像显示
	ips200_show_string(7,7*16,"8.Sensitive_Parameters");//图像显示
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
	//环岛参数
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
		{0x21,0x08,0x21,0x08,0x21,0x08,0x3F,0xF8,0x21,0x0A,0x01,0x02,0x01,0x02,0x00,0xFE},/*"电",0*/

		{0x10,0x20,0x10,0x20,0x3C,0x20,0x20,0x20,0x40,0x3E,0xBC,0x20,0x10,0x20,0x10,0x20},
		{0xFD,0xFC,0x11,0x04,0x11,0x04,0x11,0x04,0x15,0x04,0x19,0x04,0x11,0xFC,0x01,0x04},/*"钻",1*/
	},
	{
		{0x00,0x00,0x7F,0xFC,0x10,0x10,0x10,0x10,0x1F,0xF0,0x10,0x10,0x10,0x10,0x1F,0xF0},
		{0x10,0x10,0x10,0x10,0x10,0x3E,0x17,0xD0,0xF8,0x10,0x00,0x10,0x00,0x10,0x00,0x10},/*"耳",0*/

		{0x10,0x00,0x11,0xF0,0x11,0x10,0x11,0x10,0xFD,0x10,0x11,0x10,0x31,0x10,0x39,0x10},
		{0x55,0x10,0x55,0x10,0x91,0x10,0x11,0x12,0x11,0x12,0x12,0x12,0x12,0x0E,0x14,0x00},/*"机",1*/
	},
	{
		{0x00,0x00,0x00,0x00,0xFF,0xFE,0x04,0x00,0x04,0x00,0x04,0x00,0x07,0xF0,0x04,0x10},
		{0x04,0x10,0x08,0x10,0x08,0x10,0x10,0x10,0x10,0x10,0x20,0x10,0x40,0xA0,0x80,0x40},/*"万",0*/

		{0x00,0x00,0x3F,0xF8,0x21,0x08,0x21,0x08,0x21,0x08,0x3F,0xF8,0x21,0x08,0x21,0x08},
		{0x21,0x08,0x3F,0xF8,0x21,0x08,0x21,0x08,0x21,0x08,0x41,0x08,0x41,0x28,0x80,0x10},/*"用",1*/

		{0x01,0x00,0x01,0x00,0x7F,0xFC,0x01,0x00,0x01,0x00,0x3F,0xF8,0x01,0x00,0x01,0x00},
		{0xFF,0xFE,0x05,0x00,0x08,0x88,0x18,0x50,0x28,0x20,0xC9,0x18,0x0A,0x06,0x0C,0x00},/*"表",2*/
	},
	{
		{0x20,0x10,0x20,0x10,0x3B,0x7C,0x21,0x14,0x41,0xFE,0x7A,0x14,0xA2,0x7C,0x27,0x10},
		{0xF9,0x7C,0x25,0x10,0x25,0xFE,0x22,0x10,0x2A,0x10,0x35,0x00,0x28,0xFE,0x00,0x00},/*"键",0*/

		{0x02,0x00,0x04,0x00,0x1F,0xF0,0x11,0x10,0x10,0x90,0xFF,0xFE,0x10,0x10,0x12,0x10},
		{0x21,0x50,0x40,0x20,0x3F,0xF8,0x24,0x48,0x24,0x48,0x24,0x48,0xFF,0xFE,0x00,0x00},/*"盘",1*/
	},
	{
		{0x00,0x00,0x3F,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFE,0x01,0x00},
		{0x01,0x00,0x11,0x10,0x11,0x08,0x21,0x04,0x41,0x02,0x81,0x02,0x05,0x00,0x02,0x00},/*"示",0*/

		{0x00,0x20,0x20,0x20,0x10,0x20,0x13,0xFE,0x82,0x22,0x42,0x24,0x4A,0x20,0x0B,0xFC},
		{0x12,0x84,0x12,0x88,0xE2,0x48,0x22,0x50,0x22,0x20,0x24,0x50,0x24,0x88,0x09,0x06},/*"波",1*/

		{0x00,0x00,0x3E,0x7C,0x22,0x44,0x22,0x44,0x3E,0x7C,0x01,0x20,0x01,0x10,0xFF,0xFE},
		{0x02,0x80,0x0C,0x60,0x30,0x18,0xC0,0x06,0x3E,0x7C,0x22,0x44,0x22,0x44,0x3E,0x7C},/*"器",2*/
	},
	{
		{0x00,0x10,0x00,0xF8,0x3F,0x00,0x01,0x00,0x01,0x00,0x3F,0xF8,0x01,0x00,0x01,0x00},
		{0x01,0x00,0xFF,0xFE,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x05,0x00,0x02,0x00},/*"手",0*/

		{0x10,0x00,0x11,0xF0,0x11,0x10,0x11,0x10,0xFD,0x10,0x11,0x10,0x31,0x10,0x39,0x10},
		{0x55,0x10,0x55,0x10,0x91,0x10,0x11,0x12,0x11,0x12,0x12,0x12,0x12,0x0E,0x14,0x00},/*"机",1*/
	},
	{
		{0x10,0x88,0x10,0x88,0x3C,0x88,0x20,0x88,0x43,0xFE,0xBC,0x88,0x10,0x88,0x10,0x88},
		{0xFC,0x88,0x10,0xF8,0x10,0x88,0x10,0x88,0x14,0x88,0x18,0x88,0x10,0xF8,0x00,0x88},/*"钳",0*/

		{0x00,0x00,0x7F,0xF8,0x00,0x10,0x00,0x20,0x00,0x40,0x01,0x80,0x01,0x00,0xFF,0xFE},
		{0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x05,0x00,0x02,0x00},/*"子",1*/
	},
	{

		{0x00,0x00,0x1F,0xF0,0x10,0x10,0x10,0x10,0x1F,0xF0,0x10,0x10,0x10,0x10,0x1F,0xF0},
		{0x04,0x40,0x44,0x44,0x24,0x44,0x14,0x48,0x14,0x50,0x04,0x40,0xFF,0xFE,0x00,0x00},/*"显",0*/

		{0x00,0x00,0x3F,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFE,0x01,0x00},
		{0x01,0x00,0x11,0x10,0x11,0x08,0x21,0x04,0x41,0x02,0x81,0x02,0x05,0x00,0x02,0x00},/*"示",1*/

		{0x00,0x00,0x3F,0xF8,0x20,0x08,0x20,0x08,0x3F,0xF8,0x24,0x10,0x22,0x20,0x2F,0xF8},
		{0x22,0x20,0x22,0x20,0x3F,0xFC,0x22,0x20,0x42,0x20,0x44,0x20,0x84,0x20,0x08,0x20},/*"屏",2*/
	},
	{

		{0x10,0x00,0x11,0xFC,0x11,0x24,0x7D,0xFC,0x55,0x24,0x55,0xFC,0x54,0x40,0x54,0x88},
		{0x7D,0xF0,0x50,0x20,0x10,0x44,0x17,0xFE,0x1C,0x22,0xE5,0x24,0x42,0x22,0x00,0x60},/*"螺",0*/

		{0x08,0x10,0x08,0x10,0x10,0x20,0x10,0x20,0x22,0x44,0x42,0x84,0x7C,0xF8,0x04,0x08},
		{0x08,0x10,0x10,0x20,0x20,0x40,0x7E,0xFC,0x00,0x00,0x00,0x00,0xFF,0xFE,0x00,0x00},/*"丝",1*/

		{0x00,0x00,0x00,0x00,0x3F,0xF8,0x02,0x08,0x02,0x08,0x02,0x08,0x02,0x08,0x02,0x08},
		{0x02,0x08,0x04,0x08,0x04,0x08,0x08,0x08,0x08,0x08,0x10,0x08,0x20,0x50,0x40,0x20},/*"刀",2*/	
	},
	{
		{0x04,0x00,0x38,0x78,0x20,0x08,0x3C,0x78,0x20,0x08,0x20,0x08,0x3F,0xF8,0x00,0x00},
		{0x29,0x48,0x25,0x28,0x21,0x08,0x29,0x48,0x25,0x2A,0x21,0x0A,0x29,0x46,0x31,0x82},/*"鼠",0*/

		{0x10,0x00,0x11,0xFC,0x10,0x00,0x10,0x00,0xFC,0x00,0x13,0xFE,0x30,0x20,0x38,0x20},
		{0x55,0x28,0x55,0x24,0x92,0x24,0x12,0x22,0x14,0x22,0x10,0x20,0x10,0xA0,0x10,0x40},/*"标",1*/

	},
	{
		{0x01,0x00,0x01,0x00,0x01,0x00,0x3F,0xF8,0x21,0x08,0x21,0x08,0x21,0x08,0x3F,0xF8},
		{0x21,0x08,0x21,0x08,0x21,0x08,0x3F,0xF8,0x21,0x0A,0x01,0x02,0x01,0x02,0x00,0xFE},/*"电",0*/

		{0x10,0x80,0x10,0x80,0x10,0xF8,0x15,0x08,0x5B,0x10,0x54,0xA0,0x50,0x40,0x90,0xA0},
		{0x11,0x18,0x12,0x06,0x15,0xF8,0x29,0x08,0x25,0x08,0x45,0x08,0x41,0xF8,0x81,0x08},/*"烙",1*/

		{0x10,0x20,0x10,0x20,0x3D,0x20,0x21,0x20,0x41,0xFC,0xBD,0x20,0x12,0x20,0x10,0x20},
		{0xFD,0xFE,0x10,0x20,0x10,0x50,0x10,0x50,0x14,0x88,0x18,0x88,0x11,0x04,0x02,0x02},/*"铁",2*/
	},
	{
		{0x10,0x00,0x10,0x00,0x13,0xFE,0x10,0x20,0xFC,0x20,0x10,0x20,0x10,0x20,0x14,0x20},
		{0x18,0x20,0x30,0x20,0xD0,0x20,0x10,0x20,0x10,0x20,0x10,0x20,0x50,0xA0,0x20,0x40},/*"打",0*/

		{0x00,0x00,0x06,0x00,0x78,0xFC,0x40,0x84,0x40,0x84,0x40,0x84,0x7E,0x84,0x40,0x84},
		{0x40,0x84,0x40,0x84,0x40,0x84,0x4E,0xA8,0x70,0x90,0x00,0x80,0x00,0x80,0x00,0x80},/*"印",1*/

		{0x10,0x00,0x11,0xF0,0x11,0x10,0x11,0x10,0xFD,0x10,0x11,0x10,0x31,0x10,0x39,0x10},
		{0x55,0x10,0x55,0x10,0x91,0x10,0x11,0x12,0x11,0x12,0x12,0x12,0x12,0x0E,0x14,0x00},/*"机",2*/
	},
	{
		{0x01,0x00,0x11,0x10,0x09,0x20,0x3F,0xF8,0x02,0x00,0x02,0x00,0x7F,0xFC,0x08,0x20},
		{0x10,0x10,0x2F,0xE8,0xC8,0x26,0x08,0x20,0x08,0xA8,0x08,0x48,0x08,0x08,0x07,0xF8},/*"卷",0*/

		{0x00,0x00,0x1F,0xF8,0x10,0x08,0x10,0x08,0x10,0x08,0x10,0x08,0x1F,0xF8,0x10,0x80},
		{0x10,0x80,0x10,0x80,0x10,0x40,0x10,0x40,0x20,0x20,0x20,0x10,0x40,0x08,0x80,0x06},/*"尺",1*/
	},
	{
		{0x02,0x00,0x01,0x00,0x3F,0xF8,0x00,0x00,0x08,0x20,0x04,0x40,0xFF,0xFE,0x00,0x00},
		{0x1F,0xF0,0x10,0x10,0x10,0x10,0x1F,0xF0,0x10,0x10,0x10,0x10,0x1F,0xF0,0x10,0x10},/*"音",0*/

		{0x00,0x40,0x00,0x40,0x78,0x80,0x4B,0xFC,0x4A,0x04,0x4A,0x04,0x4A,0xF4,0x4A,0x94},
		{0x4A,0x94,0x4A,0x94,0x7A,0x94,0x4A,0xF4,0x02,0x04,0x02,0x04,0x02,0x14,0x02,0x08},/*"响",1*/
	},
	{
		{0x10,0x08,0x10,0x1C,0x11,0xE0,0x11,0x00,0xFD,0x00,0x11,0xFC,0x11,0x44,0x15,0x44},
		{0x19,0x44,0x31,0x28,0xD1,0x28,0x11,0x10,0x11,0x10,0x12,0x28,0x52,0x44,0x24,0x82},/*"扳",0*/

		{0x00,0x10,0x00,0xF8,0x3F,0x00,0x01,0x00,0x01,0x00,0x3F,0xF8,0x01,0x00,0x01,0x00},
		{0x01,0x00,0xFF,0xFE,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x05,0x00,0x02,0x00},/*"手",1*/
	}
};


//显示ART识别到的类别
void DisplayOpenArtPara(void)
{
	int8 num_max = tail-1-head;   //实际个数为num_max+1
	
    static bool first_call = false;
    if (!first_call) 
    {
        // 计算实际每页容量（注意：这里的pageSize已经是"容量-1"）
        uint8 actualPageSize = pageSize + 1;  // 例如pageSize=15 → 实际每页16个元素
        
        if(num_max > pageSize)
        {
            // 计算总页数（向上取整）
            actualPage = (num_max + 1 + actualPageSize - 1) / actualPageSize;
            
            // 初始化Page_num数组（存储"实际元素数-1"）
            for(int i = 0; i < actualPage; i++) {
                int startIdx = i * actualPageSize;
                int endIdx = startIdx + actualPageSize;
                
                // 计算当前页的实际元素数
                uint8 actualElements = (endIdx <= (num_max + 1)) 
                                      ? actualPageSize 
                                      : (num_max + 1 - startIdx);
                
                // 存储"实际元素数-1"
                Page_num[i] = actualElements - 1;
            }
        }
        else
        {
            // 不足一页时，实际元素数为num_max+1，存储为(num_max+1)-1 = num_max
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
