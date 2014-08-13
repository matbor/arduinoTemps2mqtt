arduinoTemps2mqtt
=================

Arduino sketch, grab Onewire Temperature's and publish to a [mqtt](http://www.mqtt.org) broker.


**Setup**

* Change MAC address (if you have multiple Adruinos)
* Change DHCP to static IP address if needed.
* Change MQTT broker, mine is setup as mqtt.localdomain
* The Topic you are publishing to is under [charTopic[] ](https://github.com/matbor/arduinoTemps2mqtt/blob/master/arduinoTemp2mqtt.ino#L178) If you do change it, the three lines below need to be modified to tell it where to start adding the HEX ROM value of the dalas onewire's (the XXXXXXX part).
* [PubSubClient](http://knolleary.net/arduino-client-for-mqtt/api/) Library

**Planned improvements, let me know if you want to do this.**

* Specify username/password support, handy for remote sensors (TLS isn't supported),
* Tidy up the topic naming
* Realtime Clock, publish time stamp with temp to JSON topic as well
* Or get the time from a topic you subscribe to

NOTE: My arduino I am using is an [etherten](http://www.freetronics.com/products/etherten#.U-tOJ_kbW-0) from Freetronics.

Thanks for the Help, 



acknowledgements..

Initial topic issues;
http://stackoverflow.com/questions/17480819/onewire-temperatures-to-mqtt-broker-server/17485978?noredirect=1#17485978

OneWire DS18S20, DS18B20, DS1822 Temperature Example
http://www.pjrc.com/teensy/td_libs_OneWire.html

The DallasTemperature library can do all this work for you!
http://milesburton.com/Dallas_Temperature_Control_Library

And Jarek Sinicki for the help with multiple onewire pins.
