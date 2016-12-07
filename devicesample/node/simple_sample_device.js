// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

//var Protocol = require('azure-iot-device-amqp').Amqp;
// Uncomment one of these transports and then change it in fromConnectionString to test other transports
// var Protocol = require('azure-iot-device-amqp-ws').AmqpWs;
// var Protocol = require('azure-iot-device-http').Http;
var Protocol = require('azure-iot-device-mqtt').Mqtt;
var Client = require('azure-iot-device').Client;
var Message = require('azure-iot-device').Message;

var querystring = require('querystring');

// String containing Hostname, Device Id & Device Key in the following formats:
//  "HostName=<iothub_host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"
var connectionString = '<connectionstring>';

// fromConnectionString must specify a transport constructor, coming from any transport package.
var client = Client.fromConnectionString(connectionString, Protocol);

var connectCallback = function (err) {
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
    var dId = querystring.parse(connectionString, ';', null, null).DeviceId;
    // Creating JSON message to send to Azure IoT Hub
    var data = JSON.stringify({ deviceId: dId, message: "Hello echo" });
    var message = new Message(data);
    console.log('Sending message: ' + message.getData());
    // Sending message to IoT Hub
    client.sendEvent(message, printResultFor('send'));
  }
};

client.open(connectCallback);

// Helper function to print results in the console
function printResultFor(op) {
  return function printResult(err, res) {
    if (err) console.log(op + ' error: ' + err.toString());
    if (res) console.log(op + ' status: ' + res.constructor.name);
  };
}
