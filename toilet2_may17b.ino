#include <MQ135.h>

#include <DHT.h>
#include <DHT_U.h>

#include <DHT11.h>

#include <ESP8266WiFi.h> 
#include <ESP8266HTTPClient.h>

#define DHTPIN D3
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

const char* ssid     = "iPhone709";       //wifi id
const char* password = "01052667709";  //wifi pw
String host = "http://43.200.171.17/"; 

const long interval = 5000; 
unsigned long previousMillis = 0;   
WiFiServer server(80); 
WiFiClient client; 
HTTPClient http;

int co2Sensor = 0;
int coSensor = 14;
int GasSensor = 15;

int redPin = 5;
int greenPin = 4;

int GasPin = A0;


int dust_sensor = A0;            // 미세먼지 핀 번호
float dust_value = 0;            // 센서에서 입력받은 미세먼지 값
float dustDensityug=0;

int sensor_led = 4;    // 미세먼지 센서 안에 있는 적외선 led 핀 번호
int sampling = 280;    // 적외선 led를 키고, 센서값을 읽어들여 미세먼지를 측정하는 샘플링 시간
int waiting = 40;     
float stop_time = 9680;   // 센서를 구동하지 않는 시간

float dust_init = 0;
float dust_initial = 0;
 
int distance=0 ;
int measure=0 ;
int count=0;
bool flag=false;
bool dbflag=false;
 
void wifi(){
  while(!Serial){;}
    delay(500);
    Serial.println("Serial Port Connected..");

  WiFi.mode(WIFI_STA); 
  WiFi.begin(ssid, password); 
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500); 
    Serial.print("."); 
  } 
  Serial.println(""); 
  Serial.print("Connecting to "); 
  Serial.println(ssid); 
//print ip address
  Serial.print("IP address: "); 
  Serial.println(WiFi.localIP()); 
//server start
  server.begin(); 
  Serial.println("Server started"); 
}

void sendDB(float count, float count2, float count3, float count4, float count5, float count6){
  String phpHost=host+"/sensor.php?tmp="+String(count)+"&hmd="+String(count2)+"&pm="+String(count3)+"&cd="+String(count5)+"&so="+String(count)+"&vo="+String(count6)+"&co="+String(count4)+"&time";

    http.begin(client, phpHost);
    http.setTimeout(1000);
    int httpCode = http.GET();
    
    if(httpCode > 0) {
      Serial.printf("GET code : %d\n\n", httpCode);
 
      if(httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println(payload);
      }
    } 
    else {
      Serial.printf("GET failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();

}


void setup() { 
  Serial.begin(115200);  

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  
  dht.begin();

  pinMode(GasPin, INPUT);
  pinMode(sensor_led,OUTPUT);    // 미세먼지 적외선 led를 출력으로 설정
  for(int i = 0 ; i < 5 ; i++){       //미세먼지 측정센서 초기 값 구하기 
  digitalWrite(sensor_led, LOW);        //미세먼지 측정 5번하기
  delayMicroseconds(sampling);
  dust_init += analogRead(dust_sensor);
  delayMicroseconds(waiting);
  digitalWrite(sensor_led, HIGH);
  delayMicroseconds(stop_time);
  }
  dust_initial = (((dust_init/5)*5.0)/1024.0);   //측정한 5번 미세먼지 값 평균 구하기
  Serial.print("dust_initial : ");
  Serial.println(dust_initial);
   
  WiFi.disconnect();
  
}

void loop() {

  setColor(0, 255); // green
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  Serial.print("Temp : ");
  Serial.println(t);
  Serial.print("Humi : ");
  Serial.println(h);

  MQ135 gasSensor = MQ135(co2Sensor);
  float rzero = gasSensor.getRZero();
  Serial.print("CO2 : ");
  Serial.println(rzero);

  digitalWrite(sensor_led, LOW);  // LED 켜기
  delayMicroseconds(sampling);   // 샘플링해주는 시간. 

  dust_value = analogRead(dust_sensor); // 센서 값 읽어오기
 
  delayMicroseconds(waiting);   // 너무 많은 데이터 입력을 피해주기 위해 잠시 멈춰주는 시간. 

  digitalWrite(sensor_led, HIGH);  // LED 끄기
  delayMicroseconds(stop_time);    // LED 끄고 대기  

  dustDensityug = ((((dust_value * (5.0 / 1024)) - dust_initial) /0.005));  // 미세먼지 값 계산
  Serial.print("Dust Density [ug/m^3]: ");  // 시리얼 모니터에 미세먼지 값 출력
  Serial.println(dustDensityug);

  float dustDensityug = 25.68;
  float vo = digitalRead(GasPin);
  
  Serial.print("pm :");
  Serial.println(dustDensityug);
  
  Serial.print("vo :");
  Serial.println(vo);

  float cd = digitalRead(coSensor);
  Serial.print("CO :");
  Serial.println(cd);

  if(t > 26 || h<40 || h>60 || cd > 30 || dustDensityug > 80 || rsero > 1000 || vo>180){
    setColor(255, 0); // red
  }

  wifi();

//  Serial.println(WiFi.status());
  sendDB(t,h,dustDensityug,rzero,cd,vo);
  dbflag=true;
  WiFi.disconnect();
//  Serial.println(WiFi.status());
  delay(5000); // ten seconds 
  
}

void setColor(int red, int green)
{
  digitalWrite(redPin, red);
  digitalWrite(greenPin, green);

}
