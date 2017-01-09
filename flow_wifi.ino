// Include the ESP8266 WiFi library. (Works a lot like the
// Arduino WiFi library.
#include <ESP8266WiFi.h>
// Include the SparkFun Phant library.
#include <Phant.h>

//////////////////////
// WiFi Definitions //
//////////////////////
const char WiFiSSID[] = "Guest@Rigel";
const char WiFiPSK[] = "Rigel6288";

/////////////////////
// Pin Definitions //
/////////////////////
#define LED_PIN LED_BUILTIN
#define hallsensor D3 // Wemos D0 D4 cannot use interrupt.

////////////////
// Phant Keys //
////////////////
const char PhantHost[] = "data.sparkfun.com";
const char PublicKey[] = "wpvZ9pE1qbFJAjaGd3bn";
const char PrivateKey[] = "wzeB1z0xWNt1YJX27xdg";


/////////////////////
// Global Variable //
/////////////////////

volatile float riseCount; //number of interrupts generated from Hallsensor.
float flowAmount; //calculate amount from rise count.
float flowSum; //sum of the flow duration.

long useCount = 1;
long flowDuration = 0;

/////////////////
// Post Timing //
/////////////////
const unsigned long postRate = 30000;
unsigned long lastPost = 0;


void setup() 
{
  initHardware();
  connectWiFi();
}

void loop() 
{
  riseCount = 0;//set RiseCount to 0

  sei();        //enable interrupt
  delay(1000);  //wait 1 second
  cli();        //disable interrupt
  
  flowAmount = (riseCount*2.1909);
  
  if(flowAmount!=0)
  {
    flowSum+=flowAmount;
    flowDuration++;
    //disp();
  }
  else if(flowSum!=0)
  {
    postToPhant();
    useCount++;
    flowDuration=0;
    flowSum=0;
  }
  /*
  if (lastPost + postRate <= millis())
  {
    if (postToPhant())
      lastPost = millis();
    else
      delay(100);    
  }
  */
}

void connectWiFi()
{
  byte ledStatus = HIGH;
  
  // Set WiFi mode to station (as opposed to AP or AP_STA)
  WiFi.mode(WIFI_STA);
  // WiFI.begin([ssid], [passkey]) initiates a WiFI connection
  // to the stated [ssid], using the [passkey] as a WPA, WPA2,
  // or WEP passphrase.
  WiFi.begin(WiFiSSID, WiFiPSK);
  
  // Use the WiFi.status() function to check if the ESP8266
  // is connected to a WiFi network.
  while (WiFi.status() != WL_CONNECTED)
  {
    // Blink the LED
    digitalWrite(LED_PIN, ledStatus); // Write LED high/low
    ledStatus = (ledStatus == HIGH) ? LOW : HIGH;
    
    // Delays allow the ESP8266 to perform critical tasks
    // defined outside of the sketch. These tasks include
    // setting up, and maintaining, a WiFi connection.
    delay(100);
    // Potentially infinite loops are generally dangerous.
    // Add delays -- allowing the processor to perform other
    // tasks -- wherever possible.
  }
}

void initHardware()
{
  Serial.begin(9600);
  //pinMode(DIGITAL_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  //initialise Hallsensor Pin as input, then attach interrupt
  pinMode(hallsensor, INPUT);
  attachInterrupt(hallsensor, countRise, RISING);  
  delay(100);
  // Don't need to set ANALOG_PIN as input, 
  // that's all it can be.
}

int postToPhant()
{
  // LED turns on when we enter, it'll go off when we 
  // successfully post.
  digitalWrite(LED_PIN, LOW);
  
  // Declare an object from the Phant library - phant
  Phant phant(PhantHost, PublicKey, PrivateKey);
  
  // Do a little work to get a unique-ish name. Append the
  // last two bytes of the MAC (HEX'd) to "Thing-":
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.macAddress(mac);
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                 String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();
  String postedID = "Jz_IoT_Thing-" + macID;
  
  // Add the four field/value pairs defined by our stream:
  phant.add("id", postedID);
  phant.add("analog", (int)flowSum);//analogRead(ANALOG_PIN));
  phant.add("digital", flowDuration);//digitalRead(DIGITAL_PIN));
  phant.add("time", millis());
  
  // Now connect to data.sparkfun.com, and post our data:
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(PhantHost, httpPort)) 
  {
    // If we fail to connect, return 0.
    return 0;
  }
  // If we successfully connected, print our Phant post:
  client.print(phant.post());
  
  // Read all the lines of the reply from server and print them to Serial
  /*
  while(client.available()){
    String line = client.readStringUntil('\r');
    //Serial.print(line); // Trying to avoid using serial
  }
  */
  
  // Before we exit, turn the LED off.
  digitalWrite(LED_PIN, HIGH);
  
  return 1; // Return success
}
