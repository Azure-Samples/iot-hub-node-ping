// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var Protocol = require('azure-iot-device-mqtt').Mqtt;
var Client = require('azure-iot-device').Client;
var Message = require('azure-iot-device').Message;

var querystring = require('querystring');

var client = null;
var deviceConnectionString;

function main() {
  // receive the IoT Hub connection string as a command line parameter
  if(process.argv.length < 3) {
    console.error('Usage: node_device.js <<IoT Hub Device Connection String>>');
    process.exit(1);
  }

  // open a connection to the device
  deviceConnectionString = process.argv[2];

  client = Client.fromConnectionString(deviceConnectionString, Protocol);
  client.open(onConnect);
}


function onConnect(err) {
  if (err) {
    console.error('Could not connect: ' + err.message);
  } else {
    console.log('Client connected');
    client.on('message', function (msg) {
      console.log('Received Message from IoT Hub: ' + msg.data);
      client.complete(msg, printResultFor('completed'));
      // We are done here
      setTimeout(function(){ process.exit(1);}, 3000);
    });

    client.on('error', function (err) {
      console.error(err.message);
    });

    client.on('disconnect', function () {
      clearInterval(sendInterval);
      client.removeAllListeners();
      client.connect(connectCallback);
    });

    // Create a message and send it to the IoT Hub
    // Extract deviceId from connection string
    var dId = querystring.parse(deviceConnectionString, ';', null, null).DeviceId;
    // Creating JSON message to send to Azure IoT Hub
    var data = JSON.stringify({ deviceId: dId, message: "Hello echo" });
    var message = new Message(data);
    console.log('Sending message: ' + message.getData());
    // Sending message to IoT Hub
    client.sendEvent(message, printResultFor('send'));
  }
}

// Helper function to print results in the console
function printResultFor(op) {
  return function printResult(err, res) {
    if (err) console.log(op + ' error: ' + err.toString());
    if (res) console.log(op + ' status: ' + res.constructor.name);
  };
}

main();