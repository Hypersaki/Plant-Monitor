#Overview:
The system of plant monitor can achieve the goals of measuring soil moisture and the surrounding air environment by sensors. Data can be displayed on webserver. And then, the data will be transferred through WIFI to an MQTT server and the data will be stored in a database. The data includes the surrounding temperature and humidity of the plant and the moisture of the soil.

#Building Systems
Get the hardware and software ready before building the system.
Arduino IDE, MQTT Explorer are necessary
Also, there are some necessary items:
##Materials and hardware:
Copper wire, Solder wire, 10k-ohm resistor *2, 100-ohm resistor *1, 220-ohm resistor *1, ESP8266 *1,
Raspberry Pi 4 *1, Nail *2, Duct tape(optional), DHT22 sensor 
##Tools:
Soldering iron, Soldering iron stand, Wire stripper, Piler

#Purposes of each components
##Sensors on the board FEATHER HUZZAH ESP8266 are used to collect data.
##Data will be transferred to MQTT and it can be displayed on a webserver.
##historic data can be stored in the Raspberry Pi and Raspberry Pi receives the data through the same WIFI network.

#Process
