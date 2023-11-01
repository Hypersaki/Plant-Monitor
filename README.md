# Plant Monitor
## 0. Overview:
This system of plant monitor can achieve the goals of measuring soil moisture and the surrounding air environment by sensors. Data can be displayed on webserver. And then, the data will be transferred through WIFI to an MQTT server and the data will be stored in a database. The data includes the surrounding temperature and humidity of the plant and the moisture of the soil. This document is a brief process to build the plant monitor system.

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
* **ESP8266WiFi**
* **ESP8266WebServer**
* **ezTime**
* **DHT**
* **DHT_U**
* **PubSubClient**

### 2.4 Raspberry Pi Set-up
#### 2.4.1 RPi
Download Rapsberry Pi Imager [Download Page](https://www.raspberrypi.com/software/) and follow the [Tutorial](https://www.tomshardware.com/reviews/raspberry-pi-headless-setup-how-to,6028.html) to flash the SD card for the RPi
Do a quick upgrade and reboot by entering the command in the terminal.
#### 2.4.2 InfluxDB
Next, install InfluxDB on RPi on [Download Page](https://portal.influxdata.com/downloads/#influxdb). You can ignore the "Register". 
#### 2.4.3 Telegraf
Install Telegraf by entering command in the terminal
#### 2.4.4 Grafana
Installation can be completed through entering command in the terminal. Grafana can be used to visualize the data.

### 2.5 Viewing Plant Data
Once we connect to MQTT server on MQTT Explorer. In the message, we can view the data sent by different plant monitors that connected to the same server.
