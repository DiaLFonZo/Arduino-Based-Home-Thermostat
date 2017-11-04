/***************************************************
  Adafruit ESP8266 Sensor Module
  
  Must use ESP8266 Arduino from:
    https://github.com/esp8266/Arduino
  Works great with Adafruit's Huzzah ESP board:
  ----> https://www.adafruit.com/product/2471
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!
  Written by Tony DiCola for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

// Libraries Temp
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "DHT.h"

// DHT 11 sensor
#define DHTPIN 0
#define DHTTYPE DHT11 

// WiFi parameters
#define WLAN_SSID       "DiaLFonZo_2.4"
#define WLAN_PASS       "macanantel"

// Adafruit IO
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "DiaLFonZo"
#define AIO_KEY         "bac7b089668d401094526564222370a5"

// Libraries OLED
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 3
Adafruit_SSD1306 display(OLED_RESET);
 
//#define LOGO16_GLCD_HEIGHT 16 
//#define LOGO16_GLCD_WIDTH  16 
 
#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

// DHT sensor
DHT dht(DHTPIN, DHTTYPE, 15);

// Functions
void connect();

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
 
// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
/****************************** Feeds ***************************************/

// Setup feeds for temperature & humidity
Adafruit_MQTT_Publish temperature = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temperature");
 
Adafruit_MQTT_Publish humidity = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/humidity");
/*************************** Sketch Code ************************************/

void setup() {

  // Init sensor
  dht.begin();

  Serial.begin(115200);
  Serial.println(F("Adafruit IO Example"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  delay(10);
  Serial.print(F("Connecting to "));
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();

  Serial.println(F("WiFi connected"));
  Serial.println(F("IP address: "));
  Serial.println(WiFi.localIP());

  // connect to adafruit io
  connect();

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  display.ssd1306_command(SSD1306_SETCONTRAST);
  display.ssd1306_command(1); // Where c is a value from 0 to 255 (sets contrast e.g. brightness)
  display.display();
  delay(2000);
  display.clearDisplay();
  
}

void loop() {

  // ping adafruit io a few times to make sure we remain connected
  if(! mqtt.ping(3)) {
    // reconnect to adafruit io
    if(! mqtt.connected())
      connect();
  }

  // Grab the current state of the sensor
  int humidity_data = (int)dht.readHumidity();
  int temperature_data = (int)dht.readTemperature();
  int ressenti_data = dht.computeHeatIndex(temperature_data, humidity_data);

  // Publish data
  if (! temperature.publish(temperature_data))
    Serial.println(F("Failed to publish temperature"));
  else
    Serial.println(F("Temperature published!"));

  if (! humidity.publish(humidity_data))
    Serial.println(F("Failed to publish humidity"));
  else
    Serial.println(F("Humidity published!"));
  
  // Update OLED values
  display.clearDisplay();
  display.setTextColor(WHITE); 
  display.setTextSize(5);
  display.setCursor(0,0);
  display.print(temperature_data);

  display.drawLine(0, 40, 128, 40, WHITE);
  display.drawLine(0, 42, 128, 42, WHITE);
  
  display.setTextSize(1);
  display.setCursor(70,0);
  display.print("Interieur");

  display.setTextSize(2);
  display.setCursor(65,20);
  display.print("Deg-C");

  display.setTextSize(1);
  display.setCursor(0,56);
  display.print("Ressentie:");
  display.setCursor(65,56);
  display.print(ressenti_data);
  display.setCursor(80,56);
  display.print("Deg-C");
  display.setCursor(0,46);
  display.print("Humidite:");
  display.setCursor(65,46);
  display.print(humidity_data);
  display.setCursor(80,46);
  display.print("%");

  display.display();

  // Repeat every 10 seconds
  delay(10000);

}

// connect to adafruit io via MQTT
void connect() {

  Serial.print(F("Connecting to Adafruit IO... "));

  int8_t ret;

  while ((ret = mqtt.connect()) != 0) {

    switch (ret) {
      case 1: Serial.println(F("Wrong protocol")); break;
      case 2: Serial.println(F("ID rejected")); break;
      case 3: Serial.println(F("Server unavail")); break;
      case 4: Serial.println(F("Bad user/pass")); break;
      case 5: Serial.println(F("Not authed")); break;
      case 6: Serial.println(F("Failed to subscribe")); break;
      default: Serial.println(F("Connection failed")); break;
    }

    if(ret >= 0)
      mqtt.disconnect();

    Serial.println(F("Retrying connection..."));
    delay(5000);

  }

  Serial.println(F("Adafruit IO Connected!"));

}
