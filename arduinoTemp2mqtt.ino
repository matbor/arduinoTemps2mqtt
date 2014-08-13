#include <SPI.h>
#include <OneWire.h>
#include <Ethernet.h>
#include <PubSubClient.h>

// by Matthew Bordignon @bordignons on twitter
//
// Modified to automatically publish to topic using the ROM ID with the celsius temperature.
// Thx to help of Nick O'Leary @knolleary and also to Jarek Sinicki 
//
// OneWire  ds(3);  // on pin 3 (a 4.7K resistor is necessary)

// Declare an array with pin numbers for individual OneWire buses
OneWire  ds[] = {
  3, 9};

byte No_of_1Wire_Buses = sizeof(ds) / sizeof(ds[0]);  // Number of pins declared for OneWire = number of Buses
byte this_1Wire_Bus = 0;  // OneWire Bus number that is currently processed
  
//Ethernet and pubsub setup BEGIN
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
//byte server[] = { 10, 0, 8, 34 }; //Comment out if you want DHCP

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}

EthernetClient ethClient;
PubSubClient client("mqtt.localdomain", 1883, callback, ethClient);
//Ethernet and pubsub setup END

char hexChars[] = "0123456789ABCDEF";
#define HEX_MSB(v) hexChars[(v & 0xf0) >> 4]
#define HEX_LSB(v) hexChars[v & 0x0f]

void setup(void) {
  Serial.begin(9600);
  
  if (Ethernet.begin(mac) == 0)
    {
      Serial.println("Failed to configure Ethernet using DHCP");
      return;
    }
    
  Serial.print("My IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print("."); 
  }
  Serial.println();
  delay(3000);
  Serial.print("Number of defined 1Wiew buses: ");
  Serial.println(No_of_1Wire_Buses);
}

void loop(void) {
//Connect to MQTT
  if (!client.connected())
  {
    Serial.println("Connecting to MQTT server");
    client.connect("arduino_temperature");
    client.publish("/lwt/arduino_temperature","ONLINE");
    //client.subscribe("/status/read");
    Serial.println("Connected to MQTT server");
  }
  client.loop();
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius, fahrenheit;
 
  if ( !ds[this_1Wire_Bus].search(addr)) {
    Serial.print("No more addresses for array element number: ");
    Serial.println(this_1Wire_Bus);
    // If all buses done, start all over again
    if (this_1Wire_Bus >= (No_of_1Wire_Buses - 1)) {
      this_1Wire_Bus = 0;
    } else {
      this_1Wire_Bus++;
    }
    ds[this_1Wire_Bus].reset_search();
    Serial.println("Pausing between search's");
    delay(30000); //delay between finding all the sensors
    return;
  }
  
  Serial.print("ROM =");
  for( i = 0; i < 8; i++) {
    Serial.write(' ');
    Serial.print(addr[i], HEX);
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return;
  }
  Serial.println();
 
  // the first ROM byte indicates which chip
  switch (addr[0]) {
    case 0x10:
      Serial.println("  Chip = DS18S20");  // or old DS1820
      type_s = 1;
      break;
    case 0x28:
      Serial.println("  Chip = DS18B20");
      type_s = 0;
      break;
    case 0x22:
      Serial.println("  Chip = DS1822");
      type_s = 0;
      break;
    default:
      Serial.println("Device is not a DS18x20 family device.");
      return;
  } 

  ds[this_1Wire_Bus].reset();
  ds[this_1Wire_Bus].select(addr);
  ds[this_1Wire_Bus].write(0x44, 1);        // start conversion, with parasite power on at the end
  
  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = ds[this_1Wire_Bus].reset();
  ds[this_1Wire_Bus].select(addr);    
  ds[this_1Wire_Bus].write(0xBE);         // Read Scratchpad

  Serial.print("  Data = ");
  Serial.print(present, HEX);
  Serial.print(" ");
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds[this_1Wire_Bus].read();
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.print(" CRC=");
  Serial.print(OneWire::crc8(data, 8), HEX);
  Serial.println();

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;
  fahrenheit = celsius * 1.8 + 32.0;
  Serial.print("  Temperature = ");
  Serial.print(celsius);
  Serial.print(" Celsius, ");
  Serial.print(fahrenheit);
  Serial.println(" Fahrenheit");
//publish the temp now
  //String strTopic = "/house/hardware/arduino/weather1/285A9282300F1/temperature/current";
  char charTopic[] = "/house/hardware/arduino/weather1/XXXXXXXXXXXXXXXX/temperature/current";
  for (i = 0; i < 8; i++) {
    charTopic[33+i*2] = HEX_MSB(addr[i]); //33 is where the backlash before XXX starts
    charTopic[34+i*2] = HEX_LSB(addr[i]); //34 is plus one on the above
  }
  char charMsg[10];
  memset(charMsg,'\0',10);
  dtostrf(celsius, 4, 2, charMsg);
  client.publish(charTopic,charMsg);
  delay(1000); // just adding a small delay between publishing just incase
}
