#include <Bridge.h>
#include <HttpClient.h>
#include <SoftwareSerial.h>
#include "Wire.h"
#include "LiquidCrystal.h"
#include <Arduino.h>
#include <avr/wdt.h>

LiquidCrystal lcd(0);
SoftwareSerial mySerial(8, 0); // RX, TX


void setup() {
  wdt_disable();
  lcd.begin(16, 2);

  randomSeed(analogRead(0));


  if (random(20) == 0) {
    forceLinuxReboot();
  }



  // Bridge takes about two seconds to start up
  // it can be helpful to use the on-board LED
  // as an indicator for when it has initialized
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  lcd.print("BOOTING...");
  Bridge.begin();
  digitalWrite(13, HIGH);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("BOOT DONE");

  SerialUSB.begin(9600);

  mySerial.begin(9600);
  wdt_enable(WDTO_8S);
}

long second = 0;
void loop() {
  wdt_reset();
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

    lcd.clear();
    lcd.print("A:");
    lcd.print(temperatureStr1);
    lcd.print("");


    lcd.print(" S:");
    lcd.print(temperatureStr2);
    lcd.print("");


    lcd.print(" U:");
    lcd.print(umiditateStr);

    int umiditateInt = umiditateStr.toInt();
    int temperaturaSolInt = temperatureStr2.toInt();
    int tempDiff = temperaturaSolInt  - 25;

    String coefStr = doGet("http://www.robofun.ro/setari-poienari.txt");
    float coefFloat = coefStr.toFloat();


    float correctionPercent = coefFloat * tempDiff;
    int umiditateCorectataInt = (int)((float)umiditateInt * (1 + correctionPercent));
    String umiditateCorectataStr = String (umiditateCorectataInt);


    doGet("http://iot.robofun.ro/api/v1/input/6um9rjl66uvlgfah99vd5t5a83/" + umiditateStr);
    doGet("http://iot.robofun.ro/api/v1/input/8h7tucc7isahn7crlmlveqctgq/" + umiditateCorectataStr);
    doGet("http://iot.robofun.ro/api/v1/input/cdo31qhe214b13tpabsfs9eobt/" + umiditateCorectataStr);
    doGet("http://iot.robofun.ro/api/v1/input/6fae1jn7orhakc11nu5hgba4ba/" + umiditateStr);



    doGet("http://iot.robofun.ro/api/v1/input/ebkp7tee34puqr6i75trhnitu3/" + String(batteryMilliVolts));
    doGet("http://iot.robofun.ro/api/v1/input/ebkp7tee34puqr6i75trhnitu3/" + String(batteryMilliVolts));

    doGet("http://iot.robofun.ro/api/v1/input/1tv24668s3kbr9i060pla9la5e/" + temperatureStr1);
    doGet("http://iot.robofun.ro/api/v1/input/ohr3ik58g8s1degi51vo7r57g9/" + temperatureStr1);


    doGet("http://iot.robofun.ro/api/v1/input/bao30u52v1eu1fee7smrehfu38/" + temperatureStr2);
    doGet("http://iot.robofun.ro/api/v1/input/ih9kb08dil70vkh7rodk269inn/" + temperatureStr2);


    doGet("http://iot.robofun.ro/api/v1/input/dqtu6oajs18or4jtsbc1h54jht/" + radioRSSI);
    doGet("http://iot.robofun.ro/api/v1/input/sl34urshgdqpg4i92ivsk85uan/" + radioRSSI);

    second = 0;
  }
  second++;
  lcd.setCursor(13, 1);
  lcd.print(second);
  delay(1000);

}


String doGet(String url) {
  wdt_reset();

  lcd.setCursor(0, 1);
  lcd.print("GET...");
  HttpClient client;

  digitalWrite(13, HIGH);
  client.get(url);
  lcd.setCursor(0, 1);
  lcd.print("      ");
  lcd.setCursor(0, 1);

  String data = "";
  while (client.available()) {
    char c = client.read();
    lcd.print(c);
    data.concat(c);
  }
  digitalWrite(13, LOW);
  delay(500);


  lcd.setCursor(0, 1);
  lcd.print("                 ");

  return data;
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


void forceLinuxReboot() {
  lcd.print("LINUX BOOT FORCED");
  Process p;
  p.runShellCommand("/sbin/reboot");
  lcd.setCursor(0, 1);
  lcd.print("WAIT 60 SEC");
  while (p.running());
  for (int i = 0; i < 60; i++) {
    lcd.setCursor(13, 1);
    lcd.print(i);
    delay(1000);
  }
  lcd.print("REBOOT DONE");
  lcd.clear();


}
