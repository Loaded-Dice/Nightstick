/*
  Load next / last bmp and folder:
  ledsChangeBmp(int8_t loadDirection) // LAST -1 , NEXT 1
  ledsChangeFld(int8_t loadDirection) // LAST -1 , NEXT 1
  
   how to load bmp to display:
   clear charBuff[] ( charBuff[0] = '\0'; )
   create full filepath to bmp file and store it in charBuff[]
   call BMPtoRAM(charBuff); it will analyze the header file auomaticly
   file is now saved to ram
   every frame update call: 
  ---------------------------------
  Led2Pixel_static(new_angle_in_rad); to obtain newest xy position from the leds on the image
  and then drawRamPixel(); to put the rgb data corrsponding to the XY data into the led array
  finally call ledsShow() to update the stick
 */

void setup_SD(){
  if (!sd.begin(SD_CONFIG)) { 
    error(1);
    //sd.initErrorHalt(&Serial);
    }
}

void printDirectory(File32 path, int numTabs) {
   while(true) {
     File32 entry =  path.openNextFile();
     if (! entry) { msgln("**nomorefiles**"); break; }
     for (uint8_t i=0; i<numTabs; i++) { msg("\t"); }
     msg(i2char(entry.dirIndex()));
     msg("\t");
     entry.getName(charBuff,sizeof(charBuff));
     msg(charBuff);
     if (entry.isDir()) {  msgln("/"); printDirectory(entry, numTabs+1);} 
     else { msg("\t\t"); msgln(ul2char(entry.size())); }
     entry.close();
   }
}


void ledsChangeBmp(int8_t loadDirection){ // LAST -1 , NEXT 1
  
  if(ledMode == LED_TRAIL ){
    strcpy(cfg.trailsBmp, getOtherBmp(toPathBuff(FULLPATH_TRAILS, cfg.trailsFolder, true) ,cfg.trailsBmp, loadDirection)); 
    BMPtoRAM(toCharBuff(pathBuff,cfg.trailsBmp,true));
    }
  else if(ledMode == LED_STATIC){
    strcpy(cfg.staticBmp, getOtherBmp(toPathBuff(FULLPATH_STATIC, cfg.staticFolder, true) ,cfg.staticBmp, loadDirection)); 
    BMPtoRAM(toCharBuff(pathBuff,cfg.staticBmp,true));
    }

Serial.println(charBuff);
  // combine parent folder (still at pathBuff) with new bmp 
  
  writeCfg();
}

void ledsChangeFld(int8_t loadDirection){
       if(ledMode == LED_TRAIL ){
        strcpy(cfg.trailsFolder, getOtherFld(FULLPATH_TRAILS,  cfg.trailsFolder , loadDirection));                  // load next/last folder
        strcpy(cfg.trailsBmp, getOtherBmp(toPathBuff(FULLPATH_TRAILS,  cfg.trailsFolder, true) ,cfg.trailsBmp, 0)); // then set new bmp to index 0
        BMPtoRAM(toCharBuff(pathBuff,cfg.trailsBmp,true));
       }
  else if(ledMode == LED_STATIC){
        strcpy(cfg.staticFolder, getOtherFld(FULLPATH_STATIC,  cfg.staticFolder , loadDirection));
        strcpy(cfg.staticBmp, getOtherBmp(toPathBuff(FULLPATH_STATIC, cfg.staticFolder, true) ,cfg.staticBmp, 0));
        BMPtoRAM(toCharBuff(pathBuff,cfg.staticBmp,true));
        }
   // combine parent folder (still at pathBuff) with new bmp
 Serial.println(charBuff);
  writeCfg();
}
//---------------------------------------------------------------------------------------------------------
//   checking config file "last folders" and "last bmps" for existance and otherwise assign first found (at boot up)
// --------------------------------------------------------------------------------------------------------
void chkBmpsAndFolders(){

bool rewriteCfg = false;
  if(!validPath(toPathBuff(FULLPATH_TRAILS,  cfg.trailsFolder, true),  cfg.trailsFolder)){
    strcpy(cfg.trailsFolder, getOtherFld(FULLPATH_TRAILS,  cfg.trailsFolder , 0));
    rewriteCfg = true;
    }
  if(!validPath(toCharBuff(pathBuff, cfg.trailsBmp, true), cfg.trailsBmp)){
    strcpy(cfg.trailsBmp, getOtherBmp(toPathBuff(FULLPATH_TRAILS,  cfg.trailsFolder, true) ,cfg.trailsBmp,0));
    rewriteCfg = true;
    }
  if(!validPath(toPathBuff(FULLPATH_STATIC,  cfg.staticFolder, true),  cfg.staticFolder)){
    strcpy(cfg.staticFolder, getOtherFld(FULLPATH_STATIC,  cfg.staticFolder , 0));
    rewriteCfg = true;
    }
  if(!validPath(toCharBuff(pathBuff, cfg.staticBmp, true), cfg.staticBmp)){
    strcpy(cfg.staticBmp, getOtherBmp(toPathBuff(FULLPATH_STATIC ,cfg.staticFolder,true),cfg.staticBmp,0));
    rewriteCfg = true;
    }
  if(rewriteCfg){writeCfg();}
}

char * toCharBuff(char* cArr1, char* cArr2, bool chkSlash){return toBuff(cArr1,cArr2,charBuff,chkSlash);}
char * toCharBuff(char* cArr1, char* cArr2){return toBuff(cArr1,cArr2,charBuff,false);}
char * toPathBuff(char* cArr1, char* cArr2, bool chkSlash){return toBuff(cArr1,cArr2,pathBuff,chkSlash);}
char * toPathBuff(char* cArr1, char* cArr2){return toBuff(cArr1,cArr2,pathBuff,false);}

char * toBuff(char* cArr1, char* cArr2, char* targetArr, bool chkSlash){
  targetArr[0]='\0';
  strcpy(targetArr, cArr1);
  if(chkSlash && !cArrEndsWith(cArr1,'/') && !cArrStartsWith(cArr2,'/')){strcat(targetArr,"/");}
  strcat(targetArr, cArr2);
  return targetArr;
}

bool validPath(char * fullPath, char * fullPathEndFile){
  if(strcmp(fullPathEndFile,NULL) == 0 || strcmp(fullPathEndFile," ") == 0 || strcmp(fullPathEndFile,"-") == 0){return false;}
  if(sd.exists(fullPath)){return true;}
  else {return false;}
}

// searchDir:   -1=last, 0 = first file, 1 = next
// searchDir is for search direction not directory!
char * getOtherFld(char * parentFld, char * fldName, int8_t searchDir){ return getOtherFile(parentFld, fldName, searchDir, TYPE_FLD);}

char * getOtherBmp(char * parentFld, char * bmpName, int8_t searchDir){ return getOtherFile(parentFld, bmpName, searchDir, TYPE_BMP); }
 
char * getOtherFile(char * parentFld, char * fileNameRaw, int8_t searchDir, uint8_t filetype ){ // searchDir is for search direction not directory!
   char fileName[MAXFILECHARS];
   strcpy(fileName,fileNameRaw); // safety measure to not alternate the input
   if(filetype == TYPE_FLD && cArrStartsWith(fileName, '/')){cArrTrimLeft(fileName);}
   if(filetype == TYPE_FLD &&   cArrEndsWith(fileName, '/')){cArrTrimRight(fileName);}
   File32 path;
   int16_t fileMax;
   if(filetype == TYPE_BMP && searchDir != 0){ fileMax = bmpCountInFld(parentFld);}
   else if(filetype == TYPE_FLD && searchDir != 0){ fileMax = fldCountInFld(parentFld);}
   int16_t fileCount = 0;
   int16_t findIdx = -1;
   if (sd.exists(parentFld) && path.open(parentFld, FILE_READ)){
      if(path.isDir()){
      while(true) {
        File32 entry =  path.openNextFile();
        if (! entry) {break; }
        entry.getName(charBuff,sizeof(charBuff));
        if((isBmp(charBuff)&& entry.isFile() && filetype == TYPE_BMP) || (entry.isDir() && filetype == TYPE_FLD)){
          
          if(searchDir == 0){findIdx = 0; break;} // return first bmp name 
          if(strcmp(fileName,charBuff) == 0){ 
                 if(searchDir == -1 && fileCount == 0){findIdx = fileMax-1; break;} // get name by fileCount index = maxCount-1
            else if(searchDir == -1 && fileCount != 0){findIdx = fileCount-1; break;}
            else if(searchDir ==  1 && fileCount == fileMax-1){findIdx = 0; break;} // roll over and get first bmp 
            else if(searchDir ==  1 && fileCount <  fileMax-1){findIdx = fileCount+1; break;} 
          }
          fileCount++;
        }
      }
    }
  }
  path.close();
  if(findIdx == -1){charBuff[0] = '\0';return charBuff;}
  else {return getFileNameByIdxCount(parentFld, findIdx, filetype);}
}

int16_t bmpCountInFld(char * searchFldPath){return filetypeInFld(searchFldPath,TYPE_BMP);}
int16_t fldCountInFld(char * searchFldPath){return filetypeInFld(searchFldPath,TYPE_FLD);}

int16_t filetypeInFld(char * fldPath, uint8_t filetype){
   File32 path;
  uint16_t fileCount = 0; 
  if (sd.exists(fldPath) && path.open(fldPath, FILE_READ)){
      if(!path.isDir()){path.close(); return -1;}
      while(true) {
         File32 entry =  path.openNextFile();
        if (! entry) {break; }
        if(filetype == TYPE_FLD && entry.isDir()){fileCount++;} //#define TYPE_FLD 0
        else if(filetype == TYPE_BMP && entry.isFile()){ //#define TYPE_BMP 1
          entry.getName(charBuff,sizeof(charBuff));
          if(isBmp(charBuff)){fileCount++;}
          }
        else if(filetype == TYPE_CFG &&  entry.isFile()){ //#define TYPE_CFG 2
          entry.getName(charBuff,sizeof(charBuff));
          if(isCfg(charBuff)){fileCount++;}
          }
      }
    }
    path.close();
    return fileCount;
}

char * getFldNameByIdxCount(char * fldPath, uint16_t fldCountIdx){return getFileNameByIdxCount(fldPath, fldCountIdx, TYPE_FLD);}
char * getBmpNameByIdxCount(char * fldPath, uint16_t bmpCountIdx){return getFileNameByIdxCount(fldPath, bmpCountIdx, TYPE_BMP);}

char * getFileNameByIdxCount(char * fldPath, uint16_t fileCountIdx, uint8_t filetype){
  File32 path;
  bool searchErr = true;
  uint16_t fileCount = 0; 
  if (sd.exists(fldPath) && path.open(fldPath, FILE_READ)){
    if(path.isDir()){
      while(true){
          File32 entry =  path.openNextFile();
          if (! entry) { break; }
          entry.getName(charBuff,sizeof(charBuff));
          if((entry.isFile() && isBmp(charBuff) && filetype == TYPE_BMP) || (entry.isDir() && filetype == TYPE_FLD)){
            if(fileCount == fileCountIdx){searchErr = false; break;}
            fileCount++;
          }
      }
    }
  }
  if(searchErr) {charBuff[0] = '\0';}
  path.close();
  return charBuff;
}

void BMPtoRAM(char* bmpFilePath) {
  
  readBmpHeader(bmpFilePath); // check the header is bmp file is ok (image depth , size , etc.) 
  
  if (bmp.goodBmp) {
    bmp.file.open(bmpFilePath, FILE_READ);
    
    int row, col;
    for (row = 0; row < bmp.h; row++) {
      if (bmp.flip) { bmp.pos = bmp.imageOffset + (bmp.height - 1 - row) * bmp.rowSize;} // Bitmap is stored bottom-to-top order (normal BMP)
      else { bmp.pos = bmp.imageOffset + row * bmp.rowSize;} // Bitmap is stored top-to-bottom

      if (bmp.file.position() != bmp.pos) { // Need seek?
        bmp.file.seek(bmp.pos);
        bmp.buffidx = sizeof(bmp.sdbuffer); // Force buffer reload
      }

      for (col = 0; col < bmp.w; col++) { // For each column…
        if (bmp.buffidx >= sizeof(bmp.sdbuffer)) {// read more pixel data
          bmp.povidx = 0;
          bmp.file.read(bmp.sdbuffer, sizeof(bmp.sdbuffer));
          bmp.buffidx = 0; // Set index to beginning
        }
        // RGB Data is stored backwards -->  BGR
        pixelBuff[col*bmp.w+row][2] = bmp.sdbuffer[bmp.buffidx++];
        pixelBuff[col*bmp.w+row][1] = bmp.sdbuffer[bmp.buffidx++];
        pixelBuff[col*bmp.w+row][0] = bmp.sdbuffer[bmp.buffidx++];

      }         //end col
    }
//    buffHeight = bmp.h ; // when bmp is loaded dimensions are kept here
//    buffWidth = bmp.w ; ////end row
  }     // end good bmp
  else { msgln("BMP format error"); }
  bmp.file.close();
}

void readBmpHeader(char* bmpFilePath) { //store full bmp path in generic charBuff[] and call this funtion with the pointer to it

  bmp.goodBmp = false; // REST to true on valid header parse
  bmp.flip = true; // RESET BMP is stored bottom-to-top

  
  if (!(sd.exists(bmpFilePath) && bmp.file.open(bmpFilePath, FILE_READ))){return;} // maybe throw an error? 
  if (read16(bmp.file) == 0x4D42) { // BMP signature - read16 and read32 are functions below

    bmp.fileSize = read32(bmp.file); // reading the file size
    (void)read32(bmp.file); // Read & ignore creator bytes
    bmp.imageOffset = read32(bmp.file); // Start of image data

    bmp.headerSize = read32(bmp.file); // reading the size of the header (obviously)
    bmp.width = read32(bmp.file);  //  reading the  width of the bmp (obviously)
    bmp.height = read32(bmp.file); //  reading the  height of the bmp (obviously)

    if (read16(bmp.file) == 1) { // # planes — must be ‘1’
      bmp.depth = read16(bmp.file); // bits per pixel

      if ((bmp.depth == 24) && (read32(bmp.file) == 0) && (bmp.width * bmp.height) <= MAXPIXEL) { // 0 = uncompressed and depth must be 24 bit and check if the image fits in the buffer

        bmp.goodBmp = true; // Supported BMP format — proceed

        bmp.rowSize = (bmp.width * 3 + 3) & ~3;    // BMP rows are padded (if needed) to 4-byte boundary
        if (bmp.height < 0) {
          bmp.height = -bmp.height;  // If bmpHeight is negative, image is in top-down order. This is not canon but has been observed in the wild.
          bmp.flip = false;
        }
        bmp.w = bmp.width;
        bmp.h = bmp.height;
      }
    }
  }
  bmp.file.close();

}
uint16_t read16(File32& f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}
uint32_t read32(File32& f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}
// calculate the x & y positions of the led inside the bmp file
//
//void Led2Pixel_static( float alpha1 ) { // angle in rad 
//
//  float halfX = bmp.w / 2;
//  float halfY = bmp.h / 2;
//  int halfLED = NUM_LEDS / 2;
//  float xPerLed = halfX / halfLED;
//  float yPerLed = halfY / halfLED;
//  for (int i = 0; i < halfLED ; i++) {
//    float x = (sin(alpha1) * (xPerLed * (float)i )) + halfX ;
//    float y = (cos(alpha1) * (yPerLed * (float)i )) + halfY ;
////    float x = ((sin16(65535.0/(M_PI*2) * alpha1) / 32767.0) * (xPerLed * (float)i )) + halfX ;
////    float y = ((cos16(65535.0/(M_PI*2) * alpha1) / 32767.0) * (yPerLed * (float)i )) + halfY ;
//    ledPixelPos[halfLED - i - 1][0] = (int)(x + (xPerLed/2))*100; // function would begin in center of cirle with #0 -> #0 in stip is on the outside
//    ledPixelPos[halfLED - i - 1][1] = (int)(y + (yPerLed/2))*100;
//    x= halfX - (x + (xPerLed/2)-halfX);
//    y= halfY - (y + (yPerLed/2)-halfY);
//    ledPixelPos[i + halfLED][0] = (int)(x )*100; // for 2nd half of strip center to outside is ok only adding offset
//    ledPixelPos[i + halfLED][1] = (int)(y )*100;
//  }
//}
void Led2Pixel_static( float alpha1 ) { // angle in rad 

  float halfX = bmp.w / 2;
  float halfY = bmp.h / 2;
  int halfLED = 138 / 2; // 138 is the length of the pseudo LED array
  float xPerLed = halfX / halfLED;
  float yPerLed = halfY / halfLED;
  for (int i = 0; i < halfLED ; i++) {
    float x = (sin(alpha1) * (xPerLed * (float)i )) + halfX ;
    float y = (cos(alpha1) * (yPerLed * (float)i )) + halfY ;
//    float x = ((sin16(65535.0/(M_PI*2) * alpha1) / 32767.0) * (xPerLed * (float)i )) + halfX ; //sin16(angle) --> angle needs to get wrapped around uint16_t
//    float y = ((cos16(65535.0/(M_PI*2) * alpha1) / 32767.0) * (yPerLed * (float)i )) + halfY ; //sin16(angle) --> angle needs to get wrapped around uint16_t
    assignLedArr(halfLED - i - 1, (int)(x + (xPerLed/2))*100, (int)(y + (yPerLed/2))*100 );
//    ledPixelPos[halfLED - i - 1][0] = (int)(x + (xPerLed/2))*100; // 
//    ledPixelPos[halfLED - i - 1][1] = (int)(y + (yPerLed/2))*100;
    x= halfX - (x + (xPerLed/2)-halfX);
    y= halfY - (y + (yPerLed/2)-halfY);
//    ledPixelPos[i + halfLED][0] = (int)(x )*100; // for 2nd half of strip center to outside is ok only adding offset
//    ledPixelPos[i + halfLED][1] = (int)(y )*100;
    assignLedArr(i + halfLED, (int)(x )*100, (int)(y )*100 );
  }
}
// rearrange the leds
void assignLedArr(uint16_t pseudoArrPos, uint16_t colX, uint16_t rowY){
         if (pseudoArrPos == 0){setRealLeds(0,11,colX,rowY);} // ring1 side A
    else if (pseudoArrPos >= 1 && pseudoArrPos <= 12){
      uint16_t tempPos = (12-pseudoArrPos)*2+pseudoArrPos;
      ledPixelPos[tempPos][0] = colX;     ledPixelPos[tempPos][1] = rowY;    // strip 1 side A (reverse)
      ledPixelPos[tempPos+12][0] = colX;  ledPixelPos[tempPos+12][1] = rowY; // strip 2 side A (reverse)
      ledPixelPos[tempPos+24][0] = colX;  ledPixelPos[tempPos+24][1] = rowY; // strip 3 side A (reverse)
      ledPixelPos[tempPos+36][0] = colX;  ledPixelPos[tempPos+36][1] = rowY; // strip 4 side A (reverse)  
      } // (12-in)*2+in
    else if (pseudoArrPos == 13){setRealLeds(60,71,colX,rowY);} // ring 2 side A
    else if (pseudoArrPos >= 14 && pseudoArrPos <= 123){ledPixelPos[pseudoArrPos+58][0] = colX; ledPixelPos[pseudoArrPos+58][1] = rowY;} // center arrays
    else if (pseudoArrPos == 124){setRealLeds(182,193,colX,rowY);} // ring1 side B
    else if (pseudoArrPos >= 125 && pseudoArrPos <= 136){
        ledPixelPos[pseudoArrPos+69][0] = colX;     ledPixelPos[pseudoArrPos+69][1] = rowY;    // strip 1 side B
        ledPixelPos[pseudoArrPos+69+12][0] = colX;  ledPixelPos[pseudoArrPos+69+12][1] = rowY; // strip 2 side B
        ledPixelPos[pseudoArrPos+69+24][0] = colX;  ledPixelPos[pseudoArrPos+69+24][1] = rowY; // strip 3 side B
        ledPixelPos[pseudoArrPos+69+36][0] = colX;  ledPixelPos[pseudoArrPos+69+36][1] = rowY; // strip 4 side B
    }
   else if (pseudoArrPos == 137){setRealLeds(242,253,colX,rowY);} // ring1 side B
}
void setRealLeds( uint16_t fromPos ,uint16_t toPos, uint16_t colX, uint16_t rowY){ for(uint16_t i =  fromPos; i <= toPos; i++){ledPixelPos[i][0] = colX; ledPixelPos[i][1] = rowY;  } }

void Led2Pixel_trails( float alpha1 ) { // angle in rad !!!
  //1x px col / degree = M_PI/180
}
// grap the x & y data (rgb value) and set the led array accordingly
void drawRamPixel(bool interpolation) {
  
  int row, col,overRow,overCol;
  float blendRow, blendCol;
  CRGB p1, p2, p3, p4;
  for (int i = 0; i < NUM_LEDS; i++) {
    col = ledPixelPos[i][0]/100; //
    row = ledPixelPos[i][1]/100; //
    p1 = getPixel(col,row);
    if(interpolation){ //Bilinear interpolation
      blendCol = ((float)ledPixelPos[i][0]/100.0)-((int)ledPixelPos[i][0]/100.0); // range from 0 to 1
      blendRow = ((float)ledPixelPos[i][1]/100.0)-((int)ledPixelPos[i][1]/100.0); // range from 0 to 1
      blendCol = (blendCol-0.5)*2*255; // range from -255 to 255
      blendRow = (blendRow-0.5)*2*255; // range from -255 to 255
      overCol = col + ((int)blendCol/abs((int)blendCol)); // set overlay column to col +/- 1
      overRow = row + ((int)blendRow/abs((int)blendRow)); // set overlay row to row +/- 1
      p2 = getPixel(overCol,row);
      p3 = getPixel(col,overRow);
      p4 = getPixel(overCol,overRow);
      nblend(p1,p2,(int)abs(blendCol));
      nblend(p3,p4,(int)abs(blendCol));
      nblend(p1,p3,(int)abs(blendRow));
    }
    leds[i] = p1;
  }
}

uint16_t arrayPos(uint16_t colX, uint16_t rowY){return (colX*bmp.w+rowY);}
CRGB getPixel(uint16_t colX, uint16_t rowY){ return CRGB(pixelBuff[arrayPos(colX,rowY)][0],pixelBuff[arrayPos(colX,rowY)][1],pixelBuff[arrayPos(colX,rowY)][2]);}


void rollStaticTest(){

float OFF_ROLL1 = 0.71559;  // rotational offset (rad) flower 1 roll axis
float OFF_ROLL2 = 1.727876; // rotational offset (rad) flower 2 roll axis
  for(int i = 0; i <12; i++){
  //  cos(roll)*127 +127 // 205  194
  //-sin(roll)*127 +127 // 241 230
  //-cos(roll)*127 +127 // 229 218
  //sin(roll)*127 +127 // 217 206
//  leds[i+12] = CHSV((-sin16(65535.0/(M_PI*2) * (roll+OFF_ROLL1)) / 32767.0) * 127 +127,255,255);
//  leds[i+24] = CHSV(( cos16(65535.0/(M_PI*2) * (roll+OFF_ROLL1)) / 32767.0) * 127 +127,255,255);
//  leds[i+36] = CHSV(( sin16(65535.0/(M_PI*2) * (roll+OFF_ROLL1)) / 32767.0) * 127 +127,255,255);
//  leds[i+48] = CHSV((-cos16(65535.0/(M_PI*2) * (roll+OFF_ROLL1)) / 32767.0) * 127 +127,255,255);
//
//  leds[i+206] = CHSV((-sin16(65535.0/(M_PI*2) * (roll+OFF_ROLL2)) / 32767.0) * 127 +127,255,255);
//  leds[i+218] = CHSV(( cos16(65535.0/(M_PI*2) * (roll+OFF_ROLL2)) / 32767.0) * 127 +127,255,255);
//  leds[i+230] = CHSV(( sin16(65535.0/(M_PI*2) * (roll+OFF_ROLL2)) / 32767.0) * 127 +127,255,255);
//  leds[i+194] = CHSV((-cos16(65535.0/(M_PI*2) * (roll+OFF_ROLL2)) / 32767.0) * 127 +127,255,255);

  //mini led strips = +/- 3,5° offset from center
  //              ring strip ring center ring strip ring
  // virtual leds = 1 + 12  + 1  + 110  + 1  + 12  + 1 = 138 leds
  // real LED length = 254

  leds[i+12] = CHSV( sin(roll+OFF_ROLL1)*127 +127,255,255);
  leds[i+24] = CHSV( cos(roll+OFF_ROLL1)*127 +127,255,255);
  leds[i+36] = CHSV(-sin(roll+OFF_ROLL1)*127 +127,255,255);
  leds[i+48] = CHSV(-cos(roll+OFF_ROLL1)*127 +127,255,255);

  leds[i+194] = CHSV( cos(roll+OFF_ROLL2)*127 +127,255,255);
  leds[i+206] = CHSV( sin(roll+OFF_ROLL2)*127 +127,255,255);
  leds[i+218] = CHSV(-cos(roll+OFF_ROLL2)*127 +127,255,255);
  leds[i+230] = CHSV(-sin(roll+OFF_ROLL2)*127 +127,255,255);
  
  }
}
