#include "ESP8266.h"
#include "PubSubClient.h"
#include "SoftwareSerial.h"
bool k = 0, j = 0;//控制只发送一次数据

#define SSID "CYi的HUAWEI P40 Pro"    //填写2.4GHz的WIFI名称，不要使用校园网
#define PASSWORD "cy021024"//填写自己的WIFI密码
#define HOST_NAME "api.heclouds.com"  //API主机名称，连接到OneNET平台，无需修改
#define DEVICE_ID "653370254"       //填写自己的OneNet设备ID
#define HOST_PORT (80)                //API端口，连接到OneNET平台，无需修改
String APIKey = "pUZ5NDFVDOMsyKalWCcWNOnukI="; //与设备绑定的APIKey
const char* DeviceID = "192.168.43.14"; //设备号

#define INTERVAL_SENSOR 5000 //定义传感器采样及发送时间间隔
int Sensor_pin = 7;//传感器引脚
int relay_pin = 3;//继电器一脚、、引脚
//计时变量
long previousMillis = 0;
int interval = 6000,delay_interval=12000;
unsigned long currentMillis = 0, leaveMillis = 0;
#define SC-HR501PIN 7
 //定义ESP8266所连接的软串口
/*********************
 * 该实验需要使用软串口
 * Arduino上的软串口RX定义为D3,
 * 接ESP8266上的TX口,
 * Arduino上的软串口TX定义为D2,
 * 接ESP8266上的RX口.
 * D3和D2可以自定义,
 * 但接ESP8266时必须恰好相反
 *********************/
SoftwareSerial mySerial(3, 2);
ESP8266 wifi(mySerial);

void setup1()
{
  mySerial.begin(115200); //初始化软串口
  Serial.begin(9600);     //初始化串口
  Serial.print("setup begin\r\n");

  //以下为ESP8266初始化的代码
  Serial.print("FW Version: ");
Serial.println(wifi.getVersion().c_str());

  if (wifi.setOprToStation()) {
    Serial.print("to station ok\r\n");
  } else {
    Serial.print("to station err\r\n");
  }

  //ESP8266接入WIFI
  if (wifi.joinAP(SSID, PASSWORD)) {
    Serial.print("Join AP success\r\n");
    Serial.print("IP: ");
    Serial.println(wifi.getLocalIP().c_str());
  } else {
    Serial.print("Join AP failure\r\n");
  }

  Serial.println("");
  
 

  mySerial.println("AT+UART_CUR=9600,8,1,0,0");
  mySerial.begin(9600);
  Serial.println("setup end\r\n");
}
//设置回调函数来用返回的数据存储到payload中，操作继电器
int incomedate = 0;
int relayPin = 3; //继电器引脚
int irSensorPin = 7 ;            // 连接红外传感器引脚
bool irSensorOutput;            // 红外传感器输出信号
 
void setup2()
 {
    pinMode(relayPin, OUTPUT);
    //设置引脚13为输出接口，Arduino 上我们用到的I/O 口都要进行类似这样的定义。  
    Serial.begin(9600); 
    //设置波特率为9600，这里要跟软件设置相一致。当接入特定设备时，我们也要跟其他设备的波特率达到一致。  



    pinMode(irSensorPin, INPUT);
  Serial.begin(9600);
  Serial.println("haha");

  
}
 
void loop1() 
{
  
  
  if (Serial.available() > 0)//串口接收到数据
  {
    
  }
  incomedate = Serial.read();//获取串口接收到的数据
    irSensorOutput = digitalRead(irSensorPin);  // 读取红外传感器输出
  if (irSensorOutput == HIGH) {  // 如果红外传感器输出高电平
      Serial.println("Carbon-based creature detected");
      digitalWrite(relayPin, HIGH);
      Serial.println("LED OPEN!");
  } else {
      Serial.println("There is no carbon-based creature moving for");
      digitalWrite(relayPin, LOW);
      Serial.println("LED CLOSE!");
  }
  delay(100);

 
}





 //数据上传服务器时间
void loop2() {
  currentMillis = millis();
  int val = digitalRead(Sensor_pin);//读取红外传感器数值，1为有人，0为无人

  if (val == 1)//当有人时
  { Serial.println("Carbon-based creature detected");
    delay(1000);
    //    digitalWrite(relay_pin, HIGH);//继电器接通
    previousMillis = currentMillis   ;//计时器停止工作
    k = 0;
    if (j == 0) {//k，j的存在使当val数值变化后，只向服务器发送一词报文
  String postString = "POST /devices/";
      postString += DEVICE_ID;
      postString += "/datapoints?type=3 HTTP/1.1";
      postString += "\r\n";
      postString += "api-key:";
      postString += APIKey;
      postString += "\r\n";
      postString += "Host:api.heclouds.com\r\n";
      postString += "Connection:close\r\n";
      postString += "1\r\n";

      const char *postArray = postString.c_str(); //将str转化为char数组

      Serial.println(postArray);
      wifi.send((const uint8_t *)postArray, strlen(postArray)); //send发送命令，参数必须是这两种格式，尤其是(const uint8_t*)
      Serial.println("send success");;
      }

      j++;
    }

  

  else
  { Serial.println("There is no carbon-based creature moving for");//如果红外传感器检测到没人，在串口打已经没有人的时长
    Serial.print((currentMillis - previousMillis) / 1000, DEC);
    Serial.println(" second");
    delay(1000);
  }
  if (currentMillis - previousMillis > interval)//如果没人时长大于1小时，向服务器发送报文新增数据点。
  { 
    if (k == 0&&j!=0)
    { 
      leaveMillis=currentMillis;

        String postString = "POST /devices/";
      postString += DEVICE_ID;
      postString += "/datapoints?type=3 HTTP/1.1";
      postString += "\r\n";
      postString += "api-key:";
      postString += APIKey;
      postString += "\r\n";
      postString += "Host:api.heclouds.com\r\n";
      postString += "Connection:close\r\n";
     
     
      postString += "0\r\n";

      const char *postArray = postString.c_str(); //将str转化为char数组

      Serial.println(postArray);
      wifi.send((const uint8_t *)postArray, strlen(postArray)); //send发送命令，参数必须是这两种格式，尤其是(const uint8_t*)
      Serial.println("send success");
      }
      k++;

   
    j = 0;
    if (currentMillis-leaveMillis>delay_interval&&leaveMillis!=0) {
      digitalWrite(relay_pin,LOW);
      leaveMillis=0;
    }
  }


  if (currentMillis < previousMillis)//当计时器溢出，归零
    previousMillis = currentMillis = 0;


}
