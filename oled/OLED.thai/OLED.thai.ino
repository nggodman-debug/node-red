#include <Wire.h>
#include "dw_font.h"
#include "SSD1306.h"

#include <PubSubClient.h>
#include <WiFi.h>

const char* ssid = "iJapan";
const char* password = "passwell";

#define mqtt_server "192.168.43.98"
#define mqtt_port 1883
#define mqtt_user "TEST"
#define mqtt_password "12345"

WiFiClient espClient;
PubSubClient client(espClient);

SSD1306   display(0x3c, 21, 22);

extern dw_font_info_t font_th_sarabunpsk_regular40;
dw_font_t myfont;

void draw_pixel(int16_t x, int16_t y)
{
  display.setColor(WHITE);
  display.setPixel(x, y);
}

void clear_pixel(int16_t x, int16_t y)
{
  display.setColor(BLACK);
  display.setPixel(x, y);
}


void runmqtt() {

  if (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
//      client.subscribe("led");
//      client.subscribe("run");
      client.subscribe("oled");


    } else {
      Serial.print("failed, rc = ");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
      return;
    }
  }

}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String msg = "";
  int i = 0;
  while (i < length) msg += (char)payload[i++];

  Serial.println(msg);
  //  client.publish("temp","asdfa");
//  if (msg == "on") {
//    digitalWrite(LED_PIN, HIGH);
//  }
//  if (msg == "off") {
//    digitalWrite(LED_PIN, LOW);
//  }
//  if (isNumeric(msg)) {
//    ledcWrite(channel, msg.toInt());
//  }

  if (not strcmp(topic, "oled")) {
    Serial.println(msg);

      display.clear();
      dw_font_setfont(&myfont, &font_th_sarabunpsk_regular40);
      dw_font_goto(&myfont, 10, 40);
      dw_font_print(&myfont,(char*) msg.c_str());
      display.display();
    
//    display.clearDisplay();
//    display.setTextSize(2);
//    display.setTextColor(WHITE);
//    display.setCursor(0, 0);
//    display.print(msg);
//    display.display();
  }



}




void setup() {
  // put your setup code here, to run once:
  uint16_t width = 0;
 
  Serial.begin(115200);
  
  display.init();
  display.flipScreenVertically();

  dw_font_init(&myfont,
               128,
               64,
               draw_pixel,
               clear_pixel);

  dw_font_setfont(&myfont, &font_th_sarabunpsk_regular40);
  dw_font_goto(&myfont, 10, 40);
//    dw_font_goto(&myfont, 10, 60);
  dw_font_print(&myfont, "สวัสดีAB");
  display.display();


  
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  // put your main code here, to run repeatedly:

    runmqtt();
  client.loop();

}
