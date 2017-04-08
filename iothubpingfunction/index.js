var Client = require('azure-iothub').Client;
var Message = require('azure-iot-common').Message;

var connectionString = process.env.AzureIoTHubConnectionString;

module.exports = function (context, myEventHubTrigger) {
    context.log("Received raw message from device " + JSON.stringify(myEventHubTrigger));
    //var jsonMessage = JSON.parse(myEventHubTrigger.toString());
    var jsonMessage = myEventHubTrigger;
    context.log("After parsing message, reading :");
    context.log("    deviceId : " + jsonMessage.deviceId);
    context.log("    message : " + jsonMessage.message);

    var client = Client.fromConnectionString(connectionString);
    
    client.open(function (err) {
      if (err) {
        context.error('Could not connect: ' + err.message);
      } else {
        context.log('Client connected');
    
        // Create a message and send it to the device
        var data = JSON.stringify(jsonMessage);
        var message = new Message(data);
        context.log('Sending message: ' + message.getData() + 'to: ' + jsonMessage.deviceId);
        client.send(jsonMessage.deviceId, message, printResultFor('send', context));
      }
      context.done();
    });
}

// Helper function to print results in the console
function printResultFor(op, context) {
  return function printResult(err, res) {
    if (err) {
      context.log(op + ' error: ' + err.toString());
    } else {
      context.log(op + ' status: ' + res.constructor.name);
    }
  };
}
