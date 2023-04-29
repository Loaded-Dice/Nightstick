void setup_BLE_COM(){

  Bluefruit.configPrphBandwidth(BANDWIDTH_MAX); // Note: All config***() function must be called before begin()

  Bluefruit.begin();
  Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values
  Bluefruit.setName("Nightstick"); // useful testing with multiple central connections
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);
  Bluefruit.Advertising.setSlowCallback(adv_slow_callback);
  Bluefruit.Advertising.setStopCallback(adv_stop_callback);

  bledis.setManufacturer("Loaded Dice");
  bledis.setModel("Nightstick v1.0");

  bledis.begin();// Configure and Start Device Information Service
  bleuart.begin();// Configure and Start BLE Uart Service
  blebas.begin();// Start BLE Battery Service
  blebas.write(100); // set battery percentage (int)

}

void main_BLE_COM(){ // no timing function to read message asap
   readBLE();
   bleMsgHandler();
   serialHandler();
   checkForSerial();
}

void startBLE(void){

  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addService(bleuart);// Include bleuart 128-bit uuid
  Bluefruit.ScanResponse.addName();
  Bluefruit.Advertising.restartOnDisconnect(false); // Enable auto advertising if disconnected
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms --> Infos:  https://developer.apple.com/library/content/qa/qa1931/_index.html   
  Bluefruit.Advertising.setFastTimeout(BLE_FAST_TIMEOUT);      // number of seconds in fast mode
  Bluefruit.Advertising.start(BLE_TIMEOUT);                // 0 = Don't stop advertising after n seconds  
  msgln("BLE on");
  bleMode = BLE_ADV;
}


void stopBLE(){
  disconnectBLE();
  stopAvertising();
  digitalWrite(LED_BLUE, HIGH); // disable blue led (High = off)
  msgln("BLE Off");
  bleMode = BLE_OFF;
  setBoardLed('B',OFF);
}

void disconnectBLE(){
  if(Bluefruit.connected() > 0){ 
    Bluefruit.disconnect(Bluefruit.connHandle()); 
    msgln("Peer Disconnected");
    }
  else{
      msgln("Nobody to disconnect");
    }
}

void stopAvertising(){

  if( Bluefruit.Advertising.isRunning()){ 
    Bluefruit.Advertising.stop();
    msgln("Advertising Stopped");
    } 
  else{
    msgln("Advertising already stopped");
  }
}

void connect_callback(uint16_t conn_handle){

  BLEConnection* connection = Bluefruit.Connection(conn_handle);
  char central_name[32] = { 0 };
  connection->getPeerName(central_name, sizeof(central_name));
  bleMode = BLE_CONN;
  msg("Callback - Connected to ");
  msgln(central_name);
}

void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;
  stopBLE();
  msgln("Callback - Peer disconnected"); //msgln(reason, HEX);
}

void adv_slow_callback(){
  Bluefruit.setConnLedInterval(800); 
  msgln("Callback - Slow Advertising active");
}
void adv_stop_callback(){
     Bluefruit.autoConnLed(false);
    msgln("Advertising stopped");
    digitalWrite(LED_BLUE, HIGH); // disable blue led (High = off)
}


void readBLE(){
    
    if (bleuart.available() > 0 && !newBleData && bleMode == BLE_CONN) {
        char rc = bleuart.read();
        if(rc >= 33 && rc <= 126  && !isControl(rc)) comBufIn[strlen(comBufIn)] = rc; comBufIn[strlen(comBufIn)+1] = '\0';
        if(strlen(comBufIn)>= sizeof(comBufIn)-1){ newBleData=true;} // check for buffer overflow
        else if(rc == TERM_CHAR){comBufIn[sizeof(comBufIn)] = '\0'; newBleData=true; } // '\n' = New Line = Line Feed = 10 (Dec) = 0A (Hex)
    }
}

//fill up comBufOut[] and send out 
void sendBLE(const char* bleBuf){ bleuart.write(bleBuf, strlen(bleBuf)); bleuart.write("\n", 1);}



void bleMsgHandler(){
  if(newBleData && bleMode == BLE_CONN){
    cArrToUpper(comBufIn);
    cArrTrim(comBufIn);
    if(strcmp(comBufIn, "#HAND")  == 0){sendBLE("#SHAKE\n");}
    else{ sendBLE("#ECHO: ->");sendBLE(comBufIn); sendBLE("<-\n"); }
    comBufIn[0]='\0';
    newBleData = false;
  }
}

void checkForSerial(){ // ------------------------- SERIAL read  115200 baud 
    if (Serial && !newSerialData && Serial.available() > 0) {
        char rc = Serial.read();
        if(rc >= 33 && rc <= 126  && !isControl(rc)) comBufIn[strlen(comBufIn)] = rc; comBufIn[strlen(comBufIn)+1] = '\0';
        if(strlen(comBufIn)>= sizeof(comBufIn)-1){ newSerialData=true;} // check for buffer overflow
        else if(rc == TERM_CHAR){comBufIn[sizeof(comBufIn)] = '\0'; newSerialData=true; } // '\n' = New Line = Line Feed = 10 (Dec) = 0A (Hex)
    }
}

void serialHandler(){
  if(Serial && newSerialData){
    cArrToUpper(comBufIn);
    cArrTrim(comBufIn);

         if(strcmp(comBufIn, "BLE-ON")          == 0 ){ msgln("BLE On recived"); startBLE();}
    else if(strcmp(comBufIn, "BLE-DISCONNECT")  == 0 ){ msgln("BLE off recived"); disconnectBLE();}
    else if(strcmp(comBufIn, "BLE-OFF")         == 0 ){ msgln("BLE off recived");stopBLE();}
    else if(strcmp(comBufIn, "AUTO-LED-ON")     == 0 ){ Bluefruit.autoConnLed(true);}
    else if(strcmp(comBufIn, "AUTO-LED-OFF")    == 0 ){ Bluefruit.autoConnLed(false);}
    else if(strncmp (comBufIn, "BLINK,", 6)     == 0 ){
      uint16_t cArrLen = strlen(comBufIn); 
      strcpy(comBufIn, &comBufIn[6]); 
      comBufIn[cArrLen-6] = '\0'; 
      if(isInt(comBufIn)){msgln(comBufIn);Bluefruit.setConnLedInterval(convInt);}
      }
      else if(strncmp (comBufIn, "LED", 3)  == 0 ){setBoardLed(comBufIn[3], comBufIn[4]);} // LEDR1 for Red channel high
    else{msg("UNKNOWN CMD:  ");msg(comBufIn); msgln("  \n");}

    newSerialData=false;
    comBufIn[0] = '\0';
  }
}