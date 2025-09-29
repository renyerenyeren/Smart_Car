import sensor, time, gc, math
from machine import UART
from pyb import LED

# ---- 定义红色阈值，用于颜色跟踪 ----
red_threshold = (0, 100, 9, 127, 8, 127)

# ---- 定义白色阈值和黑色阈值，用于判断箱子是否在赛道内 ----
white_threshold = (55, 100, -128, 127, -128, 127)
black_threshold = (0, 55, -128, 127, -128, 127)

# ---- 初始化摄像头 ----
sensor.reset()                       # 初始化摄像头传感器
sensor.set_pixformat(sensor.RGB565)  # 设置像素格式为RGB565
sensor.set_framesize(sensor.QVGA)    # 设置帧大小为QQVGA, 以提高速度。
sensor.set_brightness(700)           # 设置摄像头亮度
sensor.skip_frames(10)               # 跳过前10帧以使新设置生效
sensor.set_auto_whitebal(False)      # 关闭自动白平衡
#sensor.set_auto_exposure(False)     # 关闭自动曝光

clock = time.clock()  # 用于跟踪FPS
LED_blue = LED(3)     # 蓝色指示灯
white = LED(4)        # 摄像头自带补光灯

# ---- 打开UART串口通信，波特率为115200 ----
uart = UART(2, 115200)
data = '0'

# ---- 初始摄像头亮度 ----
brightness = 700

# ---- 设置静态面积阈值标志位 ----
#标志位为5，在小车速度为1.5m/s时在最佳位置看到箱子
#标志位为5，在小车速度为1.7m/s时在最佳位置看到箱子
Static_area_threshold_flag = 5

# ---- 要不要判断箱子是否在赛道内标志位 ----
#标志位为0，不判断箱子是否在赛道内
#标志位为1，判断箱子是否在赛道内
Determine_red_box_flag = 0

# ---- 全局变量：赛道检测标志位 ----
Racetrack_checked_flag = False  # 标记是否已检测过赛道状态
On_Racetrack_result = False     # 赛道检测结果
last_blob_cx = 0                # 上一次检测到的红色色块中心x坐标
last_blob_cy = 0                # 上一次检测到的红色色块中心y坐标
POSITION_CHANGE_THRESHOLD = 80  # 中心坐标变化阈值（像素）

# ---- 均值滤波配置 ----
MEAN_FILTER_SIZE = 5  # 移动平均窗口大小（建议3-10）
x_history = []  # X坐标历史队列
y_history = []  # Y坐标历史队列

# ---- 宽高比过滤 ----
aspect_ratio_range = [0.75, 1.35]

# ---- 边界过滤 ----
X_Max = 270
X_Min = 50

# 串口接收数据的状态机变量
rx_state = 0  # 0: 等待包头, 1: 接收数据, 2: 等待包尾
data_high = 0
data_low = 0
data_index = 0
received_data = 0
finish_flag = 0

# ---- 滤波函数 ----
#均值滤波函数
def moving_average_filter(new_x, new_y):
    global x_history, y_history

    # 更新历史数据
    x_history.append(new_x)
    y_history.append(new_y)

    # 保持队列长度
    if len(x_history) > MEAN_FILTER_SIZE:
        x_history.pop(0)
        y_history.pop(0)

    # 计算平均值（至少需要3个数据点）
    if len(x_history) >= 3:
        x_avg = int(sum(x_history) / len(x_history))
        y_avg = int(sum(y_history) / len(y_history))
        return x_avg, y_avg
    else:
        return new_x, new_y
#中值滤波函数
def median_filter(new_x, new_y):
    global x_history, y_history

    # 更新历史数据
    x_history.append(new_x)
    y_history.append(new_y)

    # 保持队列长度
    if len(x_history) > MEAN_FILTER_SIZE:
        x_history.pop(0)
        y_history.pop(0)

    # 计算中值
    if len(x_history) >= 3:
        x_sorted = sorted(x_history)
        y_sorted = sorted(y_history)
        x_median = x_sorted[len(x_sorted) // 2]
        y_median = y_sorted[len(y_sorted) // 2]
        return x_median, y_median
    else:
        return new_x, new_y


# ---- 找到面积大于某一阈值的blob ----
def find_max_Fliter(blobs):
    max_size = 0
    size_threshold = 900
    for blob in blobs:
        if blob.pixels() > max_size:
            max_blob = blob
            max_size = blob.pixels()
    if max_size > size_threshold:
        return max_blob
    else:
        return None

# ---- 判断找到的色块是否为矩形 ----
def is_rectangle(blob):
    # 获取blob的外接矩形
    x, y, w, h = blob.rect()

    # 计算宽高比
    aspect_ratio = w / h
    # 判断宽高比是否在合理范围内
    if not (aspect_ratio_range[0] <= aspect_ratio <= aspect_ratio_range[1]):
        return False

    # 获取blob的角点
    corners = blob.corners()
    # 判断角点数量是否为4
    if len(corners) != 4:
        return False

    # 计算blob的面积与外接矩形面积的比值
    blob_area = blob.pixels()
    rect_area = w * h
    area_ratio = blob_area / rect_area
    # 判断面积比是否在合理范围内（矩形的面积应该接近外接矩形面积）
    if area_ratio < 0.45:  # 根据实际情况需调整这个阈值
        return False

    return True

# ---- 计算色块中心坐标和(160,20)的连线与x=160的夹角 ----
def Offset_angle(x, y, blob):
    # boundary = blob.y() - blob.h()
    boundary = 20
    numerator = abs(x - 160)
    denominator = y - boundary
    if denominator == 0:
        # 处理分母为零的情况（例如返回 π/2 或 -π/2）
        return 0, 0
    angle_rad = math.atan2(numerator, denominator)
    angle_deg = int(math.degrees(angle_rad))
    if (x - 160) < 0:
        angle_deg = -angle_deg

    if angle_deg >= 45:
        return 45
    elif angle_deg <= -45:
        return -45
    else:
        return angle_deg

# ---- 车速1.5m/s时的静态面积阈值 ----
# 将整个图像分成6个区域并判断
def Static_area_threshold_1_5(blob):
    Y_threshold = 70
    Static_area = 0
    area_threshold = 0
    if (blob.cx() < 100 and blob.cy() >= Y_threshold):
        Static_area = 1
        area_threshold = 2000
    if (blob.cx() >= 100 and blob.cx() <= 220):
        Static_area = 2
        area_threshold = 900
    if (blob.cx() > 220 and blob.cy() >= Y_threshold):
        Static_area = 3
        area_threshold = 2000
    return area_threshold, Static_area

# ---- 车速1.7m/s时的静态面积阈值 ----
# 将整个图像分成6个区域并判断
def Static_area_threshold_1_7(blob):
    Y_threshold = 60
    Static_area = 0
    area_threshold = 0
    if (blob.cx() < 100 and blob.cy() >= Y_threshold):
        Static_area = 1
        area_threshold = 1600
    if (blob.cx() >= 100 and blob.cx() <= 220):
        Static_area = 2
        area_threshold = 900
    if (blob.cx() > 220 and blob.cy() >= Y_threshold):
        Static_area = 3
        area_threshold = 1600
    return area_threshold, Static_area

# ---- 判断红色箱子是否在赛道上 ----
def On_Racetrack(img,max_blob):
    global white_threshold,Racetrack_checked_flag,On_Racetrack_result
    #框出红色方块下方
    Xx=max_blob.x()
    Yy=max_blob.y()+max_blob.h()
    Ww=max_blob.w()
    Hh=10
    White_Threshold_Ratio=0.25 #白色区域阈值比例
    Black_Threshold_Ratio=0.7 #白色区域阈值比例

    img.draw_rectangle((Xx,Yy,Ww,Hh), color = (0, 0, 255))

    # 如果已经检测过，直接返回之前的结果
    if Racetrack_checked_flag:
        return On_Racetrack_result

    img1 = img.copy(roi=(Xx,Yy,Ww,Hh)) # 拷贝矩形框内的图像

    print(Ww*Hh)
    #判断红色方块下方是否为白色（是否在白色赛道上）
    blob_find_white = img1.find_blobs([white_threshold],x_stride=2, y_stride=2)
    for white in blob_find_white:
        #print(white.pixels())
        #print(0.25*Ww*Hh)
        if(white.pixels()>int(White_Threshold_Ratio*Ww*Hh)):
            Racetrack_checked_flag = True
            On_Racetrack_result = True
            return True
    #判断红色方块下方是否为黑色（是否在白色赛道上）
    blob_find_black = img1.find_blobs([black_threshold],x_stride=2, y_stride=2)
    for black in blob_find_black:
        #print(black.pixels())
        #print(int(0.7 * Ww * Hh))
        if(black.pixels()>int(Black_Threshold_Ratio*Ww*Hh)):
            Racetrack_checked_flag = True
            On_Racetrack_result = False
            return False

# ---- 判断检测到的红色箱子中心坐标位置变化是否较为连续 ----
def check_position_change(current_cx, current_cy):
    global last_blob_cx, last_blob_cy

    # 如果是第一次检测，直接更新位置
    if last_blob_cx == 0 and last_blob_cy == 0:
        last_blob_cx = current_cx
        last_blob_cy = current_cy
        return False

    # 计算位置变化
    delta_x = abs(current_cx - last_blob_cx)
    #delta_y = abs(current_cy - last_blob_cy)

    # 如果位置变化超过阈值
    if delta_x > POSITION_CHANGE_THRESHOLD :
        # 更新位置
        last_blob_cx = current_cx
        last_blob_cy = current_cy
        return True

    return False

# ---- 接收单片机发来的数据并处理 ----
def UART_Receive_Number(data):
    global rx_state,data_high,data_low,data_index,finish_flag,received_data
    # 状态机处理（与中断逻辑完全一致）
    if rx_state == 0:
        # 等待包头（'@'）
        if data == ord('@'):
            rx_state = 1
            data_index = 0
            data_high = 0
            data_low = 0
            finish_flag = 0
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
        # 等待包尾（'#'）
        if data == ord('#'):
            # 解析16位数据
            received_data = (data_high << 8) | data_low
            # 处理负数补码
            if received_data & 0x8000:
                received_data -= 0x10000
            finish_flag = 1  # 标记完成
        # 重置状态
        rx_state = 0



while True:
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

        if received_number == -3000: # 如果接收到-3000，切换为近距离找箱子
            Static_area_threshold_flag = 5
            #print(Static_area_threshold_flag)
            #LED_blue.toggle()
        elif received_number ==-4000: # 如果接收到-4000，切换为略远距离找箱子
            Static_area_threshold_flag = 7
            #print(Static_area_threshold_flag)
            #LED_blue.toggle()

        if received_number ==-6000: # 如果接收到-6000，不判断箱子是否在赛道内
            Determine_red_box_flag = 0
            #print(Determine_red_box_flag)
            #LED_blue.toggle()
        elif received_number ==-5000: # 如果接收到-5000，判断箱子是否在赛道内
            Determine_red_box_flag = 1
            #print(Determine_red_box_flag)
            #LED_blue.toggle()
        finish_flag = 0  # 清除标志位


    img = sensor.snapshot()  # 拍摄一张照片并返回图像

    blobs = img.find_blobs([red_threshold])
    if blobs:
        max_blob = find_max_Fliter(blobs)  # 找到最大的blob

        if max_blob:
            img.draw_rectangle(max_blob.rect())  # 在图像上绘制最大blob的矩形框
            img.draw_cross(max_blob.cx(), max_blob.cy())  # 在图像上绘制最大blob的中心点
            blob_area = max_blob.pixels()

            # img.draw_string(50,50, "%d,%d"%(result,area_threshold),color=(0,0,0),scale = 3)
            if(Static_area_threshold_flag == 5):
                Y_threshold = 70
                area_threshold, qujian = Static_area_threshold_1_5(max_blob)
            if(Static_area_threshold_flag == 7):
                Y_threshold = 60
                area_threshold, qujian = Static_area_threshold_1_7(max_blob)
            #print(qujian)
            #print(max_blob.cx(), max_blob.cy(), blob_area)

            if Determine_red_box_flag==0:
                errorFlag = is_rectangle(max_blob)
                if (errorFlag and max_blob.cy() >= Y_threshold and blob_area >= area_threshold):
                    white.on()
                    x = int(max_blob.cx())
                    y = int(max_blob.cy())
                    offset_angle = Offset_angle(x, y, max_blob)
                    filtered_x, filtered_y = moving_average_filter(x, y)
                    #median_x, median_y = median_filter(filtered_x, filtered_y)
                    uart.write("@%d,%d#" % (offset_angle + 45, filtered_y))
                    #print(filtered_x, filtered_y,max_blob.pixels())
                else:
                    uart.write("@45,0#")

            elif Determine_red_box_flag==1:
                # 检查位置是否有显著变化
                position_changed = check_position_change(max_blob.cx(), max_blob.cy())

                # 如果位置发生显著变化，重置赛道检测标志
                if position_changed:
                    Racetrack_checked_flag = False
                    On_Racetrack_result = False

                print(Racetrack_checked_flag,On_Racetrack_result)

                errorFlag = is_rectangle(max_blob)
                if (max_blob.cy() >= Y_threshold and blob_area >= area_threshold and errorFlag):
                    On_Racetrack_Flag = On_Racetrack(img,max_blob)
                    print(On_Racetrack_Flag)

                    if On_Racetrack_Flag:
                        white.on()
                        x = int(max_blob.cx())
                        y = int(max_blob.cy())
                        offset_angle = Offset_angle(x, y, max_blob)
                        filtered_x, filtered_y = moving_average_filter(x, y)
                        #median_x, median_y = median_filter(filtered_x, filtered_y)
                        uart.write("@%d,%d#" % (offset_angle + 45, filtered_y))
                        print(filtered_x, filtered_y,max_blob.pixels())
                    else:
                        uart.write("@45,0#")
        else:
            white.off()
            if Determine_red_box_flag==1:
                Racetrack_checked_flag = False
                On_Racetrack_result = False
                # 同时复位位置记录
                last_blob_cx = 0
                last_blob_cy = 0
    gc.collect()



