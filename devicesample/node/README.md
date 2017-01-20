The prerequisite to run this sample is to have [node.js](http://nodejs.org) installed on the machine you are using and that you have cloned or downloaded the current repository locally.

1. Create a new device Id in the IoT Hub deployed previously and copy its connection string: you will find instructions on how to do this [here][manageazureiothub].
1. Open a command prompt, navigate to the devicesample/node folder and type the following commands (replacing `connectionstring` with the device's connection string)

  ```
  npm install
  node node_device.js connectionstring

At this point the node sample device will establish a secure connection with Azure IoT Hub, send messages every other second and should receive the message back from IoT Hub.

[manageazureiothub]:https://github.com/Azure/azure-iot-sdks/blob/master/doc/manage_iot_hub.md
