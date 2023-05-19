void setup_Config(){ // try to find the config file on the SD card and read the config values
  
  charBuff[0]='\0'; // clear buffer
  strcpy(charBuff,MAIN_PATH);
  chkOrMkDir(charBuff); // check if mainpath /Nightstick exists and create if missing
  strcat(charBuff,SUBFLD_BMP);
  chkOrMkDir(charBuff); // check if subfolder Nightstick/BMPs exists and create if missing
  strcat(charBuff,SUBFLD_TRAIL);
  chkOrMkDir(charBuff); // check if subfolder Nightstick/BMPs/trails exists and create if missing
  cArrTrimRight(charBuff,strlen(SUBFLD_TRAIL));
  strcat(charBuff,SUBFLD_STATIC); // check if subfolder Nightstick/BMPs/static exists and create if missing
  chkOrMkDir(charBuff);
  
  charBuff[0]='\0'; // clear buffer
  strcpy(charBuff,MAIN_PATH);
  strcat(charBuff,"/");
  strcat(charBuff,CONFIGNAME);
    if (sd.exists(charBuff) && file.open(charBuff, FILE_READ)){readCfg(); }
    else {  writeCfg();  }

}

int8_t chkOrMkDir(char*  chkpath){
  if(strcmp(chkpath,"/") == 0 ){return 0;} // can't create root folder
  if(chkpath[0] == '/'){cArrTrimLeft(chkpath);} // remove first slash

  int8_t result = 0;
      if (sd.exists(chkpath)){result =  1;} // if "/subPathName" exists return 0
      else{
        if (sd.mkdir(chkpath)) {result =  2; delay(300);}  // remove leading "/" and create "subPathName" folder - on success return 1
        else {result =   -1;} // return -2 if faild to create subfolder
      }
  if(DEBUG){
    switch(result){
      case -1:  Serial.print("Error: Failed to create: "); Serial.println(chkpath); break;
      case  1:  Serial.print("OK: SubFolder found: "); Serial.println(chkpath); break;
      case  2:  Serial.print("OK: Successfully created: "); Serial.println(chkpath); break;
      default:  Serial.println("Error: Something unknown went wrong while checking / creating folder");
    }
  }
  return result;
}

void readCfg(){
    charBuff[0] = '\0';
    while (file.available()) {
        uint16_t n = file.fgets(charBuff, sizeof(charBuff));
        if (n <= 0) { writeCfg();  return; } // fgets (read line) failed
        if (charBuff[n-1] != '\n' && n == (sizeof(charBuff) - 1)) { msgln("line too long"); }
        if (!parseLine(charBuff, n)) { msgln("parseLine failed");}
  }
  file.close();
  chkBmpsAndFolders(); // see at SD.ino tab
}

bool parseLine(char* buff, uint16_t lineLen) {

  bool parseErr = false;
    // Assuming all .csv lines ending with \r\n we will replace \r 
    // with the seperator char to also get the last element as a token 
    // and then replace \n with null char to end the  char array
    // CFG_SEPERATOR = ";" and so CFG_SEPERATOR[0] = ';'
    // double qutationmarks for the seperator char are necessary because strtok expects char array
    if(buff[lineLen-2] == '\r'){buff[lineLen-2] = CFG_SEPERATOR[0];} 
    if(buff[lineLen-1] == '\n'){buff[lineLen-1] = '\0';}

  char *token;
  token = strtok(buff, CFG_SEPERATOR); // first token is property name
  uint8_t entryIdx = findProperty(token);
  strcpy(charBuff,token);
  if (entryIdx == -1){ return false;}
  token = strtok(NULL, CFG_SEPERATOR);
  
    //  Variable type checking functions
    //  not only return if the char array is convertable into the specific type 
    //  but also store the variable within the specific type buffers
    //    bool isByte(char *)   on success variable stored in   uint8_t convByte;
    //    bool isInt(char *)    on success variable stored in   int     convInt;
    //    bool isFloat(char *)  on success variable stored in   float   convFloat;
    //    bool isBool(char *)   on success variable stored in   bool    convBool;
    //    bool isWord(char *))  on success variable is available via the char * array given to the function
    // To do: store all vonverted types in a common byte array buffer and make a type conversion when assinging the value to the corresponding variable

           if( entryIdx == CFG_START   &&  token == NULL){msgln("Start Config ok");}
      else if( entryIdx == CFG_BLENAME &&  isWord(token)){cfg.bleName[0] =  '\0';  strncpy(cfg.bleName ,token, sizeof(cfg.bleName)); msgln("BLE Name OK");}
      else if( entryIdx == CFG_BRIGHT  &&  isByte(token)){cfg.bright = convByte; msgln("Brightness ok");}
      else if( entryIdx == CFG_STATICBMP && isWord(token)){cfg.staticBmp[0] =  '\0';    strncpy(cfg.staticBmp   ,token ,sizeof(cfg.staticBmp));msgln("Bmp File ok but existance not checked yet");}
      else if( entryIdx == CFG_STATICFLD && isWord(token)){cfg.staticFolder[0] =  '\0'; strncpy(cfg.staticFolder ,token ,sizeof(cfg.staticFolder));msgln("Bmp Folder Ok but existance not checked yet");}
      else if( entryIdx == CFG_TRAILSBMP && isWord(token)){cfg.trailsBmp[0] =  '\0';    strncpy(cfg.trailsBmp   ,token ,sizeof(cfg.trailsBmp));msgln("Bmp File ok but existance not checked yet");}
      else if( entryIdx == CFG_TRAILSFLD && isWord(token)){cfg.trailsFolder[0] =  '\0'; strncpy(cfg.trailsFolder ,token ,sizeof(cfg.trailsFolder));msgln("Bmp Folder Ok but existance not checked yet");}
      else if( entryIdx == CFG_ANI     &&  isByte(token)){cfg.ledAni = convByte;msgln("Animation OK");}
      else if( entryIdx == CFG_PALETTE &&  isByte(token)){cfg.palette = convByte;msgln("Colorpalette ok");}
      else if( entryIdx == CFG_LEDMODE &&  isByte(token)){cfg.ledMode = convByte;msgln("LED Mode OK");}
      else if( entryIdx == CFG_ENDE    &&  token == NULL){msgln("End Config file ok");}
      else {msg("error with this token: "); msg(charBuff); msg("reading this value -->"); msg(token); msgln("<--"); return false;}
      return true;
}

int16_t findProperty(char* str){
  for(uint8_t i = 0; i < cfgInfoCount; i++){
    getCfgInfo(i);
    if(strcmp(str,cfgEntry.cfgVarName) == 0){return i;}
  }
  return -1; // --> no matching entry
}

// load the indexed entry of cfgEntryArray[] from progmem into  the cfgEntry buffer to compare the values
void getCfgInfo(uint8_t index){ if(index < cfgInfoCount){ memcpy_P(&cfgEntry, &cfgEntryArray[index], sizeof(cfgEntry));}}

void writeCfg(){
  
  msgln("Writing new config file");
  charBuff[0]='\0'; // clear buffer
  toCharBuff(MAIN_PATH,CONFIGNAME,true);
  
    if(sd.exists(charBuff)){sd.remove(charBuff);} // remove old config file
    if (!file.open(charBuff, FILE_WRITE)) {  msgln("open failed"); }
  for(uint8_t entryIdx = 0; entryIdx < cfgInfoCount; entryIdx++){
    
      getCfgInfo(entryIdx);
      file.print(cfgEntry.cfgVarName); // Writing the property name 
      file.print(";");

      // writing the property values
           if( entryIdx == CFG_START ){msgln("Header Start written");} // no value to write just property name "Nightstick Config Start" is written
      else if( entryIdx == CFG_BLENAME ){ file.print(cfg.bleName);msgln("BLE Name written");}
      else if( entryIdx == CFG_BRIGHT ){ file.print(i2char(cfg.bright));msgln("Brightness written");}
      else if( entryIdx == CFG_STATICBMP ){ file.print(cfg.staticBmp);msgln("bmp filename written");}
      else if( entryIdx == CFG_STATICFLD ){ file.print(cfg.staticFolder);msgln("bmp folder written");}
      else if( entryIdx == CFG_TRAILSBMP ){ file.print(cfg.trailsBmp);msgln("bmp filename written");}
      else if( entryIdx == CFG_TRAILSFLD ){ file.print(cfg.trailsFolder);msgln("bmp folder written");}
      else if( entryIdx == CFG_ANI ){ file.print(i2char(cfg.ledAni));msgln("led ani no. written");}
      else if( entryIdx == CFG_PALETTE ){ file.print(i2char(cfg.palette));msgln("palette no. written");}
      else if( entryIdx == CFG_LEDMODE ){ file.print(i2char(cfg.ledMode));msgln("led mode no. written");}
      else if( entryIdx == CFG_ENDE ){msgln("Header End written");} // no value to write just property name "Nightstick Config Start" is written
  file.print(F("\r\n"));
  }
  file.close();
  msgln("new config.csv created");
 }
