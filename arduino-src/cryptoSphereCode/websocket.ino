

void handleWSEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      if (num == 0) {
        digitalWrite(LED_BUILTIN, LOW);
      }
      break;
      
    case WStype_CONNECTED: {
        char * token = (char *) &payload[1];
        boolean success = validateFBToken(String(token));
        
        if (success)
        {
          IPAddress ip = websocket.remoteIP(num);
          Serial.printf("[%u] Connected %d.%d.%d.%d",
                        num, ip[0], ip[1], ip[2], ip[3]);
          wsNum = num;
          websocket.sendTXT(wsNum, "{\"type\": \"connected\"}");
          confirmedAnimation(3);
          digitalWrite(LED_BUILTIN, HIGH);
        } else {
          IPAddress ip = websocket.remoteIP(num);
          Serial.printf("[%u] Rejected %d.%d.%d.%d",
                        num, ip[0], ip[1], ip[2], ip[3]);
          websocket.disconnect(num);
          refusedAnimation(3);
        }
      }
      break;
      
    case WStype_TEXT: {
        Serial.printf("[%u] get Text: %s\n", num, payload);
        if (payload[0] == 's') {
          resetAll();
          char * json = (char *) &payload[2];
          DynamicJsonBuffer jsonBuffer(1600); jsonBuffer;
          JsonObject& root = jsonBuffer.parseObject(json);
          if (!root.success()) {
            Serial.println("parseObject() failed");
            return;
          }

          if (root["puzzle_type"] == "binary") {
            nextThreshold();
            loopState = SENDBINARY;
          }
          else if (root["puzzle_type"] == "color") {
            loopState = SENDCOLOR;
          }
          const char * message = root["message"];
          const char * receiver = root["receiver"];
          const char * receiver_name = root["receiver_name"];
          memcpy(loadedMessage, message, sizeof loadedMessage - sizeof(char));
          memcpy(encrypted, message, sizeof encrypted - sizeof(char));
          memcpy(recipient, receiver, sizeof recipient - sizeof(char));
          memcpy(recipient_name, receiver_name, sizeof recipient_name - sizeof(char));
          
          msgLength = strlen(loadedMessage);
          websocket.sendTXT(wsNum, "{\"type\": \"received\"}");
        }
        else if (payload[0] == 'r') {
          resetAll();
          handlePuzzle((char *) &payload[2]);
          websocket.sendTXT(wsNum, "{\"type\": \"received\"}");
        }
        else if (payload[0] == 'c') {
          resetAll();
          confirmedAnimation(3);
          loopState = WAITING;
        }
        else if (payload[0] == 'd') {
          if (loopState == RECEIVEBINARY) {
            refusedAnimation(3);
          } else {
            resetAll();
            refusedAnimation(3);
            loopState = WAITING;
          }
        }
      }
      break;
  }
}
