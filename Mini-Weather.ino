#include <Adafruit_BMP085.h>
#include <DHT.h>
#include <SoftwareSerial.h>
#define RX 2
#define TX 3
#define DHTPIN 7     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
String AP = "AndroidAP8290";       // AP NAME
String PASS = "uzka1681"; // AP PASSWORD
String API = "HHHRL6WVCETDH2SP";   // Write API KEY
String HOST = "api.thingspeak.com";
String PORT = "80";
String field1 = "field1";
String field2 = "field2";
String field3 = "field3";
String field4 = "field4";
String field6 = "field6";
String field7 = "field7";
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 
SoftwareSerial esp8266(RX,TX); 
Adafruit_BMP085 bmp;
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino
int UVOUT = A0; //Output from the sensor
int REF_3V3 = A1; //3.3V power on the Arduino board
int temt6000Pin = A2;
int chk;
float hum;  //Stores humidity value
float temp; //Stores temperature value
float temp_bmp;
float pressure_bmp;
  
void setup() {
  Serial.begin(9600);
  esp8266.begin(115200);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
  dht.begin();
  pinMode(UVOUT, INPUT);
  pinMode(REF_3V3, INPUT);
  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  }
}

void loop() {
  hum = dht.readHumidity();
  temp= dht.readTemperature();


  int uvLevel = averageAnalogRead(UVOUT);
  int refLevel = averageAnalogRead(REF_3V3);
  
  float outputVoltage = 3.3 / refLevel * uvLevel;
    
  float uvIntensity = mapfloat(outputVoltage, 0.99, 2.9, 0.0, 15.0);
  float volts = analogRead(temt6000Pin) * 5.0 / 1024.0;
  float amps = volts / 10000.0;
  float microamps = amps * 1000000;
  float lux = microamps * 2.0;
 
  String getData = "GET /update?api_key="+ API +"&"+ field1 +"="+String(lux);
  sendCommand("AT+CIPMUX=1",5,"OK");
  sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
  sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
  esp8266.println(getData);
  
  getData = "GET /update?api_key="+ API +"&"+ field2 +"="+String(hum);
  sendCommand("AT+CIPMUX=1",5,"OK");
  sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
  sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
  esp8266.println(getData);
 
  getData = "GET /update?api_key="+ API +"&"+ field3 +"="+String(temp);
  sendCommand("AT+CIPMUX=1",5,"OK");
  sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
  sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
  esp8266.println(getData);
  
  getData = "GET /update?api_key="+ API +"&"+ field4 +"="+String(uvIntensity);
  sendCommand("AT+CIPMUX=1",5,"OK");
  sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
  sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
  esp8266.println(getData);

  getData = "GET /update?api_key="+ API +"&"+ field6 +"="+String(bmp.readPressure());
  sendCommand("AT+CIPMUX=1",5,"OK");
  sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
  sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
  esp8266.println(getData);

  getData = "GET /update?api_key="+ API +"&"+ field7 +"="+String(bmp.readTemperature());
  sendCommand("AT+CIPMUX=1",5,"OK");
  sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
  sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
  esp8266.println(getData);
  delay(1500);
 countTrueCommand++;
 sendCommand("AT+CIPCLOSE=0",5,"OK");
}

void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("OYI");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
 }

 
int averageAnalogRead(int pinToRead)
{
  byte numberOfReadings = 8;
  unsigned int runningValue = 0; 

  for(int x = 0 ; x < numberOfReadings ; x++)
    runningValue += analogRead(pinToRead);
  runningValue /= numberOfReadings;

  return(runningValue);  
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
