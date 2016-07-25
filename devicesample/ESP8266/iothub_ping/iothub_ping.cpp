// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Includes and variables for serializing messages
#include <ArduinoJson.h>
StaticJsonBuffer<200> jsonBuffer;

#include <AzureIoTHub.h>

static const char* deviceId = "[deviceid]";
static const char* connectionString = "[connectionstring]";

static const char* pingMessage = "Hello IoT World";
static bool messageDelivered = false;
static bool receivedMessageBack = false;

static void SendConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
{
  messageDelivered = true;
  switch(result){
    case IOTHUB_CLIENT_CONFIRMATION_OK:
      LogInfo("Message was succesfully sent to IoT Hub.\r\n");
      break;
  
    case IOTHUB_CLIENT_CONFIRMATION_BECAUSE_DESTROY:
      LogInfo("ERROR: Message was destroyed before it could be sent by the client to IoT Hub.\r\n");
      break;
      
    case IOTHUB_CLIENT_CONFIRMATION_ERROR:
      LogInfo("ERROR: An error occured and the message was not sent to IoT Hub.\r\n");
      break;
      
    default:
      LogInfo("UNKNOWN ERROR: An unknown error occured and the message was not sent to IoT Hub.\r\n");
  }
}

static void sendMessage(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, const unsigned char* buffer, size_t size)
{
    IOTHUB_MESSAGE_HANDLE messageHandle = IoTHubMessage_CreateFromByteArray(buffer, size);
    if (messageHandle == NULL)
    {
        LogInfo("Unable to create a new IoTHubMessage.\r\n");
    }
    else
    {
        if (IoTHubClient_LL_SendEventAsync(iotHubClientHandle, messageHandle, SendConfirmationCallback, NULL) != IOTHUB_CLIENT_OK)
        {
            LogInfo("Failed to hand over the message to IoTHubClient.\r\n");
        }
        else
        {
            LogInfo("IoTHubClient accepted the message for delivery.\r\n");
            messageDelivered = true;
        }

        IoTHubMessage_Destroy(messageHandle);
    }
}

/*this function "links" IoTHub to the serialization library*/
static IOTHUBMESSAGE_DISPOSITION_RESULT ReceiveMessageCallback(IOTHUB_MESSAGE_HANDLE message, void* userContextCallback)
{
    IOTHUBMESSAGE_DISPOSITION_RESULT result;
    const unsigned char* buffer;
    size_t size;
    if (IoTHubMessage_GetByteArray(message, &buffer, &size) != IOTHUB_MESSAGE_OK)
    {
        LogInfo("Unable to IoTHubMessage_GetByteArray.\r\n");
	    result = IOTHUBMESSAGE_ABANDONED;
    }
    else
    {
        receivedMessageBack = true;

        /*buffer is not zero terminated*/
        char* temp = (char*)malloc(size + 1);
        if (temp == NULL)
        {
            LogInfo("Failed to malloc\r\n");
            result = IOTHUBMESSAGE_ABANDONED;
        }
        else
        {
          memcpy(temp, buffer, size);
          temp[size] = '\0';
          LogInfo("Received following message back from IoT Hub %s\r\n",temp);

          // Parse the json message received
          StaticJsonBuffer<200> jsonBuffer;
          JsonObject& root = jsonBuffer.parseObject(temp);

          if (strcmp(root["message"], pingMessage)==0)
          {
              LogInfo("Ping successful!\r\n");
              // TODO: turn green led on
  	        result = IOTHUBMESSAGE_ACCEPTED;
          } else {
              LogInfo("Received a message back... but not the one expected.\r\n");
              // TODO: turn red led on
  	        result = IOTHUBMESSAGE_REJECTED;
          }
          free(temp);
        }
    }
    return result;
}

void iothub_ping_run(void)
{
//    srand((unsigned int)time(NULL));
    int displayedMessageCounter = -1;
    
    IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle;

    iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(connectionString, HTTP_Protocol);
    if (iotHubClientHandle == NULL)
    {
        LogInfo("Failed on IoTHubClient_CreateFromConnectionString\r\n");
    }
    else
    {
#ifdef MBED_BUILD_TIMESTAMP
        // For mbed add the certificate information
        if (IoTHubClient_LL_SetOption(iotHubClientHandle, "TrustedCerts", certificates) != IOTHUB_CLIENT_OK)
        { 
            LogInfo("failure to set option \"TrustedCerts\"\r\n");
        }
#endif // MBED_BUILD_TIMESTAMP

        /* Attach callback for received messages */
        if (IoTHubClient_LL_SetMessageCallback(iotHubClientHandle, ReceiveMessageCallback, NULL) != IOTHUB_CLIENT_OK)
        {
            LogInfo("unable to IoTHubClient_SetMessageCallback\r\n");
        }
        else
        {
            /* Send the ping message to IoT Hub */
            JsonObject& root = jsonBuffer.createObject();
            root["deviceId"] = (char*)deviceId;
            root["message"] = (char*)pingMessage;
            
            char buffer[256];
            root.printTo(buffer, sizeof(buffer));
            
            LogInfo("Send ping to IoT Hub with message: %s\r\n", buffer);
            
            sendMessage(iotHubClientHandle, (unsigned char*)buffer, sizeof(buffer));

            /* Wait for the answer back from IoT Hub */
            while (!receivedMessageBack)
            {
              IoTHubClient_LL_DoWork(iotHubClientHandle);
              ThreadAPI_Sleep(300);
              if ( messageDelivered&& !receivedMessageBack )
              {
                if (displayedMessageCounter++ == -1)
                {
                  LogInfo("Waiting for response from IoT Hub.\r\n");
                } else
                {
                  LogInfo(".");
                  if (displayedMessageCounter++ >= 59)
                  {
                    LogInfo("\r\n");
                    displayedMessageCounter = 0;
                  }
                }
              }
            }
        }
        // Reset flags
        receivedMessageBack = false;
        messageDelivered = false;
        
      IoTHubClient_LL_Destroy(iotHubClientHandle);
    }
}
