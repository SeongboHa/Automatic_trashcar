#include <Servo.h>
#include <SoftwareSerial.h>

int trigPin = 6; //적외선센서 trigpin
int echoPin = 7; //적외선센서 ecoPin

int top_servo = 5; //뚜껑
int front_servo = 3; //앞문
int hit_servo = 11;

SoftwareSerial BTSerial(9,10); //TXD, RXD

//모터 제어 핀들
int motorPin1 = 13;
int motorPin2 = 8;
int motorPin3 = 4;
int motorPin4 = 12;

//모터 속도 제어 변수
int rotate_val = 240; //회전속도
int rotate_time = 1500; //회전시간(ms)


//서보모터 클래스
Servo topservo; //뚜껑 0 : 열림 , 90 : 닫힘
Servo frontservo; //앞문
Servo hit; //배출 100:default, 0 : 배출


////////////함수 선언단/////////////

//모터 제어
void left_motors(int direc); //좌측 바퀴들 제어 전진 : 1, 후진 : 0
void right_motors(int direc); //우측 바퀴들 제어 전진 : 1, 후진 : 0
void rotate_left(); //좌회전
void rotate_right(); //우회전
void motor_stop(); // 모터 정지
void go_forward(int time); //차체 전진
void go_backward(int time); //차체 후진


//적외선센서
int short_distance(); //근거리 적외선센서, 거리를 mm단위로 출력//trigpin, echopin

//문 여닫기

void setup() {
  Serial.begin(9600);
  BTSerial.begin(9600);
  
  //적외선센서
  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);
  
  //서보모터
  topservo.attach(top_servo);
  frontservo.attach(front_servo);
  hit.attach(hit_servo);
  
  //모터
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);
}

  //if(BTSerial.available())
  //Serial.write(BTSerial.read());
  //if(Serial.available())
  //BTSerial.write(Serial.read());

  
void loop() {
  int a = 100;
  char message;
  while(1)
  {
    if(BTSerial.available())
    {
      if(BTSerial.read() == "start");
      BTSerial.write("from Arduino : 출발\n");
      break;
    }
  }
  
  hit.write(100);
  go_forward(50*a);
  topservo.write(0);
  delay(1000);
  topservo.write(90);
  delay(1000);
  rotate_left();
  Serial.println(short_distance());
  go_forward(50*a);
  topservo.write(0);
  delay(1000);
  topservo.write(90);
  delay(1000);

  if(short_distance() < 70)
  {
    Serial.println("쓰레기 배출");
    go_forward(50*a);
    frontservo.write(0);
    delay(1000);
    hit.write(0);
    delay(3000);
    hit.write(100);
    frontservo.write(90);
    rotate_left();
    go_forward(50*a);
    rotate_left();
    go_forward(50*a);
    
  }
  else
  {
    rotate_left();
    go_forward(50*a);
    rotate_left();
    topservo.write(0);
    delay(1000);
    topservo.write(90);
  }
  go_forward(50*a);
  rotate_left();
  
  delay(1000);
  Serial.println("쓰레기 체크");
  Serial.println(short_distance());
  

  go_forward(50*a);
  rotate_left();
  
  
}



///////////////////////모터 제어 함수들///////////////////////////////

//왼쪽 모터 제어
//velocity는 0~255
//direc(방향)은 전진 : 1, 후진 : 0
void left_motors(int direc)
{
  
  if (direc == 1)
  {
    digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, LOW);
  }
  if (direc == 0)
  {
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, HIGH);
  }
}

//우측 모터 제어
//velocity는 0~255
//direc(방향)은 전진 : 1, 후진 : 0
void right_motors(int direc)
{
  if (direc == 1)
  {
    digitalWrite(motorPin3, HIGH);
    digitalWrite(motorPin4, LOW);
  }
  if (direc == 0)
  {
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, HIGH);
  }
}

//모터 정지
void motor_stop()
{
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, LOW);
}

//차체 전진
void go_forward(int time)
{
  motor_stop();
  BTSerial.write("전진_start\n");
  left_motors(1);
  right_motors(1);
  delay(time);
  motor_stop();
  Serial.println("전진");
  BTSerial.write("전진_end\n");
}

//차체 후진
void go_backward(int time)
{
  motor_stop();
  BTSerial.write("후진_start\n");
  left_motors(0);
  right_motors(0);
  delay(time);
  motor_stop();
  Serial.println("후진");
  BTSerial.write("후진_end\n");
}

//좌회전
void rotate_left()
{
  motor_stop();
  BTSerial.write("좌회전\n");
  left_motors(0);
  right_motors(1);
  delay(rotate_time);
  motor_stop();
  Serial.println("좌회전");
  
}
//우회전
void rotate_right()
{
  motor_stop();
  BTSerial.write("우회전\n");
  left_motors(1);
  right_motors(0);
  delay(rotate_time);
  motor_stop();
  Serial.println("우회전");
  
}

//지정된 거리만큼 이동

///////////////////////모터 제어 함수들///////////////////////////////

///////////////////////적외선센서////////////////////////////

//거리 mm 단위로 출력
int short_distance()
{
  long duration, distance;
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = ((float)(340 * duration) / 1000) / 2;

  return distance;  
}

///////////////////////적외선센서////////////////////////////
