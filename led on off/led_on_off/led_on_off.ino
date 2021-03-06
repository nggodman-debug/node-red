#include <WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.
const char* ssid = "Boon";
const char* password = "21345678";

// Config MQTT Server
#define mqtt_server "192.168.43.171"
#define mqtt_port 1883
#define mqtt_user "TEST"
#define mqtt_password "12345"

const int led1 = 2;

WiFiClient espClient;
PubSubClient client(espClient);

  void setup() {
  
  pinMode(led1, OUTPUT);

  Serial.begin(115200);
  delay(10);

  
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

  }

void loop() {
  if (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      client.subscribe("/ESP/LED");
      client.subscribe("/ESP/LED2");

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
      return;
    }
 
  }
  client.loop(); 

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
}
