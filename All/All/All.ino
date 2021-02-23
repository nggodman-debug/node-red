
#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Update these with values suitable for your network.
const char* ssid = "Boon";
const char* password = "21345678";

// Config MQTT Server
#define mqtt_server "192.168.43.171"
#define mqtt_port 1883
#define mqtt_user "TEST"
#define mqtt_password "12345"

#define DHTTYPE DHT22
const int DHTPin = 16;
DHT dht(DHTPin, DHTTYPE);

static char celsiusTemp[7];
static char fahrenheitTemp[7];
static char humidityTemp[7];

/*OLED*/
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET 4
Adafruit_SSD1306 display(SCREEN_WIDTH,SCREEN_HEIGHT, &Wire, OLED_RESET);


const int led1 = 2;
const int led2 = 4;

const int freq = 2000;
const int ledChannel = 0;
const int resolution = 8;

const int buttonPin = 12;
const int ledPin =  13;
int buttonState = 0;

WiFiClient espClient;
PubSubClient client(espClient);
String inputString = "";

boolean isNumeric(String str) {
    unsigned int stringLength = str.length();
 
    if (stringLength == 0) {
        return false;
    }
 
    boolean seenDecimal = false;
 
    for(unsigned int i = 0; i < stringLength; ++i) {
        if (isDigit(str.charAt(i))) {
            continue;
        }
 
        if (str.charAt(i) == '.') {
            if (seenDecimal) {
                return false;
            }
            seenDecimal = true;
            continue;
        }
        return false;
    }
    return true;
}
  unsigned long currentTime = millis();
  const long delayTime = 2000;

  unsigned long previousMillis =0;
  const long interval =5000;
  
  void setup() {
  dht.begin();
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  
  Serial.begin(115200);
  delay(10);

  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(led2, ledChannel);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

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

    Serial.begin(115200);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)){
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextColor(WHITE);

}

void loop() {
  if (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      client.subscribe("/ESP/LED");
      client.subscribe("/ESP/LED2");
      client.subscribe("OLED");
      client.subscribe("BTN");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
      return;
    }
 
  }
  client.loop(); 
  dhtread();


 int newst = digitalRead(buttonPin);
  if (buttonState != newst) {
    // turn LED on
    digitalWrite(ledPin, newst);

    client.publish("BTN", newst? "ON":"OFF");
    
    Serial.println(newst);
    buttonState = newst;
  }
  
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String msg = "";
  int i=0;
  while (i<length) msg += (char)payload[i++];
  Serial.print("msg = ");
  Serial.println(msg);
  if (msg == "GET") {
    client.publish("/ESP/LED", (digitalRead(led1) ? "LEDON" : "LEDOFF"));
    Serial.println("Send !");
    return;
  } 
  if(msg == "on"){
    digitalWrite(led1,HIGH);
  }
    if(msg == "off"){
    digitalWrite(led1,LOW);
  }
 if(isNumeric(msg)){
    int dutyCycle = msg.toInt();
    ledcWrite(ledChannel,dutyCycle);
    Serial.print(dutyCycle);
  }

  //OLED
  if(not strcmp(topic, "OLED")){
    Serial.println(msg);
      display.clearDisplay();
      display.setTextSize(1.75);
      display.setTextColor(WHITE);
      display.setCursor(60,30);
      display.print(msg);
      display.display();
  }
  
  
}

void dhtread() {

  if (millis() - currentTime > delayTime) {
    currentTime = millis();

    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = dht.readTemperature(true);
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f)) {
      Serial.println("Failed to read from DHT sensor!");
      strcpy(celsiusTemp, "Failed");
      strcpy(fahrenheitTemp, "Failed");
      strcpy(humidityTemp, "Failed");
    }
    else {
      // Computes temperature values in Celsius + Fahrenheit and Humidity
      float hic = dht.computeHeatIndex(t, h, false);
      dtostrf(hic, 6, 2, celsiusTemp);
      float hif = dht.computeHeatIndex(f, h);
      dtostrf(hif, 6, 2, fahrenheitTemp);
      dtostrf(h, 6, 2, humidityTemp);
      // You can delete the following Serial.prints, it s just for debugging purposes
    }
    client.publish("room/temperature", celsiusTemp);
    client.publish("room/fahrenheit", fahrenheitTemp);
    client.publish("room/humidity", humidityTemp);

    
  }
}
