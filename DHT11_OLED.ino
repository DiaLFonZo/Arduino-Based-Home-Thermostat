// FONT https://github.com/olikraus/u8glib/wiki/fontsize


#include "DHT.h"
#define DHTPIN 2     // what digital pin we're connected to
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);

#include "U8glib.h"
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0);  // I2C / TWI 

float h;
float t;
float hic;
int Tint;
  
void setup() {
  Serial.begin(9600);
  Serial.println("DHTxx test!");
  dht.begin();
  
}

void loop() {
delay(1000);
sensor_update();
draw();

}

void sensor_update(void) {
// Get sensor values
  h = dht.readHumidity();
  t = dht.readTemperature();
  hic = dht.computeHeatIndex(t, h); 
  Tint = t;
}

void draw(void) {
// Draw the values to the OLED
  u8g.firstPage();  
  do {
    u8g.setFont(u8g_font_fub30); // fub25 25 pixels - stock was unifont at 10 pixels
    u8g.setPrintPos(0, 31); 
    u8g.print(Tint);
    
    u8g.setFont(u8g_font_7x14B);
    u8g.setPrintPos(0, 50); 
    u8g.print(h);
    u8g.setPrintPos(0, 64); 
    u8g.print(hic);

    u8g.setFont(u8g_font_7x14B);
    u8g.setPrintPos(60, 10); 
    u8g.print("INTÉRIEUR"); 
    u8g.setPrintPos(48, 31); 
    u8g.print("°C");
    u8g.setPrintPos(40, 50); 
    u8g.print(" %");
    u8g.setPrintPos(65, 50); 
    u8g.print("Humidité");
    u8g.setPrintPos(40, 64); 
    u8g.print("°C");
    u8g.setPrintPos(65, 64); 
    u8g.print("Ressentie");
    
  } while( u8g.nextPage() );
}

void draw_static(void) {
// Draw the values to the OLED
  u8g.firstPage();  
  do {

  } while( u8g.nextPage() );
}
