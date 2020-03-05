#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <HX711.h>

const int PINO_DT = D6;
const int PINO_SCK = D7;

const int TEMPO_ESPERA = 100;
float fator_calibracao = -45000;

char comando;

const char* ssid = "ponto";
const char* password =  "senha888";
const char* mqttServer = "broker.hivemq.com";
const char* mqttUser = "fffdpzks";
const char* mqttPassword = "mJwNquIkLedX";
const int mqttPort = 1883;

char msg[50];

HX711 escala;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {

  Serial.begin(115200);

  WiFi.begin(ssid, password);

  escala.begin (PINO_DT, PINO_SCK);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");

  Connection_Mqtt();
  
  //client.subscribe("esp/test");
  Serial.println("Celula de carga - Calibracao de Peso");
  Serial.println("Posicione um peso conhecido sobre a celula ao comecar as leituras");
  float media_leitura = escala.read_average(); 
  Serial.print("Media de leituras com Celula sem carga: ");
  Serial.print(media_leitura);
  Serial.println();
  escala.tare();  
}

void Connection_Mqtt(){
    client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");

    if (client.connect("ESP8266Client")) {

      Serial.println("connected");
    }
    else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
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

void Print_Sinal() {
   escala.set_scale(fator_calibracao); //ajusta a escala para o fator de calibracao

  //verifica se o modulo esta pronto para realizar leituras
  if (escala.is_ready())
  {
    //mensagens de leitura no monitor serial
    Serial.print("Leitura: ");
    Serial.print(escala.get_units()); //retorna a leitura da variavel escala com a unidade quilogramas
    Serial.print(" kg");
    Serial.print(" \t Fator de Calibracao = ");
    Serial.print(fator_calibracao);
    Serial.println();
    pull(escala.get_units());

  //alteracao do fator de calibracao
//    if(Serial.available())
//      {
//        comando = Serial.read();
//        switch (comando)
//        {
//          case 'x':
//          fator_calibracao = fator_calibracao - 100;
//          break;
//          case 'c':
//          fator_calibracao = fator_calibracao + 100;
//          break;
//          case 'v':
//          fator_calibracao = fator_calibracao - 10;
//          break;
//          case 'b':
//          fator_calibracao = fator_calibracao + 10;
//          break;
//          case 'n':
//          fator_calibracao = fator_calibracao - 1;
//          break;
//          case 'm':
//          fator_calibracao = fator_calibracao + 1;
//          break;
//        }
//      }
    }
    else
    {
      Serial.print("HX-711 ocupado");
    }
  delay(TEMPO_ESPERA);
}
void pull (float text) {
  Serial.println(text);
  sprintf(msg, "%f", text);

}
void loop() {
  Print_Sinal();
  
  Connection_Mqtt();
  client.publish("ecg", msg);
  client.loop();
}
