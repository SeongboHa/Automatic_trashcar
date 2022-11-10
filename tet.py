import serial
import time
import cv2
import numpy as np
import math

 #블루투스 세팅
ser = serial.Serial(
        port='COM3',
        baudrate=9600,
)

size_x = 7200
size_y = 7200
xx = size_x-700
yy = size_y-700
home = (size_x-700, size_y-700)
points = []
theta = math.pi/2 # theta 초기값 90도
speed = 0
speed_max = 20
trig = 0

trash_point = (3600, 3600) #쓰레기 배출하는곳 좌표

current_point = [xx, yy]
target_points = []

#차에 메세지 보내기
# w : 전진/ a : 좌회전/ d : 우회전/ s : 후진/ x : 정지
def send_message(op):
    ser.write(op.encode())



def drawPoints(imgg, points):
        for point in points:
                cv2.circle(imgg, point, 20, (0, 0, 255), cv2.FILLED)
                cv2.circle(imgg, points[-1], 40, (0, 255,0), cv2.FILLED)

        for point in target_points:
                cv2.circle(imgg, point, 50, (255, 255, 0), cv2.FILLED)

        cv2.circle(imgg, trash_point, 50, (255, 255, 255), cv2.FILLED)

def drawing():
    global xx, yy, speed, theta
    xx += speed * math.cos(theta)
    yy -= speed * math.sin(theta)

    imgg = np.zeros((size_x, size_y, 3), np.uint8)
    points.append((int(xx), int(yy)))
    drawPoints(imgg, points)
    imggg = cv2.resize(imgg,(720,720))
    return imggg

#차량 제어 함수, msg로 차량에 보낼 신호 지정
def car_rotate(vector, theta_move): #0.0133ms/degree
    if vector == "left":
        send_message("l")
        time.sleep(1.36/90*abs(theta_move)*180/math.pi)
        send_message("s")
    else:
        send_message("r")
        time.sleep(1.36/90*abs(theta_move)*180/math.pi)
        send_message("s")
    

def car_move(next_point): #단위 : cm #0.025s/cm
    global speed, xx, yy
    send_message("f")
    speed = speed_max
    while True:
        img_moving = drawing()
        cv2.imshow("moving", img_moving)
        cv2.waitKey(1)
        print(xx, "  ",yy)
        if((xx>=next_point[0]-30 and xx<=next_point[0]+30) and (yy>=next_point[1]-30 and yy<=next_point[1]+30)):
            break

    send_message("s")
    speed = 0

def trash_out():
    send_message("o")   #쓰레기 배출 명령
    while True:
        if ser.in_waiting != 0:
            content = ser.readline()
            content = content[:-1].decode()
            if content=="ok":
                break
    
def car_remote(next_point):
    global theta
    theta_next = math.atan2(yy - next_point[1], next_point[0] - xx)
    #theta_next값 양수로 보정
    if theta_next < 0:
        theta_next += 2 * math.pi
    
    #각도 조정
    move_theta = theta_next - theta #움직여야 할 각도
    if move_theta > 0:
        if move_theta > math.pi:
            car_rotate("right", math.pi*2 - move_theta)
        else:
            car_rotate("left", move_theta)
        
    if move_theta < 0:
        if move_theta < -1*math.pi:
            car_rotate("left", math.pi*2 + move_theta)
        else:
            car_rotate("right", move_theta)
    
    theta = theta_next #회전 후에는 목표각 = 현재각
    print(math.degrees(theta))
    #이동
    car_move(next_point)

#start로 출발하는 파트
# print("start로 출발")
# while True:
#         if input() == "start":
#                 #send_message("start")
#                 break

# print("탈출")
content = 0


#목표 좌표들 마우스로 찍어주는 파트
while True:
        img = drawing()
        cv2.imshow("output", img)
        x_pos, y_pos, ww, hh = cv2.selectROI("output", img, False) #클릭해서 목표 좌표값 입력
        if (x_pos==0 & y_pos==0):
            break
        target_points.append((x_pos*10, y_pos*10)) 
        print(target_points)

cv2.destroyAllWindows()

#차 운전하는 파트
for i in target_points:
    #다음 지점까지의 각도
    car_remote(i)
    send_message('p')

    while True:
        if ser.in_waiting != 0:
            content = ser.readline()
            content = content[:-1].decode()
        if content == "next":
            break
        
    print("탈출1")
    
    #쓰레기통 잔량 체크
    send_message('c')   #쓰레기통에 잔량 체크 명령 전송

    while True:
        if ser.in_waiting != 0:
            content = ser.readline()
            content = content[:-1].decode()
            break

    print("탈출")
    if content == "full":
        car_remote(trash_point)
        trash_out()
        break

car_remote(home)

while True:
    img = drawing()
    cv2.imshow("output", img)
    cv2.waitKey(1)