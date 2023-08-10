#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <time.h>
#include "secrets.h"

#define TIME_ZONE -5
#define LED 2

// Variables globales
const char* tiempo = "tiempo";
int presionSistolica = 0;
int presionDiastolica = 0;
char unit[] = "mmHg";
char notes[] = "TEST";

// Variables de tiempo
unsigned long lastMillis = 0;
unsigned long previousMillis = 0;
const long interval = 5000;
time_t now;
time_t nowish = 1510592825;

// Se definen los topic para publicar y recibir mensajes
#define AWS_IOT_PUBLISH_TOPIC   "esp8266/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp8266/sub"

// Configuración de certificados y claves para la conexión segura
WiFiClientSecure net;
 
BearSSL::X509List cert(cacert);
BearSSL::X509List client_crt(client_cert);
BearSSL::PrivateKey key(privkey);

// Se crea un objeto de cliente MQTT utilizando la conexión segura  
PubSubClient client(net);
 

// Función se encarga de establecer la hora del dispositivo utilizando el servicio NTP para sincronizarla con la hora del servidor
void NTPConnect(void)
{
  Serial.print("Configuración de la hora mediante SNTP");
  // Configuranción del tiempo
  configTime(TIME_ZONE * 3600, 0 * 3600, "pool.ntp.org", "time.nist.gov");
  now = time(nullptr);
  while (now < nowish)
  {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("Hecho!");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Tiempo actual: ");
  Serial.print(asctime(&timeinfo));
}

 
// Función que actualiza la fecha y hora en la variable tiempo
void TimeNTP(void)
{  
  now = time(nullptr);
  while (now < nowish)
  {
    delay(500);
    now = time(nullptr);
  }
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  tiempo = asctime(&timeinfo);
}


// Función que se llama cuando recibe un mensaje MQTT y muestra el contenido por el puerto
void messageReceived(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Recibido [");
  Serial.print(topic);
  Serial.print("]: ");
  for (unsigned int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  
  //prende el led por 2 segundos
  digitalWrite(LED, HIGH);
  delay(2000);
  digitalWrite(LED, LOW);  
}
 
 
// Función que maneja la conexión al servicio AWS IoT Core utilizando las credenciales y configuraciones
void connectAWS()
{
  delay(3000);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
 
  Serial.println(String("Intentando conectarse al SSID: ") + String(WIFI_SSID));
 
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }
 
  NTPConnect();
 
  net.setTrustAnchors(&cert);
  net.setClientRSACert(&client_crt, &key);
 
  client.setServer(MQTT_HOST, 8883);
  client.setCallback(messageReceived);
 
 
  Serial.println("Conexión a AWS IOT CORE");
 
  while (!client.connect(THINGNAME))
  {
    Serial.print(".");
    delay(1000);
  }
 
  if (!client.connected()) {
    Serial.println("Tiempo de espera de AWS IoT!");
    return;
  }
  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
 
  Serial.println("AWS IoT Conectado!");
}
 
 
// Función que crea un mensaje JSON y lo publica en el tópico MQTT 
void publishMessage()
{
  //Crear el JSON del mensaje
  StaticJsonDocument<200> doc;
  doc["timestamp"] = tiempo;
  doc["presionSistolica"] = presionSistolica;
  doc["presionDiastolica"] = presionDiastolica;
  doc["unit"] = unit;
  doc["notes"] = notes;

  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client
 
  //publicar el mensaje en el tópico MQTT
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}


// Establece conexión con el servidor AWS 
void setup() {

  //Abre el puerto serie y coloca la taza dde transferencia a spped bits por segundo
  Serial.begin(115200);  //velocidad

  pinMode(LED, OUTPUT);

  connectAWS();

  Serial.println("");
  //Escribimos por el monitor serie mensaje de inicio
  Serial.println("Inicio de sketch - valores de la presión arterial");
  Serial.println("");
}


// Bucle principal
void loop() {

  // leemos del pin A0 valor
  presionSistolica = analogRead(A0);
  presionDiastolica = presionSistolica - 40;
  TimeNTP(); // se actualiza fecha y hora

  // se imprimen los datos que se envian al servidor AWS
  Serial.print("Timestamp: ");
  Serial.print(tiempo);
  Serial.print("Presión Sistólica: ");
  Serial.println(presionSistolica);
  Serial.print("Presión Diastólica: ");
  Serial.println(presionDiastolica);
  //Serial.print("Unit: ");
  //Serial.println(unit);
  //Serial.print("Notes: ");
  //Serial.println(notes);
  //Serial.println("");
  
  //delay(2000);

  now = time(nullptr);

  if (!client.connected())
  {
    connectAWS();
  }
  else
  {
    client.loop();
    if (millis() - lastMillis > 5000)
    {
      lastMillis = millis();
      publishMessage();
    }
  }
  delay(3000);// Esperar 30 segundos antes de enviar el siguiente mensaje
}