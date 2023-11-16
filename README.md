# Plant Monitor
## 0. Overview:
This system of plant monitor can achieve the goals of measuring soil moisture and the surrounding air environment by sensors. Data can be displayed on webserver. And then, the data will be transferred through WIFI to an MQTT server and the data will be stored in a database. The data includes the surrounding temperature and humidity of the plant and the moisture of the soil. This document is a brief process to build the plant monitor system. This project mainly followed by the [Workshop](https://workshops.cetools.org/codelabs/CASA0014-2-Plant-Monitor) . There are a few additional functions added in this device compared to the workshop.
## 1. Preparations:
* **Arduino IDE**
* **MQTT Explorer**
### Physical items:
* **Copper wire**
* **Solder wire**
* **Resistors**
  * 10k ohm &times; 2
  * 220-ohm resistor &times; 1
  * 100-ohm resistor &times; 1
* **Feather Huzzah ESP8266 Wifi**
* **USB-A to Micro USB** &times; 1
* **USB-C-end power adaptor** &times; 1
* **Nail** &times; 2
* **DHT22 sensor**
* **Raspberry Pi**
### Tools:
* **Soldering iron**
* **Soldering iron stand**
* **Wire stripper**
* **Piler** 
* **Duct tape(optional)**

## 2. Process
### 2.1 Feather Set-up
Download the driver in [Guildline](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers).
### 2.2 Soldering
Four resistors are required to be soldered on the circuit board. The nails should be wrapped tightly with copper wire and solder the wire to the board. Also DHT22 Sensor is required to be soldered as well.

### 2.3 Arduino IDE Set-up
Create a `.h` file to store the WIFI name, WIFI password, MQTT-server Username, MQTT-server User password.
Use the PubSubClient library to  connect to the MQTT server.
Scripting through the DHT and DHT_U libraries, and the use of assembled components to accomplish the sensing of soil moisture as well as the the surrounding temperature and humidity of the plant.

#### Libraries
* **ESP8266WiFi**[Source](https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/ESP8266WiFi.h)
* **ESP8266WebServer**[Source](https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WebServer/src/ESP8266WebServer.h)
* **ezTime**[Source](https://github.com/ropg/ezTime)
* **DHT**[Source](https://github.com/adafruit/DHT-sensor-library/blob/master/DHT.h)
* **DHT_U**[Source](https://github.com/adafruit/DHT-sensor-library/blob/master/DHT_U.h)
* **PubSubClient**][Source](https://www.arduino.cc/reference/en/libraries/pubsubclient/)

### 2.4 Raspberry Pi Set-up
#### 2.4.1 RPi
Download Rapsberry Pi Imager [Download Page](https://www.raspberrypi.com/software/) and follow the [Tutorial](https://www.tomshardware.com/reviews/raspberry-pi-headless-setup-how-to,6028.html) to flash the SD card for the RPi
Do a quick upgrade and reboot by entering the command in the terminal.
```
sudo apt update
sudo apt upgrade -y
sudo reboot
```
#### 2.4.2 InfluxDB
Next, install InfluxDB on RPi on [Download Page](https://portal.influxdata.com/downloads/#influxdb). You can ignore the "Register". 
Then, Add the InfluxDB key.
```
wget -q https://repos.influxdata.com/influxdata-archive_compat.key
echo '393e8779c89ac8d958f81f942f9ad7fb82a25e133faddaf92e15b16e6ac9ce4c influxdata-archive_compat.key' | sha256sum -c && cat influxdata-archive_compat.key | gpg --dearmor | sudo tee /etc/apt/trusted.gpg.d/influxdata-archive_compat.gpg > /dev/null
echo 'deb [signed-by=/etc/apt/trusted.gpg.d/influxdata-archive_compat.gpg] https://repos.influxdata.com/debian stable main' | sudo tee /etc/apt/sources.list.d/influxdata.list
```
After add repo, run another update to the package list
```
sudo apt-get update
```
Install InfluxDB with following coommand:
```
sudo apt-get install influxdb2 -y
```
Finally to get InfluxDB up and running at reboot we need to enable and start it within systemctl:
```
sudo systemctl unmask influxdb.service
sudo systemctl start influxdb
sudo systemctl enable influxdb.service
```
This is used to check whether it's running OK or not.
```
sudo systemctl status influxdb
```

Once successfully running it, access InfluxDB on a browser at the following URL: `http://your_host_name.celab:8086/`.
Use template to create a database with information about the Raspberry Pi.

#### 2.4.3 Telegraf and Grafana
Follow the instructions to finish the visualization started from [Workshop](https://workshops.cetools.org/codelabs/CASA0014-2-Plant-Monitor/index.html#12)

### 2.5 Viewing Plant Data
Once we connect to MQTT server on MQTT Explorer. In the message, we can view the data sent by different plant monitors that connected to the same server.
#### 2.5.1 Data limitations
In the data visualization, you may sometimes see the moisture is over 100. Therefore, it doesn't mean percentage of the soil moisture, which is just a that collected or reported by the sensor.
The user may need to calibrate the data. But this calibration curve is non-linear so it is not easy to get an accurate value. Approximate values can be obtained in many ways. For example, a humidity saturated environment is set to come in and then the value detected by the iron nail is given a value of 100% humidity, while a completely dry environment is set and then the value detected by the iron nail is given a value of 0% humidity.

### 2.6 Additional Function
Huzzah Plant Monitor will send additional data to the MQTT broker. The DHT22_MQTT with nails will be set up within the program, which will set the ideal values for the temperature and humidity of the plant's surroundings and the soil moisture level.
Step 1
Set up a few macros to define the upper and lower limits of the desired values for temperature and humidity and soil moisture. When changing plants in the future, simply replace the values in the macros directly.
The next example is "Temperature", which is applied to "Humidity" and "Moisture" by changing the variable name and the contents of the message.
```
#define temp_min 20 //Define the ideal minimum temperature of a certain plant
#define temp_max 30 //Define the ideal maximum temperature of a certain plant
#define percentage_h_low 60 //Define the ideal minimum humidity of a certain plant
#define percentage_h_high 90 //Define the idea maximum humidity of a certain plant
#define percentage_m_min 30 //Define the ideal minimum moisture moisture of a certain plant
#define percentage_m_max 60 //Define the ideal maximum moisture moisture of a certain plant
```
Step 2
Declaration of new variables and conditions
```
float temp_low = temp_min;
float temp_high = temp_max;
bool temperaturel0 = (Temperature < temp_low);
bool temperatureh1 = (Temperature > temp_high);
```
Step 3
Using the if-else control flow statements to evaluate conditions and transmit environmental status to MQTT.
```
  if (temperaturel0) {
    client.publish("student/CASA0014/plant/ucfniad/temperature_status", "The current temperature is lower than ideal.");
  } else if (temperatureh1) {
    client.publish("student/CASA0014/plant/ucfniad/temperature_status", "The current temperature is higher than ideal.");
  } else {
    client.publish("student/CASA0014/plant/ucfniad/temperature_status", "The current temperature is at the ideal temperature.");
```
The code needs to be added inside the sendMQTT function. For more information, please refer to the [DHT22_MQTT with nails](https://github.com/Hypersaki/Plant-Monitor/blob/main/DHT22_MQTT%20with%20nails/DHT22_MQTT%20with%20nails.ino)
