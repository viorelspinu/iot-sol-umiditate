#include <Bridge.h>
#include <HttpClient.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(8, 0); // RX, TX


void setup() {
  // Bridge takes about two seconds to start up
  // it can be helpful to use the on-board LED
  // as an indicator for when it has initialized
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin();
  digitalWrite(13, HIGH);

  SerialUSB.begin(9600);
  Serial.println("start1");
  SerialUSB.println("start2");

  mySerial.begin(9600);
}


void loop() {
  if (mySerial.available()) {

    String umiditateStr = readSerialData();
    String batteryStr = readSerialData();
    String temperatureStr1 = readSerialData();
    String temperatureStr2 = readSerialData();
    String radioRSSI = readSerialData();

    SerialUSB.println(umiditateStr);
    SerialUSB.println(batteryStr);
    SerialUSB.println(temperatureStr1);
    SerialUSB.println(temperatureStr2);
    SerialUSB.println(radioRSSI);


    int batteryInt = batteryStr.toInt();
    int batteryMilliVolts = ((((float)batteryInt / 1023) * 3.3) * 3) * 1000;


    doGet("http://iot.robofun.ro/api/v1/input/6um9rjl66uvlgfah99vd5t5a83/" + umiditateStr);

    doGet("http://iot.robofun.ro/api/v1/input/ebkp7tee34puqr6i75trhnitu3/" + String(batteryMilliVolts));

    doGet("http://iot.robofun.ro/api/v1/input/1tv24668s3kbr9i060pla9la5e/" + temperatureStr1);

    doGet("http://iot.robofun.ro/api/v1/input/7scei9p2amfires12i9lj7gg0g/" + temperatureStr2);

    doGet("http://iot.robofun.ro/api/v1/input/dqtu6oajs18or4jtsbc1h54jht/" + radioRSSI);




  }

}


void doGet(String url) {

  HttpClient client;

  digitalWrite(13, HIGH);
  client.get(url);
  delay(250);
  digitalWrite(13, LOW);
  delay(250);

}

String readSerialData() {
  String data = "";
  while (mySerial.available()) {
    char c = (char)mySerial.read();
    delay(1);
    if (c == '#') {
      return data;
    }
    data.concat(c);
  }
  return data;
}

