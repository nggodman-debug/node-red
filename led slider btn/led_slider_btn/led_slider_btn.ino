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


const int led2 = 4;
const int ledPin =  13;

const int freq = 2000;
const int ledChannel = 0;
const int resolution = 8;

int buttonState = 0;

WiFiClient espClient;
PubSubClient client(espClient);


void setup() {
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

}

}

void loop() {
  if (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      client.subscribe("/ESP/LED2");
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
}

boolean isNumeric(String str) {
  unsigned int stringLength = str.length();

  if (stringLength == 0) {
    return false;
  }

  boolean seenDecimal = false;

  for (unsigned int i = 0; i < stringLength; ++i) {
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
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String msg = "";
  int i = 0;
  while (i < length) msg += (char)payload[i++];
  if (msg == "GET") {
    client.publish("BTN", (digitalRead(ledPin) ? "ledon" : "ledoff"));
    Serial.println("Send !");
    return;
  }
  if (msg == "on") {
    client.publish("BTN", "state-on");
    digitalWrite(ledPin, HIGH);
    Serial.println("Send on !");
    return;
  }
  if (msg == "off") {
    client.publish("BTN", "state-off");
    digitalWrite(ledPin, LOW);
    Serial.println("Send off !");
    return;
  }

  Serial.println(topic);
  if (isNumeric(msg)  ) {
    
    ledcWrite(ledChannel, msg.toInt());
    Serial.println("LED = " + msg);
    return;
  }
