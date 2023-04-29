void setup_BLE_COM(){
  setup_BLE();
  setup_COM();
}
//--------------------------------------------------------------------------------------
void setup_BLE(){

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

void setup_COM(){
   
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
    char rc;
    if (bleuart.available() > 0) {
        rc = bleuart.read();
        if(rc >= 33 && rc <= 126  && !isControl(rc)) stringBuf += rc;
        if(rc == TERM_CHAR){newBleData=true; } // '\n' = New Line = Line Feed = 10 (Dec) = 0A (Hex) serialMsg.trim()
    }
}


void sendBLE(String stringBuf){
  uint16_t bufLen;
  bleBuf[0]='\0';
  if(stringBuf.length() >= BLE_BUFSIZE-1){bufLen = BLE_BUFSIZE-1; err=1;}
  else{bufLen = stringBuf.length();}
  stringBuf.toCharArray(bleBuf, bufLen);
  if(bleBuf[bufLen-1] != '\n'){bleBuf[bufLen-1] = '\n';}
  bleuart.write(bleBuf, bufLen);
}



void blelMsgHandler(){
  stringBuf.trim();
  stringBuf.toUpperCase();
  if(stringBuf == "#HAND"){sendBLE("#SHAKE\n");}
  else{ sendBLE("#ECHO: ->" + stringBuf + "<-\n"); }
  stringBuf="";
  newBleData = false;
}


void checkForSerial(){ // ------------------------- SERIAL read  115200 baud 
 
    if (Serial.available() > 0) {
        char rc = Serial.read();
        if(rc >= 33 && rc <= 126  && !isControl(rc)) stringBuf += rc;
        else if(rc == TERM_CHAR){newSerialData=true; stringBuf.trim();} // '\n' = New Line = Line Feed = 10 (Dec) = 0A (Hex)
    }
}


void serialHandler(){
  if(newSerialData && stringBuf != ""){
    stringBuf.trim();
    stringBuf.toUpperCase();
    if(stringBuf =="BLE-ON"){msgln("BLE On recived"); startBLE();}
    else if(stringBuf =="BLE-DISCONNECT"){ msgln("BLE off recived"); disconnectBLE();}
    else if(stringBuf =="BLE-OFF" ){ msgln("BLE off recived");stopBLE();}
    else if(stringBuf =="AUTO-LED-ON" ){Bluefruit.autoConnLed(true);}
    else if(stringBuf =="AUTO-LED-OFF" ){Bluefruit.autoConnLed(false);}
    else if(stringBuf.startsWith("BLINK,")){Bluefruit.setConnLedInterval(stringBuf.substring(6).toInt());}
    else if(stringBuf.startsWith("LED")){setBoardLed(stringBuf.charAt(3), stringBuf.charAt(4));} // LEDR1 for Red channel high
    else{msg("UNKNOWN CMD:  ");msg(stringBuf.c_str()); msgln("  \n");}
    // if (err != 0){
    //   msgln("ERROR CMD:  " + stringBuf + "  \n");
    //   }
    newSerialData=false;
    stringBuf = "";
  }
}
