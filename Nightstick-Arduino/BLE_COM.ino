void setup_BLE_COM(){

  Bluefruit.configPrphBandwidth(BANDWIDTH_MAX); // Note: All config***() function must be called before begin()

  Bluefruit.begin();
  Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values
  Bluefruit.setName("Nightstick"); // useful testing with multiple central connections
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);
  Bluefruit.Advertising.setSlowCallback(adv_slow_callback);
  Bluefruit.Advertising.setStopCallback(adv_stop_callback);

  bledis.setManufacturer("Marlon Graeber");
  //bledis.setModel(cfg.bleName);

  bledis.begin();// Configure and Start Device Information Service
  bleuart.begin();// Configure and Start BLE Uart Service
  blebas.begin();// Start BLE Battery Service
  blebas.write(100); // set battery percentage (int)

}

void main_BLE_COM(){ // no timing function to read message asap
   if(bleMode == BLE_CONN){
     readBLE();
     bleMsgHandler();
   }
 //  checkForSerial();
 //  serialHandler();
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
  Bluefruit.setConnLedInterval(400); 
  msgln("BLE on");
  bleMode = BLE_ADV;
  if(cfg.ledMode != LED_OFF){bakLedMode = cfg.ledMode; cfg.ledMode = LED_BLE;}
}


void stopBLE(){
  disconnectBLE();
  stopAvertising();
  digitalWrite(LED_BLUE, HIGH); // disable blue led (High = off)
  msgln("BLE Off");
  bleMode = BLE_OFF;
  setBoardLed('B',OFF);
  if(bakLedMode != LED_OFF){cfg.ledMode = bakLedMode; bakLedMode = LED_OFF;}
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
        int n_bytes = bleuart.available();
        if(n_bytes > SIZE(comBufIn)){sendBLE("BLE Buffer overflow");}
        for(int i = 0; i < n_bytes; i++){ 
          if(i < SIZE(comBufIn)){comBufIn[i] = bleuart.read();}  // overflow protection
          else{bleuart.read(); }
          }
        if(cArrEndsWith(comBufIn,'\r') || cArrEndsWith(comBufIn,'\n')){cArrTrimRight(comBufIn);}

        newBleData=true;

    }
}

//fill up comBufOut[] and send out 
void sendBLE(const char* bleBuf){ bleuart.write(bleBuf, strlen(bleBuf)); bleuart.write("\n", 1);}

void bleMsgHandler(){
  if(newBleData && bleMode == BLE_CONN){
    cArrToUpper(comBufIn);
//    comBufOut[0] = '\0';
//    strcpy(comBufOut,i2char((int)rawBat));
//    strcat(comBufOut,"  ");
//    strcat(comBufOut,f2char(vBat));
//    strcat(comBufOut,"  ");
//    strcat(comBufOut,i2char(pBat));
//    strcat(comBufOut,"  ");
//    strcat(comBufOut,i2char(btnVal));
    if(strcmp(comBufIn, "#HAND")  == 0){sendBLE("#SHAKE\n");}
    else if(strcmp(comBufIn, "MODE+")   == 0){switchLedMode(1);}     
    else if(strcmp(comBufIn, "MODE-")   == 0){switchLedMode(-1);}
    else if(strcmp(comBufIn, "MODE?")   == 0){ sendBLE(i2char(cfg.ledMode)); sendBLE(","); sendBLE(modeNames[cfg.ledMode]); sendBLE("\n");} 
    else if(strcmp(comBufIn, "PAL+")    == 0){nextPalette();} 
    else if(strcmp(comBufIn, "PAL-")    == 0){lastPalette();} 
    else if(strcmp(comBufIn, "PAL?")    == 0){sendBLE(i2char(cfg.palette));sendBLE(","); sendBLE(paletteNames[cfg.palette]); sendBLE("\n");} 
    else if(strcmp(comBufIn, "ANI+")    == 0){nextAni();} 
    else if(strcmp(comBufIn, "ANI-")    == 0){lastAni();}
    else if(strcmp(comBufIn, "ANI?")    == 0){ sendBLE(i2char(cfg.ledAni));sendBLE(","); sendBLE(aniList[cfg.ledAni].aniName); sendBLE("\n");}
    else if(strcmp(comBufIn, "BRIGHT+") == 0){changeBright(1); }
    else if(strcmp(comBufIn, "BRIGHT-") == 0){changeBright(-1);}
    else if(strcmp(comBufIn, "BRIGHT?") == 0){ sendBLE(i2char(cfg.bright)); sendBLE("\n");}
    else if(strcmp(comBufIn, "BMP+")    == 0){ledsChangeBmp(1);}
    else if(strcmp(comBufIn, "BMP-")    == 0){ledsChangeBmp(-1);}
    else if(strcmp(comBufIn, "BMP?")    == 0){sendBLE(getBmpName()); sendBLE("\n");}
    else if(strcmp(comBufIn, "FLD+")    == 0){ledsChangeFld(1);}
    else if(strcmp(comBufIn, "FLD-")    == 0){ledsChangeFld(-1);}    
    else if(strcmp(comBufIn, "FLD?")    == 0){sendBLE(getFldName()); sendBLE("\n");}    // cfg.trailsBmp   cfg.staticBmp  i2char(cfg.ledAni) i2char(cfg.palette) i2char(cfg.ledMode)
    else if(strcmp(comBufIn, "BATT?")   == 0){sendBLE(i2char(pBat)); sendBLE("\n");}
    else if(strncmp (comBufIn, "STATIC,", 7) == 0 ){ strcpy(comBufIn, &comBufIn[7]); setBmp(FULLPATH_STATIC,comBufIn);}
    else if(strncmp (comBufIn, "TRAIL,", 6)  == 0 ){ strcpy(comBufIn, &comBufIn[6]); setBmp(FULLPATH_TRAILS,comBufIn);}
  

  
    else{ sendBLE("#ECHO: ->");sendBLE(comBufIn); sendBLE("<-\n"); }
    comBufIn[0]='\0';
    newBleData = false;
  }
}
void setBmp(char * constPath, char * fldFile){

  folderBuff[0] = '\0'; fileBuff[0] = '\0'; 
  strncpy(folderBuff,fldFile,cArrIndexOf(fldFile,','));
  strcpy(fileBuff,&fldFile[cArrIndexOf(fldFile,',')+1]);

  if(chkPathValid(constPath,folderBuff,fileBuff)){
       if(strcmp (constPath, FULLPATH_STATIC)  == 0 ){
        cfg.staticFolder[0] = '\0'; cfg.staticBmp[0] = '\0';  
        cfg.ledMode = LED_STATIC; 
        strcpy(cfg.staticFolder,folderBuff); 
        strcpy(cfg.staticBmp,fileBuff);
        }
       else if(strcmp (constPath, FULLPATH_TRAILS)  == 0 ){
        cfg.trailsFolder[0] = '\0'; cfg.trailsBmp[0] = '\0';  
        cfg.ledMode = LED_TRAIL; 
        strcpy(cfg.trailsFolder,folderBuff); 
        strcpy(cfg.trailsBmp,fileBuff);
        }
        BMPtoRAM(pathBuff);
  }
  else{sendBLE("Error!\n");
  }
}

bool chkPathValid(char * constPath, char * fld, char * file){
  pathBuff[0] = '\0';
  strcpy(pathBuff,constPath); strcat(pathBuff,"/");strcat(pathBuff,fld);strcat(pathBuff,"/");strcat(pathBuff,file);
  return sd.exists(pathBuff);
}

const char * getFldName(){
  if(cfg.ledMode == LED_STATIC){return cfg.staticFolder;}
  else if(cfg.ledMode == LED_TRAIL){return cfg.trailsFolder;}
  return "-";
}
const char * getBmpName(){
  if(cfg.ledMode == LED_STATIC){return cfg.staticBmp;}
  else if(cfg.ledMode == LED_TRAIL){return cfg.trailsBmp;}
  return "-";
}


void checkForSerial(){ // ------------------------- SERIAL read  115200 baud 
    if (Serial && !newSerialData && Serial.available() > 0) {
       int n_bytes = Serial.available();
       char rx;
        for(int i = 0; i < n_bytes; i++){ 
          rx = Serial.read();
          if(i < SIZE(comBufIn)-1 && !isControl(rx) ){comBufIn[i] = rx;}  // overflow protection
          else{comBufIn[i] = '\0'; 
//            while(Serial.available() > 0){Serial.read(); }
//            break;
          }
        }
        if(cArrEndsWith(comBufIn,'\r') || cArrEndsWith(comBufIn,'\n')){cArrTrimRight(comBufIn);}
        newSerialData=true;
    }
}


void serialHandler(){
  if(Serial && newSerialData){
    cArrToUpper(comBufIn);
    //cArrTrim(comBufIn);

         if(strcmp(comBufIn, "BLE-ON")          == 0 ){ msgln("BLE On recived"); startBLE();}
    else if(strcmp(comBufIn, "BLE-DISCONNECT")  == 0 ){ msgln("BLE off recived"); disconnectBLE();}
    else if(strcmp(comBufIn, "BLE-OFF")         == 0 ){ msgln("BLE off recived");stopBLE();}
    else if(strcmp(comBufIn, "BLE-INIT")        == 0 ){ msgln("BLE init recived");setup_BLE_COM();}
    else if(strcmp(comBufIn, "AUTO-LED-ON")     == 0 ){ Bluefruit.autoConnLed(true);}
    else if(strcmp(comBufIn, "AUTO-LED-OFF")    == 0 ){ Bluefruit.autoConnLed(false);}
    else if(strcmp(comBufIn, "PEAK")            == 0 ){ }
    else if(strcmp(comBufIn, "CLEAR")           == 0 ){ }
    else if(strncmp (comBufIn, "BLINK,", 6)     == 0 ){
      uint16_t cArrLen = strlen(comBufIn); 
      strcpy(comBufIn, &comBufIn[6]); 
      //comBufIn[cArrLen-6] = '\0'; 
      if(isInt(comBufIn)){msgln(comBufIn);Bluefruit.setConnLedInterval(convInt);}
      }
      else if(strncmp (comBufIn, "LED", 3)  == 0 ){setBoardLed(comBufIn[3], comBufIn[4]);} // LEDR1 for Red channel high
    else if(strcmp(comBufIn, "ROLL")       == 0){}
    else if(isInt(comBufIn) && convInt < NUM_LEDS && convInt >= 0){cfg.ledMode = LED_TEST; ledsClear(); leds[convInt] = CRGB::Red; ledsShow(); comBufIn[0] = '\0';} 
    else{msg("UNKNOWN CMD:  ");msg(comBufIn); msgln("  \n");}

    newSerialData=false;
    comBufIn[0] = '\0';
  }
}
