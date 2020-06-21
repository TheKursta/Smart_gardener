/* Attach DHT22 to D6, attach Soil_temp to D3, attach Servo to D5*/
/* Authentication*/
char auth[] = "hPXFEpmrVwiv-CIrJyD1k_4aSdtlsg5q";
char ssid[] = "smart_wifi";
char pass[] = "honolulu";
char server[] = "101.101.101.101";

/* Blynk virtual pins*/
#define virtualButton = V0
#define virtualLED = V7
#define virtualTemp = V1
#define virtualHum = V2
#define virtualSoilTemp = V3
#define ON 255
#define OFF 0
bool isFirstConnect = true;

/* DHT22*/
#include <DHT.h>
#define DHTPIN D6
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
float DHT_hum = 0;
float DHT_temp = 0;

/* Temperature*/
#include <OneWire.h>
#include <DallasTemperature.h>
#define wire  D3
OneWire oneWire(wire);
DallasTemperature sensors(&oneWire);

/* Servo*/
#include <Servo.h>
Servo servo;
int servo_state;
int state;

/* ESP & Blynk */
#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>


/***************************************************
 * Get Air Humidity and Temperature Sensor data
 **************************************************/

void getDhtData(void)
{
  float tempIni = DHT_temp;
  float humIni = DHT_hum;
  DHT_temp = dht.readTemperature();
  Serial.println("Air temperature is: ");
  Serial.println(DHT_temp);
  DHT_hum = dht.readHumidity();
  Serial.println("Air relative humidity is: ");
  Serial.println(DHT_hum);

  //sends data
  Blynk.run();
  Blynk.virtualWrite(1, DHT_temp);
  delay(500);
  Blynk.run();
  Blynk.virtualWrite(2, DHT_hum);

  if (isnan(DHT_hum) || isnan(DHT_temp))   // Check if any reads failed and exit early (to try again).
  {//resets values
    Serial.println("Failed to read from DHT sensor!");
    DHT_temp = tempIni;
    DHT_hum = humIni;
    return;
  }
}

/***************************************************
 * Get Soil Temperature Sensor data
 **************************************************/
float tempr()
{
      sensors.requestTemperatures();
      float temp = sensors.getTempCByIndex(0);
      Serial.println("Soil temperature is: ");
      Serial.println(temp);
      Blynk.virtualWrite(3, temp);
      
    
}

/***************************************************
 * Setup
 **************************************************/
void setup() {
  //put your setup code here, to run once:
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass, server,8080);
  Serial.setTimeout(2000);
  //Servo
  servo.attach(D5);
  servo.write(10);
  delay(1000);
  Serial.println("HoHoHo!"); 
  
}

/***************************************************
 * Blynk synchronization
 **************************************************/
BLYNK_CONNECTED() // runs every time Blynk connection is established
{
    if (isFirstConnect) 
    {
// Request server to re-send latest values for all pins
    Blynk.syncAll();
    isFirstConnect = false;
    }
    Blynk.syncAll();
}
 
/***************************************************
 * Blynk response to button
 **************************************************/
BLYNK_WRITE(V0) //Function to test status from BLYNK widget to PHOTON
{
    state = param.asInt(); 
    Serial.println("Button state is:");
    Serial.println(state);
     
    if (state == 1 && servo_state == 0)
    {
        Blynk.run();
        Serial.println("Watering!");
        Blynk.virtualWrite(V7, ON); // virtual LED
        servo.attach(D5);
        servo.write(179);
        delay(500);
        servo.detach();
        servo_state = 1;
        Blynk.syncVirtual(V7);
        Blynk.syncVirtual(V0);
    }
    if (state == 0 && servo_state == 1)
    {
        Blynk.run();
        Serial.println("Waiting!");
        Blynk.virtualWrite(V7, OFF); // virtual LED
        servo.attach(D5);
        servo.write(10);
        delay(500);
        servo.detach();
        servo_state = 0;
        Blynk.syncVirtual(V7);
        Blynk.syncVirtual(V0);
    }
}

/***************************************************
 * Running free
 **************************************************/
void loop() {
  // put your main code here, to run repeatedly:
  //get air humidity and temperature
  dht.begin();
  getDhtData();
 
  //get the soil temperature and send to the server
  tempr();
  Blynk.run();
}
