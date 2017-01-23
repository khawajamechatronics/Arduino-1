/*
 *   Using GPRS A6 development board, powewr comes from USB so no need to supply from Arduino
 *   RX/TX always connect to a hardware Serial port so we can utilize serialevent
 *   PWR always connected to Arduino VCC, RESET to TRANSISTOR_CONTROL
 *   
 *   Simple example publishes and subscribes to the same topic - see testTopic
 *   If using a public broker, choose a unique topic so you dont get swamped by other peoples stuff
 */

 /*
  * this exasmple does not ppublish any messages but ping keeps the connection alive,
  * To show what happens if no ping is sent , comment out the next line
  * After KEEP_ALIVE_TIME secconds there will be a disconnect  event followed by reconnect
  * You may send publish messages using the same parameters from another MQTT client and you will see the message
  * displayed on the serial terminal. However it doesnt stop the disconnect mechanism working
  */
#define KEEP_PINGING // comment out to demonstrate the effect of not ping and the connection going down
#include "A6Services.h"
#include "A6MQTT.h"

char buff[30];    // We must send at least 1 packet within the chosen keepalive time
                  // If you have nothing else to send, send at least a  ping
#define APN "uinternet"  // write your APN here
#define KEEP_ALIVE_TIME 30
A6MQTT MQTT(KEEP_ALIVE_TIME);

char imei[20];

void setup() {
  Serial.begin(115200);
   // A6 uses default baud 115200
   // power up the board, do hardware reset & get ready to execute commands
  if (gsm.begin()) 
  {
    Serial.println("GSM up");
    // we need a unique userid when logging on to the broker. We also need a unique topic
    // name. We'll use this devices IMIE tp get that.
    if (!gsm.getIMEI(imei))
      strcpy(imei,"defaultime");
    // setup GPRS connection with your provider
    if (gsm.startIP(APN))
    {
      Serial.println("IP up");
      // AutoConnect sets up TCP session with the broker and makes a user connection
      MQTT.AutoConnect();
    }
    else
      Serial.println("IP down");
  }
  else
    Serial.println("GSM down");
}

uint16_t messageid = 0;
void loop() {
  /*
   *  THe condition below checks if the modem is able to process data from the broker
   */
  if (MQTT.connectedToServer || MQTT.waitingforConnack)
  {
    /*
     * This machanism ensures that the connection with the broker is not disconnected du to inactivirt
     */
#ifdef KEEP_PINGING
    if (MQTT._PingNextMillis < millis())
      MQTT.ping();
#endif
    /*
     * Process data received from the broker
     * Note that this may be mixed up with unsolicited messages from the modem. Parse takes car of that
     */
    MQTT.Parse();
  }
}

