/*
   03/09/2017
   Jose Maria Cesario Jr
   Quadro IoT - Exemplo IBM Watson IoT Platform
   Hardware: Arduino MEga, Ethernet Shield, GLCD 128x64, LCD 16x4
   Sensores: qualidade ar SeeedStudio, Temp + Umid DHT 11, LDR Seeedstudio, Barometro BMP180, Chuva, Umid Solo

   Logica:
   1. efetuar conexao com a rede
   2. obter as grandezas dos sensores
   3. conectar no IBM Watson IoT Platform
   4. publicar a JSON string para o topico
   
   referencias conversao float para string
   http://www.hobbytronics.co.uk/arduino-float-vars
   http://forum.carriots.com/index.php/topic/61-wireless-gardening-with-arduino-cc3000-wifi-modules/page-2
*/

#include <Ethernet.h>
#include <PubSubClient.h>
#include <IPStack.h>
#include <Countdown.h>
#include <MQTTClient.h>
#include <SPI.h>
#include <dht.h>
#include <openGLCD.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <AirQuality.h>
#include <LiquidCrystal.h>
#include <bitmaps/ArduinoIcon96x32.h>
#include <bitmaps/ibm.h>
#include <bitmaps/bluemix.h>

Adafruit_BMP085 bmp;
dht DHT;
EthernetClient ethClient;

//initialize lcd1 16x4 char
LiquidCrystal lcd1(38, 39, 43, 42, 41, 40);

//WIoTP parameters
char server[] = "org";
int port = 1883;
char topic[] = "iot-2/evt/status/fmt/json";
// clientName format d:org:deviceType:deviceId
String clientName = "d:org:deviceType:deviceId";
char token[] = "password";

PubSubClient pubSubClient(server, port, 0, ethClient);

//Ethernet setup
byte mac[]    = {0x98, 0x4b, 0x4a, 0x15, 0x91, 0x1b };
char macstr[] = "984b4a15911b";
byte ip[]     = {192, 168, 0, 177 };

//Cria variaveis globais
char charBuf[30];

float pressao = 0.0;
float altitude = 0.0;
float temperatura = 0.0;

char pressaostr[6];
char altitudestr[6];
char temperaturastr[6];

int valDigitalChuva = 0;
int umidadeSolo = 0;

int soilMoisture = 0;
int humidity = 0;
int airq = 0;
int lumen = 0;

int rain = 0;
int val_a = 0;

const int thresholdvalue = 12;  //The threshold for which the LED should turn on.
float Rsensor;                  //Resistance of sensor in K

AirQuality airqualitysensor;
int current_quality = -1;

void setup() {
  Serial.begin(9600);

  delay(3000);
  //to LCD1
  lcd1.begin(16, 2);
  lcd1.setCursor(0, 0);
  lcd1.print("Inic Quadro IoT...");
  delay(2000);
  
  lcd1.setCursor(0, 0);
  lcd1.print("Inic Sens Qua Ar");
  airqualitysensor.init(14);
  lcd1.setCursor(0, 0);

  lcd1.setCursor(0, 0);
  lcd1.print("Inic Barometro  ");
  delay(1000);
  /* Initialise the sensor */
  if (!bmp.begin())
  {
    Serial.print("Falha BMP180. Resetar");
    lcd1.setCursor(0, 1);
    lcd1.print("Falha BMP180. Resetar");
    while (1);
  }

  //to LCD1
  lcd1.setCursor(0, 0);
  lcd1.print("Inic Conex Rede ");

  if (Ethernet.begin(mac) == 0)  // Start in DHCP Mode
  {
    Serial.println("Failed to configure DHCP, using Static Mode");
    // If DHCP Mode failed, start in Static Mode
    Ethernet.begin(mac, ip);
  }
  Serial.print("IP address: ");
  Serial.println(Ethernet.localIP());

  lcd1.setCursor(0, 0);
  lcd1.print("IP Atribuido    ");

  lcd1.setCursor(0, 1);
  lcd1.print(Ethernet.localIP());
  delay (2000);

  // Initialize the GLCD 
  GLCD.Init();

  // Select the font for the default text area
  GLCD.SelectFont(System5x7);
  GLCD.ClearScreen();

  GLCD.DrawBitmap(ArduinoIcon96x32, GLCD.Width/2 - 48, 0);
  GLCD.DrawString("Quadro IoT", 40, 40, eraseNONE);
  delay(2000);
  GLCD.ClearScreen();

  GLCD.DrawBitmap(ibm, 0, 0);
  GLCD.DrawString("IBM Watson IoT Platform", 40, 40, eraseNONE);
  delay(2000);
  GLCD.ClearScreen();

  // led azul comunicacao WIoTP
  pinMode(44, OUTPUT);
}

void loop() {

  // Connect MQTT Broker
  Serial.println("[INFO] Connecting to MQTT Broker");

  char clientStr[60];
  clientName.toCharArray(clientStr, 60);

  if (pubSubClient.connect(clientStr, "use-token-auth", token)) {
    Serial.println("[INFO] Connection to MQTT Broker Successfull");
  }
  else {
    Serial.println("[INFO] Connection to MQTT Broker Failed");
  }

  //******* Sensor 01 - Qualidade do ar
  current_quality = airqualitysensor.slope();

  //******* Sensor 02 - Temp Umid 
  DHT.read11(A1);

  //******* Sensor 03 - LDR Luminosidade
  int ldr = analogRead(A2);
  lumen = 0;
  Rsensor = (float)(1023 - ldr) * 10 / ldr;
  if (Rsensor > thresholdvalue){
    lumen = 0;
  }
  else {
    lumen = 1;
  }

  //******* Sensor 04 - BMP180 Temp Barometro Altitude
  pressao = (float)bmp.readPressure();
  altitude = (float)bmp.readAltitude();
  temperatura = (float)bmp.readTemperature();

  dtostrf(pressao, 6, 0, pressaostr);
  dtostrf(altitude, 6, 0, altitudestr);
  dtostrf(temperatura, 6, 0, temperaturastr);

  // Conversao Floats para Strings
  char TempString[32];  //  array de character temporario

  // dtostrf( [Float variable] , [Minimum SizeBeforePoint] , [sizeAfterPoint] , [WhereToStoreIt] )
  dtostrf(temperatura, 2, 1, TempString);
  String temperaturastr =  String(TempString);
  dtostrf(pressao, 2, 2, TempString);
  String pressaostr =  String(TempString);
  dtostrf(altitude, 2, 2, TempString);
  String altitudestr =  String(TempString);

  //******* Sensor 05 - Chuva
  rain = analogRead(A3);

  //******* Sensor 06 - Umid Solo
  umidadeSolo = analogRead(A4);
  soilMoisture = map (umidadeSolo, 0, 948, 99, 0);
  
  // Publish to MQTT Topic
  if (pubSubClient.connected())
  {
    Serial.println("Publishing to WIoTP");

    //String data = "{\"d\":{\"air\": \"" + String(airq) + "\",\"temperature\":\"" + String(DHT.temperature) + "\",\"humidity\":\"" + String(DHT.humidity) + "\",\"light\":\"" + String(lumen) + "\",\"rain\":\"" + String(rain) + "\",\"soil\":\"" + String(soilMoisture)  + "\"}}";

    String data = "{\"d\":{\"air\": \"" + String(current_quality) + "\",";
    data = data + "\"temperature\":\"" + String(DHT.temperature) + "\",";
    data = data + "\"humidity\":\"" + String(DHT.humidity) + "\",";
    data = data + "\"light\":\"" + String(lumen) + "\",";    
    data = data + "\"pressao\":\"" + String(pressaostr) + "\",";
    data = data + "\"altitude\":\"" + String(altitudestr) + "\",";
    data = data + "\"rain\":\"" + String(rain) + "\",";
    data = data + "\"soil\":\"" + String(soilMoisture)  + "\"}}";
    
    char jsonStr[400];
    data.toCharArray(jsonStr, 400);

    Serial.print("[INFO] JSON Data: ");
    Serial.println(jsonStr);

    char topicStr[33];
    String topicName = topic;
    topicName.toCharArray(topicStr, 33);

    if (pubSubClient.publish(topicStr, jsonStr))
    {
      digitalWrite(44, HIGH);
      delay(1000);
      digitalWrite(44, LOW);
      Serial.println("[INFO] Data Published Successfully");
    }
    else
    {
      Serial.println("[INFO] Failed to Publish Data");
    }

    Serial.println("[INFO] Disconnecting Server");
    pubSubClient.disconnect();
  }

  Serial.println("[INFO] Publish to MQTT Broker Complete");
  Serial.println("-----------------------------------------------");

  //Write data to GLCD

  GLCD.ClearScreen();
  GLCD.DrawString("Qualid Ar:", 5, 1, eraseNONE);
  switch (current_quality) {
    case 0:
      GLCD.print("Péssima");
      break;
    case 1:
      GLCD.print("Ruim");
      break;
    case 2:
      GLCD.print("Moderada");
      break;
    case 3:
      GLCD.print("Boa");
      break;
  }

   GLCD.DrawString("Temp (oC):", 5, 11, eraseNONE);
   GLCD.print(DHT.temperature);
  
   GLCD.DrawString("Umidade (%):", 5, 21, eraseNONE);
   GLCD.print(DHT.humidity);
  
   GLCD.DrawString("Luz amb:",5 , 31, eraseNONE);
   switch (lumen) {
    case 1:
      GLCD.print("Acessa");
      break;
    case 0:
      GLCD.print("Apagada");
      break;
    }

  GLCD.DrawString("Pres atm (HPa):", 5, 41, eraseNONE);
  GLCD.print(String(pressaostr));
  
  GLCD.DrawString("Altitude (m):", 5, 51, eraseNONE);
  GLCD.print(String(altitudestr));
  delay(3000);
  
  GLCD.ClearScreen();
  GLCD.DrawString("Chuva: ", 5, 1, eraseNONE);

  val_a = analogRead(A3);
  if (val_a >700 && val_a <1024)
  {
    GLCD.print("sem chuva");
  }
  if (val_a >400 && val_a <700)
  {
    GLCD.print("moderada");
  }
  if (val_a > 0 && val_a <400)
  {
     GLCD.print("forte");
  }
  
  GLCD.DrawString("Umid Solo (%):", 5, 11, eraseNONE);
  GLCD.print(soilMoisture);   
  
  delay(3000);
  pubSubClient.loop();
}

ISR(TIMER2_OVF_vect)
{
  if (airqualitysensor.counter == 122) //set 2 seconds as a detected duty
  {

    airqualitysensor.last_vol = airqualitysensor.first_vol;
    airqualitysensor.first_vol = analogRead(A0);
    airqualitysensor.counter = 0;
    airqualitysensor.timer_index = 1;
    PORTB = PORTB ^ 0x20;
  }
  else
  {
    airqualitysensor.counter++;
  }
}
