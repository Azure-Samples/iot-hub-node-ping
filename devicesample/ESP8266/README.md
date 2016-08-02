# Azure IoT Hub ping sample using Adafruit Huzzah ESP8266 Certified Kit

This tutorial describes the process of taking your Adafruit Huzzah ESP8266 kit and using it to send a ping message to IoT Hub 
to validate the device connectivity to Azure IoT Hub. The solution is using Azure IoT Hub libraries, Azure IoT Hub, Azure Functions and Azure Storage required by both IoT Hub and Function.       

***
**Don't have a kit yet?** Click [here](http://azure.com/iotstarterkits)
***

## Tutorial Overview

This tutorial will teach you how to: 
- Set up the Huzzah device and sensors
- Install the software and dependend libraries using Arduino IDE
- How to compile and run a cpp ping sample which sends a message to IoT Hub, it's picked up by an Azure Function which listens to the IoT Hub and triggers a new message with the same content back to the device to confirm that the message is received.
- Capture return message in the callback, log it and turn on the green LED on the board.   

## Prerequisites 

### Required Software

- Azure Subscription (this is the subscription you want the services to be deployed to if you have several Azure subscriptions)
- [Git](https://git-scm.com/downloads) - For cloning the required repositories
- Arduino IDE, version 1.6.8. (Earlier versions will not work with the Azure IoT library)
- Sensor libraries from Adafruit: DHT Sensor library, Adafruit DHT Unified, Adafruit Unified Sensor
- Deploy supporting services Azure IoT Hub, Azure Function and Azure Storage 

### Required Hardware

- Adafruit Huzzah ESP8266 IoT kit
  - Huzzah ESP8266 board
  - DHT22 Sensor
  - breadboard
  - M/M jumper wires
  - 10k Ohm Resistor (brown, black, orange)
  - 2x 330 Ohm Resistor (orange, orange, brown)
  - Green LED
  - Red LED
  - A microB USB cable
  - A desktop or laptop computer which can run **Arduino IDE 1.6.8**

## Deploy Azure services for ping solution
If you have not done so already, follow all the instructions [here](https://github.com/Azure-Samples/iot-hub-node-ping#deploy-the-ping-solution) to deploy in one-click all Azure services needed for this solution: Azure IoT Hub, Azure Function and Azure Storage. 

## Create a device and register it with IoT Hub
Create a new device Id in the IoT Hub deployed previously and copy its connection string: you will find instructions on how to do this [here][manageazureiothub].
It should look the following
```
HostName=<YourHubName>.azure-devices.net;DeviceId=<YourDeviceId>;SharedAccessKey=<YourSharedAccessKey>
```

## Connect the DHT22 Sensor Module to your Device

- Using [this image](https://github.com/Azure-Samples/iot-hub-c-huzzah-getstartedkit/blob/master/img/huzzah_command_center.png?raw=true) as a reference, connect your DHT22 and Adafruit Huzzah ESP8266 to the breadboard

***
**Note:** Column on the left corresponds to sensor and on the Right to board. On the image, the board is place between 10 and 30 and sensor between 1 and 9. Additionally, when counting the - pins, start from the right and count in, as these do not align with the numbers indicated on the board.
***

- Connect the board, sensor, and parts on the breadboard:

| Start                   | End                    | Connector     |
| ----------------------- | ---------------------- | ------------ |
| Huzzah RST (Pin 30i)    | Huzzah CHPD (Pin 15i)  | Huzzah ESP8266 |
| DHT22 (Pin 1J)          | DHT22 (Pin 4J)         | DHT22         |
| NULL (Pin 2I)           | Pin 1F                 | 10k Ohm Resistor  |
| Pin 2-                  | Pin 2A                 | 330 Ohm Resistor  |
| Pin 4-                  | Pin 5A                 | 330 Ohm Resistor  |
| GRN LED (Pin 2C)        | Pin 3C                 | Green LED  |
| RED LED (Pin 5C)        | Pin 6C                 | Red LED  |

- For the pins, we will use the following wiring:

| Start                   | End                    | Cable Color   | Connected to |
| ----------------------- | ---------------------- | ------------ | ------------- |
| VDD (Pin 1G)            | Pin 29J             | Red cable    | DHT22 |
| DATA (Pin 2G)           | Pin 17B             | White cable  | DHT22 |
| GND (Pin 4G)            | Pin 9-              | Black cable  | DHT22 |
| GND (Pin 27J)           | Pin 25-             | Black cable  | Huzzah ESP8266 |
| Pin 22B                 | Pin 6A              | Red cable    | Red LED  |
| Pin 21B                 | Pin 3A              | Green cable    | Green LED  |


- For more information, see: [Adafruit DHT22 sensor setup](https://learn.adafruit.com/dht/connecting-to-a-dhtxx-sensor).

**At the end of your work, your Adafruit Huzzah ESP8266 should be connected with a working sensor.**

## Run the sample on the ESP8266

### Add the Adafruit Huzzah ESP8266 to the Arduino IDE
You will need to install the Adafruit Huzzah ESP8266 board extension for the Arduino IDE:

- Follow the instructions [here](https://learn.adafruit.com/adafruit-huzzah-esp8266-breakout/using-arduino-ide). There you will see how to add a URL pointing to Adafruit's repository of board extensions, how to make the Adafruit Huzzah ESP8266 board selectable under the **Tools** menu, and how to get the Blink sketch to run.
  - **Note**: There are two versions of Huzzah board, one with microB USB connector and other with a USB cable connected directly to the board. Both works properly with Azure IoT.
  - Boards with microB connector don't have the GPIO0 button. So, in the 'Blink Test', ignore the steps to put the board in the bootload mode, and go directly to the step to upload the sketch via the IDE.
- After going through this, you should have a working sample with a blinking light on your board.
    - If you run into any connection issues, unplug the board, hold the reset button, and while still holding it, plug the board back in. This will flash to board to try again.

### Clone the repo

- The prerequisite to run ping sample is to clone or download this current repository (https://github.com/Azure-Samples/iot-hub-node-ping)
- Double-click file `iot-hub-node-ping\devicesample\ESP8266\iothub_ping\iothub_ping.ino` to open the project in the Arduino IDE.

### Install Library Dependencies

For this project, we'll  need the following libraries:

 - DHT Sensor Library
 - AzureIoTHub
 - Adafruit DHT Unified
 - Adafruit Unified Sensor

To install them, click on the `Sketch -> Include Library -> Manage Libraries`. Search for each library using the box in the upper-right to filter your search, click on the found library, and click the "Install" button. 

![Install Libraries][1]

***
**Note**: If you have an earlier version of the IoT library, navigate to your Arduino documents directory. Inside the "Libraries" folder, there will be a number of installed libraries. Simply delete the `AzureIoT` folder.
***

### Modify the Ping Sample

- In the iothub_ping.ino file, look for the following lines of code:

```
static char ssid[] = "[Your WiFi network SSID or name]";
static char pass[] = "[Your WiFi network WPA password or WEP key]";
```

- Replace the placeholders with your WiFi name (SSID), WiFi password, and the device connection string you created at the beginning of this tutorial. Save with `Control-s`
- Open up the file `iothub_ping.cpp`. Look for the following lines of code and replace the placeholders connection information (this is the Device information that you've created in Step [Create a new device][#16-create-new-device]):

```
static const char* deviceId = "[deviceid]";
static const char* connectionString = "[connectionstring]";
```
- Save all changes

### Compile and deploy the sample

- Select the COM port on the Arduino IDE. Use **Tools -&gt; Port -&gt; COM** to select it.
- Use **Sketch -&gt;  Upload** on Arduino IDE to compile and upload to the device.

## View the Ping sent and received message

- Re-select the COM port if necessary, and then open the serial. On the Arduino IDE, use **Tools -&gt; Serial Monitor**. After 15 seconds you should see a measurements update.
- You should see a message similar with the one in following image 

![][3]

- If the message is sent back to the device via Azure Function, there should now be a green LED on your Adafruit Huzzah ESP8266. 

***
**Note**: When first starting you will likely see a “Fetching NTP epoch time failed” error – This is normal, and it trying to sync with Azure. This can take even up to 30 seconds to find a NTP server to sync with. One it is synced, it should start transmitting from there.
***

## Next steps

Please visit our [Azure IoT Dev Center](https://azure.microsoft.com/en-us/develop/iot/) for more samples and documentation on Azure IoT.
 
[1]: ../../media/AdafruitInstallLibraries.png
[2]: ../../media/Error.png
[3]: ../../media/SerialMonitorConsole.png
[manageazureiothub]: https://github.com/Azure/azure-iot-sdks/blob/master/doc/manage_iot_hub.md