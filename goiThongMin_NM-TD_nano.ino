
#include <virtuabotixRTC.h>
#include "SharpGP2Y10.h"
#include "Adafruit_SHT31.h"
#include <Wire.h>
#include "SoftwareSerial.h"

#define MQ135Pin A3
#define MQ3Pin A7
#define voPin A6
#define FLEX1 A0
#define FLEX2 A1
#define FLEX3 A2
#define ledPin 2
#define NGUONG 850


virtuabotixRTC myRTC(3, 4, 5);
SharpGP2Y10 dustSensor(voPin, ledPin);
SoftwareSerial mySerial(6, 7);

float dustDensity = 0;
int lastState = 0;
int currentState = 0;
unsigned long lastOneSecond = 0;
unsigned long time_up  = 0;
unsigned long time = 0;
bool check = false;
float t, h;
int giatri_mtr, giatri_con , flex1, flex2 ,flex3; 
unsigned long updateTime = 0;
String data = "", sleepHour = "";
Adafruit_SHT31 sht31 = Adafruit_SHT31();


void setup() {
  pinMode(FLEX1, INPUT);
  pinMode(FLEX2, INPUT);
  pinMode(FLEX3, INPUT);
 // myRTC.setDS1302Time(0, 37, 18, 3, 17, 10, 2023);
  Serial.begin(9600);
  mySerial.begin(4800);
  Serial.println("MQ3 warming up!");
  Serial.println("SHT31 test");
  if (!sht31.begin(0x44)) {
    Serial.println("Couldn't find SHT31");
  }

}

void loop() {
  flexRead();
  read_sensor();
  printDebug();

  if (millis() - updateTime > 5000) {
    updateTime = millis();
    update_data();
    Serial.println("da update du lieu");
  }
}

void read_sensor() {
  myRTC.updateTime();
  giatri_mtr = analogRead(MQ135Pin);
  giatri_con = analogRead(MQ3Pin);  // read analog input pin 0
  dustDensity = dustSensor.getDustDensity();
  t = sht31.readTemperature();
  h = sht31.readHumidity();
  sleepHour = String(myRTC.dayofmonth) + "," + String(myRTC.month) + "," + String(myRTC.year) + "," + String(myRTC.hours) + "," + String(myRTC.minutes) + "," + String(myRTC.seconds);
}

int timeCount() {
  if (millis() - lastOneSecond > 1000) {
    lastOneSecond = millis();
    time++;
  }
}

int getState() {
  flex1 = analogRead(FLEX1);
   flex2 = analogRead(FLEX2);
  flex3 = analogRead(FLEX3);

  if (flex1 > NGUONG) {
    return 1;
  } else if (flex2 > 810) {
    return 2;
  } else if (flex3 > 840) {
    return 3;
  } else {
    return 0;
  }
}
void printDebug() {
  // Serial.print("sleepHour: ");
  // Serial.print(sleepHour);
  Serial.print("\t flex1: ");
    Serial.print(flex1);
    Serial.print("\t flex2: ");
      Serial.print(flex2);
    Serial.print("\t flex3: ");
      Serial.print(flex3);
      //    Serial.print(" check: ");
      // Serial.print(check);
      //  Serial.print("\t time_up: ");
      // Serial.print((millis() - time_up)/1000);
      //    Serial.print("\t timeCount ");
      // Serial.print(time);
      //   Serial.print("\t currentState: ");
      // Serial.print(currentState);
      //  Serial.print("\t lastState: ");
      // Serial.print(lastState);
      //  Serial.print("\t data: ");
      // Serial.print(data);
 
  //   Serial.print("\t giatri_mtr: ");
  // Serial.print(giatri_mtr);
  //   Serial.print("\t dustDensity: ");
  // Serial.print(dustDensity);
  // Serial.print("\t giatri_con: ");
  // Serial.print(giatri_con);
  // Serial.print("\t t: ");
  // Serial.print(t);
  // Serial.print("\t h: ");
  // Serial.print(h);
   Serial.println();
}

void flexRead() {
  // put your main code here, to run repeatedly:
  currentState = getState();

  if (currentState != lastState) {
    data += String(sleepHour) + "," + String(lastState) + "," + String(time) + ";";
    time = 0;
    check = true;
  }
  if (currentState != 0) {
    timeCount();
  time_up = millis();
  }
  if (check == true) {
    if (currentState == 0 && lastState == 0) {
          Serial.print("1112345555666666666677777777777777777777777777777777");
      if (millis() - time_up > 30000) {
        data += "\n";
        //Serial.println(data);
        mySerial.print(data);
        Serial.println(data);
        check = false;
        data = "";
        Serial.print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
        delay(100000);
      }
    }
  }

  lastState = currentState;
  // check state, and if state =1 ; then increase time1 by 1 every 1s
}

void update_data() {
  String data1[5] = { String(giatri_mtr), String(giatri_con), String(dustDensity), String(t), String(h) };

  String combinedData = "envir:";  // Khởi tạo chuỗi kết quả

  // Kết hợp các dữ liệu vào chuỗi
  for (int i = 0; i < 5; i++) {
    combinedData += data1[i];
    if (i < 4) {
      combinedData += ",";  // Thêm dấu phẩy nếu không phải là phần tử cuối cùng
    }
  }
  combinedData += "\n";  // Thêm ký tự xuống dòng

  // Gửi chuỗi dữ liệu qua UART
  mySerial.print(combinedData);
 Serial.print(combinedData);
  //delay(100);
  //nhan_dulieu();
}
