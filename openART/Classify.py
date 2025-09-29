# 导入必要的模块
import seekfree
import sensor, time
import tf
from machine import UART
from pyb import LED

# 初始化串口，波特率为115200，TX是B12，RX是B13
uart = UART(2, baudrate=115200)

sensor.reset()                       # 重置摄像头
sensor.set_pixformat(sensor.RGB565)  # 设置像素格式为RGB565
sensor.set_framesize(sensor.QVGA)    # 设置摄像头分辨率为QVGA
sensor.set_brightness(700)           # 设置摄像头亮度
sensor.skip_frames(time = 200)       # 跳过前200帧，让摄像头稳定
clock = time.clock()                 # 初始化时钟模块，用于测量帧率
white = LED(4)
lcd = seekfree.IPS200(3)
lcd.full()  # 将背景颜色显示到整个屏幕

# ---- 初始化摄像头亮度 ----
brightness = 700

# ---- 模式切换标志位。0为百分类，1为十位加个位 ----
Number_identify_mode_flag = 1

# ---- 串口接收数据的状态机变量 ----
rx_state = 0  # 0: 等待包头, 1: 接收数据, 2: 等待包尾
data_high = 0
data_low = 0
data_index = 0
received_data = 0
finish_flag = 0

# ---- 设置模型 ----
# 目标检测
face_detect = 'yolo3_iou_kuang_99.11.tflite'
net_detect = tf.load(face_detect)

# 分类
net_path_pic_num = "C_obj_num_wen_one.tflite"                               # 模型路径
labels_pic_num = [line.rstrip() for line in open("/sd/labels_wen.txt")]     # 加载标签文件
net1 = tf.load(net_path_pic_num, load_to_fb=True)                           # 加载模型到帧缓冲区

net_path_shiwei = "shi_me_one.tflite"                                       # 模型路径
labels_shiwei = [line.rstrip() for line in open("/sd/labels_shiwei.txt")]   # 加载标签文件
net2 = tf.load(net_path_shiwei, load_to_fb=True)                            # 加载模型到帧缓冲区

net_path_gewei = "ge_me_one.tflite"                                         # 模型路径
labels_gewei = [line.rstrip() for line in open("/sd/labels_gewei.txt")]     # 加载标签文件
net3 = tf.load(net_path_gewei, load_to_fb=True)                             # 加载模型到帧缓冲区

net_path_number = "bai_me_one.tflite"                                       # 模型路径
labels_number = [line.rstrip() for line in open("/sd/labels_bai.txt")]      # 加载标签文件
net4 = tf.load(net_path_number, load_to_fb=True)                            # 加载模型到帧缓冲区

uart_data= '0'  # 初始化串口接收到的数据
flag = 0        # 初始化串口接收到的数据


# ---- 判断前三次类别是否相同 ----
card_class_record = []       # 前三次记录的卡片类别
def class_record(card_class):
    global card_class_record
    if len(card_class_record) >= 3:
        card_class_record.pop(0)             # 移除最旧的记录
    card_class_record.append(card_class)     # 添加最新的记录
    return len(set(card_class_record)) == 1  # 判断前三次类别是否相同


# ---- 目标检测 ----
def face_detect(img):
    img2=(80,25,160,160)
    img1 = img.copy(0.75, 1)
    for obj in tf.detect(net_detect,img1):
        x1,y1,x2,y2,label,scores = obj

        if(scores >= 0.96):
            # 转换坐标到像素值
            w = int((x2 - x1) * img.width())
            h = int((y2 - y1) * img.height())
            x1 = int(x1 * img.width())
            y1 = int(y1 * img.height())

            # 绘制原始检测框
            #img.draw_rectangle((x1, y1, w, h), thickness=2)
            # 计算中心点
            x_center = x1 + w // 2
            y_center = y1 + h // 2

            # 计算正方形区域坐标
            square_size = 160
            half_size = square_size // 2

            # 计算并约束坐标范围
            square_x = max(0, min(x_center - half_size, img.width() - square_size))
            square_y = max(0, min(y_center - half_size, img.height() - square_size))

            # 截取正方形区域
            img2 = (square_x, square_y, square_size, square_size)
            #cropped_img = img.copy(roi=cropped_roi)

            # 在原始图像上绘制正方形区域（绿色框）
            img.draw_rectangle(img2, color=(0, 255, 0), thickness=1)
    return img2

# ---- 分类 ----
pic_num_find_card = 0
pic_num_sorted_list = []
def pic_num_classify(img):
    global pic_num_find_card,pic_num_sorted_list
    for obj in tf.classify(net1, img, min_scale=1.0, scale_mul=0.5, x_overlap=0.0, y_overlap=0.0):# 分类
        pic_num_sorted_list = sorted(zip(labels_pic_num, obj.output()), key = lambda x: x[1], reverse = True)# 排序分类结果
        pic_num_find_card += 1                        # 检测到卡片，计数加1


shiwei_find_card = 0
shiwei_sorted_list = []
def shiwei_classify(img):
    global shiwei_find_card,shiwei_sorted_list
    for obj in tf.classify(net2, img, min_scale=1.0, scale_mul=0.5, x_overlap=0.0, y_overlap=0.0):# 分类
        shiwei_sorted_list = sorted(zip(labels_shiwei, obj.output()), key = lambda x: x[1], reverse = True)# 排序分类结果
        shiwei_find_card += 1                        # 检测到卡片，计数加1

gewei_find_card = 0
gewei_sorted_list = []
def gewei_classify(img):
    global gewei_find_card,gewei_sorted_list
    for obj in tf.classify(net3, img, min_scale=1.0, scale_mul=0.5, x_overlap=0.0, y_overlap=0.0):# 分类
        gewei_sorted_list = sorted(zip(labels_gewei, obj.output()), key = lambda x: x[1], reverse = True)# 排序分类结果
        gewei_find_card += 1                        # 检测到卡片，计数加1

number_find_card = 0
number_sorted_list = []
def number_classify(img):
    global number_find_card,number_sorted_list
    for obj in tf.classify(net4, img, min_scale=1.0, scale_mul=0.5, x_overlap=0.0, y_overlap=0.0):# 分类
        number_sorted_list = sorted(zip(labels_number, obj.output()), key = lambda x: x[1], reverse = True)# 排序分类结果
        number_find_card += 1                        # 检测到卡片，计数加1

# ---- 计算十位加个位 ----
def shu_zhi_chu_li(img):
    global shiwei_sorted_list, gewei_sorted_list  # 添加全局声明
    shiwei = 0
    gewei = 0
    shiwei_classify(img)
    shiwei = int(shiwei_sorted_list[0][0])
    gewei_classify(img)
    gewei = int(gewei_sorted_list[0][0])
    return shiwei*10 + gewei

# ---- 接收单片机发来的数据并处理 ----
def UART_Receive_Number(data):
    global rx_state,data_high,data_low,data_index,finish_flag,received_data,uart_data
    # 状态机处理（与中断逻辑完全一致）
    if rx_state == 0:
        # 等待包头（例如'X'）
        if data == ord('X'):
            rx_state = 1
            data_index = 0
            data_high = 0
            data_low = 0
            finish_flag = 0
        elif data == ord('a'):
            uart_data = 'a'
        elif data == ord('b'):
            uart_data = 'b'
    elif rx_state == 1:
        # 接收数据（高8位→低8位）
        if data_index == 0:
            data_high = data
            data_index = 1
        elif data_index == 1:
            data_low = data
            data_index = 2
            rx_state = 2  # 切换到等待包尾
        else:
            # 数据异常，重置
            rx_state = 0
            data_index = 0
    elif rx_state == 2:
        # 等待包尾（例如'Y'）
        if data == ord('Y'):
            # 解析16位数据
            received_data = (data_high << 8) | data_low
            # 处理负数补码
            if received_data & 0x8000:
                received_data -= 0x10000
            finish_flag = 1  # 标记完成
        # 重置状态
        rx_state = 0

while(True):
    #查询是否有数据到达
    if uart.any() > 0:  # 有数据待读取
        # 读取1字节数据
        data = uart.read(1)[0]  # 读取1字节（转为整数）
        UART_Receive_Number(data)
    #处理解析完成的数据
    if finish_flag:
        #print(f"接收完成: {received_data}")
        received_number = received_data
        if received_number >= 0: # 改变亮度
            brightness = received_number
            #LED_blue.toggle()
            sensor.set_brightness(brightness)   # 更新摄像头亮度

        if received_number == -1000: # 如果接收到-1000，识别数字为十位加个位
            Number_identify_mode_flag = 1
            #print(Number_identify_mode_flag)
        elif received_number == -2000: #如果接收到-2000，识别数字为百分类
            Number_identify_mode_flag = 0
            #print(Number_identify_mode_flag)
        finish_flag = 0  # 清除标志位

    #white.on()            #是否开补光灯
    #每一轮重新初始化
    number_find_card = 0
    pic_num_find_card = 0
    shiwei_find_card = 0
    gewei_find_card = 0
    shuzhi = 0

    img = sensor.snapshot()    #捕获一帧图像
    #img.draw_rectangle((60, 20, 168, 168),thickness=1)
    cropped_roi = face_detect(img)
    #img.draw_rectangle(cropped_roi, thickness=1)
    #lcd.show_image(img, 320, 240, zoom=0)
    img1=img.copy(1,1,roi=cropped_roi)
    pic_num_classify(img1)         # 调用分类函数
    if pic_num_sorted_list:       # 如果有分类结果
        if((pic_num_find_card>0) and class_record(pic_num_sorted_list[0][0])): # 检测到卡片且前几次类别一致
            pic_num_label = int(pic_num_sorted_list[0][0])
            if pic_num_label == 99:
                # 识别数字为十位加个位
                if Number_identify_mode_flag == 1:
                    shuzhi = shu_zhi_chu_li(img1)
                    if uart_data == 'a':
                        if(flag == 0):
                            #print("%d,%f" %(int(shiwei_sorted_list[0][0]),shiwei_sorted_list[0][1]))
                            #print("%d,%f" %(int(gewei_sorted_list[0][0]),gewei_sorted_list[0][1]))
                            #print (shuzhi)
                            #img.draw_string(10,10, "%d,%d"%(result,area_threshold),color=(0,0,0),scale = 3)
                            uart.write("Y%dZ" %(shuzhi))
                            flag = 1
                # 识别数字为百分类
                if Number_identify_mode_flag == 0:
                    number_classify(img1)
                    if uart_data == 'a':
                        if(flag == 0):
                            #print("%d,%f" %(int(number_sorted_list[0][0]),number_sorted_list[0][1]))
                            uart.write("Y%dZ" %(int(number_sorted_list[0][0])))
                            flag = 1
            if not(pic_num_label == 99):
                if uart_data == 'a':
                    if(flag == 0):
                        #print("%d,%f" %(int(pic_num_sorted_list[0][0]),pic_num_sorted_list[0][1]))
                        uart.write("Y%dZ" %(int(pic_num_sorted_list[0][0])))
                        flag = 1

    if uart_data == 'b':
        flag = 0
