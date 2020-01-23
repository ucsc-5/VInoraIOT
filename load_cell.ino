#include <HX711_ADC.h>
#include <EEPROM.h>

#include <FirebaseESP8266.h>
#include <FirebaseESP8266HTTPClient.h>
#include <FirebaseJson.h>
#include <jsmn.h>

#include <ESP8266WiFi.h>


#define FIREBASE_HOST "https://vinora-dc8a2.firebaseio.com"
#define FIREBASE_AUTH "omc2oXAPjM8atviirWIB0anBOh61PevthTv7EzFe"
#define WIFI_SSID "chamod"
#define WIFI_PASSWORD "123456789"

//HX711 constructor (dout pin, sck pin):
HX711_ADC LoadCell(5, 4);

const int eepromAdress = 0;

long t;
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////



// Set these to run example.

String myString;

FirebaseData firebaseData;


/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
void setup_LoadCell() {
  /////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////

  

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    //Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.println("connected: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  //5. Optional, set AP reconnection in setup()
  Firebase.reconnectWiFi(true);

  //6. Optional, set number of error retry
  Firebase.setMaxRetry(firebaseData, 3);

  //7. Optional, set number of error resumable queues
  Firebase.setMaxErrorQueue(firebaseData, 30);

  //8. Optional, use classic HTTP GET and POST requests.
  Firebase.enableClassicRequest(firebaseData, true);


  //////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////

  float calValue; // calibration value
  // calValue = 696.0; // uncomment this if you want to set this value in the sketch
  calValue = 48100.0; // uncomment this if you want to set this value in the sketch
#if defined(ESP8266)
  //EEPROM.begin(512); // uncomment this if you use ESP8266 and want to fetch the value from eeprom
#endif
  //EEPROM.get(eepromAdress, calValue); // uncomment this if you want to fetch the value from eeprom


  Serial.println();
  Serial.println("Starting...");
  LoadCell.begin();
  long stabilisingtime = 2000; // tare preciscion can be improved by adding a few seconds of stabilising time
  LoadCell.start(stabilisingtime);
  LoadCell.setCalFactor(calValue);
  if (LoadCell.getTareTimeoutFlag()) {
    Serial.println("Tare timeout, check MCU&gt;HX711 wiring and pin designations");
  }
  else {
    LoadCell.setCalFactor(calValue); // set calibration value (float)
    Serial.println("Startup + tare is complete");
  }
}

void loop_LoadCell() {

  //update() should be called at least as often as HX711 sample rate; &gt;10Hz@10SPS, &gt;80Hz@80SPS
  //use of delay in sketch will reduce effective sample rate (be carefull with use of delay() in the loop)
  LoadCell.update();

  //get smoothed value from data set
  if (millis() > t + 250) {
    float i1 = LoadCell.getData();
    float c=i1*1000.0;
    int d=(int)c;
    double i=(double)d/1000.0;
    Serial.print("Load_cell output val: ");

    
    lcd.setCursor(0, 1);
    lcd.print("Weight: ");
    if (i < 0) {
      Serial.println(i);
      i=0.0;
      
      lcd.print(i);
      lcd.print(" kg");
    } else {
      Serial.println(i);
      lcd.print(i);
      lcd.print(" kg");
      FirebaseJson updateData;
      FirebaseJson json;
      updateData.set("weight", i);


      if (Firebase.updateNode(firebaseData, "/weights", updateData)) {

        Serial.println(firebaseData.dataPath());

        Serial.println(firebaseData.dataType());

        Serial.println(firebaseData.jsonString());

      } else {
        Serial.println(firebaseData.errorReason());
      }

    }
    /////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////


    //////////////////////////////////////////////////////////////////////////

    delay(100);
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    

    t = millis();
  }

  //receive from serial terminal
  if (Serial.available() > 0) {
    float i;
    char inByte = Serial.read();
    if (inByte == 't') LoadCell.tareNoDelay();
  }

  //check if last tare operation is complete
  if (LoadCell.getTareStatus() == true) {
    Serial.println("Tare complete");
  }
  
}
