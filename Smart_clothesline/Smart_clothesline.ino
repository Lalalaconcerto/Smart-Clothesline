#include <SPI.h>
#include <ezTime.h>
#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>
#include "arduino_secrets.h"
#define PIN 13
#define rainfall A0

//Rain
int rainvalue;
int Raindrop;
int rainmqtt;
boolean raindetected = false;
//Motor
int in1 = 12;
int in2 = 7;
// Two switches
const int switchPin1 = 9;
const int switchPin2 = 10;
int switchState1 = 0;
int switchState2 = 0;
//led
int NUM_LEDS = 8;
// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

//Mqtt and Wifi
int status = WL_IDLE_STATUS;     // the WiFi radio's status
const char* ssid     = SECRET_SSID;
const char* password = SECRET_PASS;
const char* mqttuser = SECRET_MQTTUSER;
const char* mqttpass = SECRET_MQTTPASS;
const char* mqtt_server = "MQTT SERVER ADDRESS";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
Timezone GB;

void setup() {
  //led strip
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  Serial.begin(9600);
  delay(100);// to give time for the serial connection to open
  //mqtt and time
  startWifi();
  syncDate();
  client.setServer(mqtt_server, 1884);
  client.setCallback(callback);
  //rain
  pinMode(rainfall, INPUT);
  //motor
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  //Two switches
  pinMode(switchPin1, INPUT);
  pinMode(switchPin2, INPUT);
}


void loop() {
  rainvalue = analogRead(rainfall);
  rainmqtt = map(rainvalue, 1023, 0, 0, 1000);// Modify raindrop value shown on mqtt server,make it better to understand.
  Serial.println(rainmqtt);

  // Swtiches to control the motor to spin clockwise or counterclockwise.
  switchState1 = digitalRead(switchPin1);
  switchState2 = digitalRead(switchPin2);

  if (switchState1 == HIGH) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    Serial.println("switch1 ");
    delay(80);
  }
  else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    delay(200);
  }

  if (switchState2 == HIGH) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    Serial.println("switch2");
    delay(80);
  }
  else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    Serial.println("switch23");
    delay(200);
  }

  //Rain--led_motor_piezo loop
  if (rainvalue > 900) {//When there is no rain
    setAll(0, 0, 0);
  }

  if (rainvalue <= 900) {// When it rains ,rainfall is classified into three levels.
    Serial.println("rain detected");

    if (rainvalue > 600 && rainvalue <= 900) {//light rain,set LEDs to blue
      CylonBounce(0, 0, 255, 1, 100, 50);
    }

    else if (rainvalue > 300 && rainvalue <= 600) {//middle rain,set LEDs to yellow
      CylonBounce(255, 255, 0, 1, 100, 50);
    }
    else if (rainvalue <= 300 && rainvalue > 0) {//heavy rain,set LEDs to red
      CylonBounce(255, 0, 0, 1, 100, 50);
    }

    if (!raindetected) {
      int pitch = map(rainvalue, 0, 1023, 50, 4000);
      tone(8, 4000 - pitch, 100); // Let the frequency of the buzzer be positively correlated with rainfall
      delay(10);

      digitalWrite(in1, HIGH); // Motor spin clockwise to collect clothes
      digitalWrite(in2, LOW);
      delay(80);
      keepoff(); delay(1000); //Motor stops
      raindetected = true;

    }
  }

  else {

    tone(0, 0, 0);//set piezo off
    delay(50);
    if (raindetected) {
      digitalWrite(in1, LOW);// Motor spin counterclockwise to dry clothes
      digitalWrite(in2, HIGH);
      delay(80);
      keepoff(); delay(1000);
      raindetected = false;

      setAll(0, 0, 0);//set LEDs off
      showStrip();
      delay(200);
    }
  }


  //  mqtt
  //Set the frequency of reading data;
  //Send data to MQTT server
  if (secondChanged()) {
    readRain();
    sendMQTT();
    Serial.println(GB.dateTime("H:i:s")); // UTC.dateTime("l, d-M-y H:i:s.v T")
  }

  client.loop();

}


void keepoff() {//Stop motor from working
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);

}
//Animations and effects of the LED strips
void CylonBounce(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  int rainvaluemap = map(rainvalue, 1023 , 0 , 3, 8) ;
  NUM_LEDS = rainvaluemap;

  for (int i = 0; i < NUM_LEDS - EyeSize - 2; i++) { //let LEDs that are on move to one side of the LED strip
    setAll(0, 0, 0);
    setPixel(i, red / 10, green / 10, blue / 10);//The two LEDs around the LED that is on are dimed by changing their RGB values
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(i + j, red, green, blue);
    }
    setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);//The two LEDs around the LED that is on are dimed by changing their RGB values
    showStrip();
    delay(SpeedDelay);
  }

  delay(ReturnDelay);

  for (int i = NUM_LEDS - EyeSize - 2; i > 0; i--) {//let LEDs that are on move back to the other side of the LED strip
    setAll(0, 0, 0);
    setPixel(i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(i + j, red, green, blue);
    }
    setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
    showStrip();
    delay(SpeedDelay);
  }

  delay(ReturnDelay);
}

void showStrip() {
#ifdef ADAFRUIT_NEOPIXEL_H
  // NeoPixel
  strip.show();
#endif
#ifndef ADAFRUIT_NEOPIXEL_H
  // FastLED
  FastLED.show();
#endif
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
#ifdef ADAFRUIT_NEOPIXEL_H
  // NeoPixel
  strip.setPixelColor(Pixel, strip.Color(red, green, blue));
#endif
}

void setAll(byte red, byte green, byte blue) {
  for (int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue);
  }
  showStrip();
}


void readRain() {//Read rainfall data and map it
  // delay to slow down and enable the current to stabilise
  delay(100);
  rainvalue = analogRead(rainfall);
  rainmqtt = map(rainvalue, 1023, 0, 0, 1000);
  Serial.println(rainmqtt);   // Print data to the serial

}

void startWifi() {
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  // check to see if connected and wait until you are
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// This function is used to make sure the arduino is connected
// to an MQTT broker before it tries to send a message and to
// keep alive subscriptions on the broker (ie listens for inTopic)


void syncDate() {
  // get real date and time
  waitForSync();
  Serial.println("UTC: " + UTC.dateTime());
  GB.setLocation("Europe/London");
  Serial.println("London time: " + GB.dateTime());

}


// Publish topic on MQTT server
void sendMQTT() {
  // try to reconnect if not
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  rainmqtt = map(rainvalue, 1023, 0, 0, 1000);
  Serial.println(rainmqtt);
  snprintf (msg, 50, "%.0i", rainmqtt);
  Serial.print("Publish message for R: ");
  Serial.println(msg);
  client.publish("student/CASA0016/ucfnxla/Raindrop", msg);//Publish raindrop data on MQTT server

  if (rainvalue < 900) {
    Serial.print("Rain detected");
    client.publish("student/CASA0016/ucfnxla/Raincondition", "Rain detected");//Publish raining state:"Rain detected"
  }
  else if (rainvalue >= 900) {
    Serial.print("No rain");
    client.publish("student/CASA0016/ucfnxla/Raincondition", "No rain");//Publish raining state:"No rain"
  }


}
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {    // while not (!) connected....
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);

    // Attempt to connect
    if (client.connect(clientId.c_str(), mqttuser, mqttpass)) {
      Serial.println("connected");
      // ... and subscribe to messages on broker
      client.subscribe("student/CASA0016/inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


// Execute code when a message is recieved from the MQTT
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

}
