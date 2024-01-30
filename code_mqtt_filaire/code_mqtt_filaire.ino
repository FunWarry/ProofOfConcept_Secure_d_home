#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
LiquidCrystal_I2C lcd(0x27,20,4);

//température et humidité
#include <dht11.h>
#define DHT11PIN 8
dht11 DHT11;

//lumière
float R;
float G;
float B;
byte lumi;
int tab_lum[6]={};
char check_hex[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

//ultrason
#include <Ultrasonic.h>
Ultrasonic ultrasonic(9, 10);
int presence = 1;
int distance = 0;
int mesure = 0;
int desactivation = 0;
int memoire_d_intrus = 0;
int message;
byte pin_buzz = 11;

// connection ethernet
const char* mqtt_server = "eseodp.cloud.shiftr.io";
byte mac[] = {0xA8, 0x61, 0x0A, 0xAE, 0x89, 0x43 }; // à compléter


// Callback function header
void callback(char* topic, byte* payload, unsigned int length);
EthernetClient ethClient;
PubSubClient client(ethClient);

unsigned long lastMsg = 0;

bool capteur = false;
byte Humidite = 0;
byte Temperature = 0;


String test;
String chemin;

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] : ");
  chemin = topic;
  message = 0 ;
  test = '\0';
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    if ((char)payload[i] != '#')
      test += String((char)payload[i]);
  }
  Serial.println();
  message = test.toInt();

  if (chemin == String("maison1/presence")){
    if (message == 1 ){
      presence = 1 ;
    } else {
      presence = 0 ;
    }
  }

  if (chemin == String("maison1/garage/temperature/setpoint")){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("temp set : ");
    lcd.print(message);
    lcd.write(223);
    lcd.print("C");
  }

  if (chemin == String("maison1/garage/lumiere/color/command")){
    for (int i = 0; i<6; i++){
      tab_lum[i] = (int)test[i];
    }
    Serial.println();
    R = (Strtoint(0)*16 + Strtoint(1));
    G = (Strtoint(2)*16 + Strtoint(3));
    B = (Strtoint(4)*16 + Strtoint(5));
    R = map(R,0,255,0,1023);
    G = map(G,0,255,0,1023);
    B = map(B,0,255,0,1023);
  }

  if (chemin == String("maison1/garage/lumiere/brightness/command")){
    lumi = message;
    Serial.print("lumi : ");
    Serial.println(lumi);
  }
  analogWrite(A1, R*lumi/100);
  analogWrite(A2, G*lumi/100);
  analogWrite(A3, B*lumi/100);
}

void setup() {
  lcd.init();
  lcd.backlight();
  pinMode(pin_buzz, OUTPUT);
  Serial.begin(115200);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  Ethernet.begin(mac);
  delay(500);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  distance = ultrasonic.read();
}

void reconnect() {// Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("Serveur", "eseodp", "eseoproj1")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");// Wait 5 seconds before retrying
      delay(5000);
    }
  }
  client.subscribe("maison1/garage/lumiere/color/command");
  client.subscribe("maison1/garage/lumiere/brightness/command");
  client.subscribe("maison1/garage/temperature/setpoint");
  client.subscribe("maison1/presence");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  distance = ultrasonic.read();
  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    if (presence == 0) { //ultrason
      memoire_d_intrusion = UltraPres(desactivation, memoire_d_intrusion, distance);
      desactivation = 0 ;
    } else {
      client.publish("maison1/garage/presence", String(0).c_str());
      desactivation = 1 ;
      memoire_d_intrusion = 0 ;
    }
    DHT11.read(DHT11PIN);
    Humidite = (int)DHT11.humidity;
    Temperature = (int)DHT11.temperature-5;
    client.publish("maison1/garage/temperature", String(Temperature).c_str());
    client.publish("maison1/garage/humidite", String(Humidite).c_str());
    loop();
  }
}