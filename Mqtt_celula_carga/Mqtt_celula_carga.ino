 #include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define POT A0

const char* ssid = "NoConnection";
const char* password =  "naoTemSenha";
const char* mqttServer = "tailor.cloudmqtt.com";
const int mqttPort = 16494;
const char* mqttUser = "fffdpzks";
const char* mqttPassword = "mJwNquIkLedX";
char msg[50];
int sinal = 0;
int entrada = analogRead(POT);

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {

  pinMode(A0, INPUT);

  Serial.begin(115200);

  WiFi.begin(ssid, password);
  
  Connection_Mqtt();
}

void Connection_Mqtt(){
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");

    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {

      Serial.println("connected");
    } 
    else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
  //client.subscribe("esp/test");
}
void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived in topic: ");
  Serial.println(topic);

  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }

  Serial.println();
  Serial.println("-----------------------");

}
void Print_Sinal(){
  
  Serial.print(analogRead(POT));
  Serial.print("     ");
  Serial.print(POT);
  Serial.print("     ");
  if (sinal >= 0) {
    
    if (analogRead(POT) >= 100 && analogRead(POT) <= 340 ) {
      pull("APERTO FRACO");
    }
    if (analogRead(POT) > 340 && analogRead(POT) <= 680) {
      pull("APERTO MEDIO");
    }
    if (analogRead(POT) > 680) {
      pull("APERTO FORTE");
    }
    if (analogRead(POT) < 100) {
      pull("BOTÃO NÃO APERTADO");
    }
    client.publish("ecg", msg);
    client.loop();
  }
}
void pull (char text[50]) {
  Serial.println(text);
  sprintf(msg, "%s", text);
  
}
void loop() {
  Print_Sinal();
}
