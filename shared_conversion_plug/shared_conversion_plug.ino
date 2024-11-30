/*
 * --------------------------------------------------------------------------------------------------------------------
 * Example showing how to read data from a PICC to serial.
 * --------------------------------------------------------------------------------------------------------------------
 * This is a MFRC522 library example; for further details and other examples see: https://github.com/OSSLibraries/Arduino_MFRC522v2
 * 
 * Example sketch/program showing how to read data from a PICC (that is: a RFID Tag or Card) using a MFRC522 based RFID
 * Reader on the Arduino SPI interface.
 * 
 * When the Arduino and the MFRC522 module are connected (see the pin layout below), load this sketch into Arduino IDE
 * then verify/compile and upload it. To see the output: use Tools, Serial Monitor of the IDE (hit Ctrl+Shft+M). When
 * you present a PICC (that is: a RFID Tag or Card) at reading distance of the MFRC522 Reader/PCD, the serial output
 * will show the ID/UID, type and any data blocks it can read. Note: you may see "Timeout in communication" messages
 * when removing the PICC from reading distance too early.
 * 
 * If your reader supports it, this sketch/program will read all the PICCs presented (that is: multiple tag reading).
 * So if you stack two or more PICCs on top of each other and present them to the reader, it will first output all
 * details of the first and then the next PICC. Note that this may take some time as all data blocks are dumped, so
 * keep the PICCs at reading distance until complete.
 * 
 * @license Released into the public domain.
 * 
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 *
 * Not found? For more see: https://github.com/OSSLibraries/Arduino_MFRC522v2#pin-layout
 */

// we need to reserve others contribution!
/*  Title: MQTT Publish
    This sketch publishes a message onto an MQTT topic
    Dr C.K. Leung (c) Oct 2024
*/


/* A struct used for passing the UID of a PICC.
typedef struct {
  byte		size;			// Number of bytes in the UID. 4, 7 or 10.
  byte		uidByte[10];
  byte		sak;			// The SAK (Select acknowledge) byte returned from the PICC after successful selection.
} Uid;
*/

/*
For my test result:
  the white card UID is: 7378AE2
  the circle card UID is: 34E7213
*/

#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522Debug.h>

//Include the MQTT client library
#include <ArduinoMqttClient.h>
//Include WiFi library
#include <WiFiNINA.h>//we also need this header to access RGB LED
//Timer for blinking led
#include <TickTwo.h>

//for rgb led control
typedef enum {
  RED   = 0,
  GREEN = 1,
  BLUE  = 2,
} ledColor;
ledColor rgbled{BLUE};

// for relay control
#define PIN_RELAY     (3u)

/*MFRC522 and SPI configuration*/
MFRC522DriverPinSimple ss_pin(10); // Create pin driver. See typical pin layout above.
SPIClass &spiClass = SPI; // Alternative SPI e.g. SPI2 or from library e.g. softwarespi.
MFRC522DriverSPI driver{ss_pin, spiClass}; // Create SPI driver.
MFRC522 mfrc522{driver}; // Create MFRC522 instance.

/* MQTT application */
// Connect to WiFi Router
char ssid[] = "EIE568-R";   // WiFi network SSID (name) for EIE568 Lab
char pass[] = "12345678";    // WiFi network password (use for WPA, or use as key for WEP)

// OCH wifi
// char ssid[] = "TP-Link_098C";   // WiFi network SSID (name) for EIE568 Lab
// char pass[] = "69512964";    // WiFi network password (use for WPA, or use as key for WEP)

//Create objects
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);
//Define the MQTT server to be connected to
const char server[] = "test.mosquitto.org";
//Define the standard port for TCP/IP communication
int port = 1883;
//Define the "Topic", for pub
const char pub_topic[]  = "PLUG/Msg";
//Define the fixed message to be published 
String Message = "52:Hello"; //

//Subscribe MQTT, for sub
const char sub_topic[]  = "PLUG/Cmd";
// set interval for polling server/server (milliseconds) for keep-alive
const long poll_interval = 5000;  //set polling interval at 5 sec
unsigned long previousMillis = 0; // time last time the loop is executed
unsigned long currentMillis = 0;  // current time when the loop is executed


// for led blink test
void blink() {
  static bool ledState = false;
  // flip 
  ledState = !ledState;
  switch(rgbled) {
      case RED:
        {
          digitalWrite(LEDB, LOW);
          digitalWrite(LEDG, LOW);
          if (ledState) {
            digitalWrite(LEDR, HIGH);
          } else {
            digitalWrite(LEDR, LOW);
          }          
        }
        break;
      case GREEN:
        {
          digitalWrite(LEDR, LOW);
          digitalWrite(LEDB, LOW);
          if (ledState) {
            digitalWrite(LEDG, HIGH);
          } else {
            digitalWrite(LEDG, LOW);
          }          
        }
        break;
      case BLUE:
        {
          digitalWrite(LEDR, LOW);
          digitalWrite(LEDG, LOW);
          if (ledState) {
            digitalWrite(LEDB, HIGH);
          } else {
            digitalWrite(LEDB, LOW);
          }          
        }
        break;
      default:
        // disable all led
        digitalWrite(LEDR, LOW);
        digitalWrite(LEDG, LOW);
        digitalWrite(LEDB, LOW);
  }

}
TickTwo timer1(blink, 100); // change led every 100ms

void setup() {

  /*RGB LED configuration*/
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
  //default to blink blue
  rgbled = BLUE;

  //relay pin, default is disconnect
  pinMode(PIN_RELAY, OUTPUT);
  digitalWrite(PIN_RELAY, LOW);


  Serial.begin(9600); // Initialize serial communications with the PC for debugging.
  while (!Serial);      // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4).
  mfrc522.PCD_Init();   // Init MFRC522 board.

  /* attempt to connect to Wi-Fi network:*/
  Serial.print("Connecting to WPA SSID: ");
  Serial.print(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }
  Serial.println();
  Serial.print("Connected successfully to WPA SSID: ");
  Serial.println(ssid);
  Serial.println();

  /*attempt to connect to MQTT server network:*/
  Serial.print("Connecting to the MQTT server: ");
  Serial.println(server);

  if (!mqttClient.connect(server, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    while (1); //endless loop here
  }

  Serial.print("Connected successfully to the MQTT server: ");
  Serial.println(server);
  Serial.println();
  /*end of mqtt connect*/


  /*mqtt topic subscribe*/
  mqttClient.onMessage(onMqttMessage);

  Serial.print("Subscribing to topic: '");
  Serial.print(sub_topic);
  Serial.println("'");
  Serial.println();

  // subscribe to a topic
  // the second parameter sets the QoS of the subscription,
  // the the library supports subscribing at QoS 0, 1, or 2
  int subscribeQos = 1;

  mqttClient.subscribe(sub_topic,subscribeQos);

  Serial.print("Waiting for messages on topic: '");
  Serial.print(sub_topic);
  Serial.println("' ...");
  /*end of mqtt subscribe*/


  /*start timer1*/
  timer1.start();
}

String printUIDtoSerial() {
  String hexString = "";  
  Serial.print("CARD UID: \n");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    // Serial.print(mfrc522.uid.uidByte[i], HEX);
    if (mfrc522.uid.uidByte[i] < 0x10) {
      hexString += "0"; // 如果字节值小于16，前面补0
    }
    hexString += String(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.print(hexString);
  Serial.print("\n");
  return hexString;
}

void loop() {
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    String UIDString = printUIDtoSerial();

    // Publish message
    Message = UIDString;
    mqttClient.beginMessage(pub_topic);
    mqttClient.print(Message);
    mqttClient.endMessage();
    delay(100);
  }

  timer1.update();

  // call poll() regularly to allow the library to receive MQTT messages and
  // send MQTT keep alive which avoids being disconnected by the server
  // todo:may be it is better to put this in a timer
   mqttClient.poll();
   delay(50);
}

void onMqttMessage(int messageSize) {
// Process subscribed message as it arrives

  String Topic_string = "";
  String Message_string = "";
  
  Topic_string=mqttClient.messageTopic();
  Serial.print("Received message on topic '");
  Serial.print(Topic_string);
  Serial.println("'");

    // Gather the received message character by charater into a string
  for (int i = 0; i < messageSize; i++) {
    Message_string=Message_string+(char)mqttClient.read();
  }

  Serial.print("Received message is '");
  Serial.print(Message_string);
  Serial.println("'");
  Serial.println();

  // Using YES and NO for comand, to blink different led
  if (Message_string == "YES") {
    // green led blinks
    rgbled = GREEN;
    // resume the circuit
    digitalWrite(PIN_RELAY, HIGH);
  } else if (Message_string == "NO") {
    // read led blinks
    rgbled = RED;
    // cut off the circuit
    digitalWrite(PIN_RELAY, LOW);
  } else {
    // blue led blinks
    rgbled = BLUE;
    // disconnect the circuit
    digitalWrite(PIN_RELAY, LOW);
  }
}



