arduinoTemps2mqtt
=================

Arduino, grab Onewire Temperature's and publish to a mqtt broker.


**Things you will need to change to suit your setup;**

* MAC address (if you have multiple adruinos
* DHCP or static IP address
* your mqtt broker, mine is setup as mqtt.localdomain
* the charTopic[] will need to be changed to whatever topic you require you will also need to change the three lines below to tell it where to start adding the HEX ROM value of the dalas onewire
* PubSubClient Library -- http://knolleary.net/arduino-client-for-mqtt/api/

**Things that need doing, let me know if you want to do this.**

* Add TLS (TLS isn't supported), username/password support, handy for remote sensors
* Add the ablity to specify multiple pins to search for sensors.
* tidy up the topic naming
* 

NOTE: My arduino I am using is an etherten from Freetronics.
