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
// calculate the x & y positions of the led projected on the bmp file
//

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


/*
  1) calculate X/Y bitmap pixel pos. for all vitual LED strip LEDs
  2) invert LED order of the mini stip un the cpu side
  3) caculate the roll16 correction but save correction as int correction=(32768-LedOff16)
  4) apply correction to roll16 to get (int)distAngle = (int)roll16 + correction
  5) wrap around (int)distAngle with wrap_u16() to get uint16_t and calculate the sin16(uint16_t distAngle) (sin16 input: 0 to 65335 output -32767 to 32767)

*/   

void Led2Pixel_static() { // angle in rad 

  float halfX = (float)bmp.w / 2.0;
  float halfY = (float)bmp.h / 2.0;
  float xPerLed = (float)bmp.w / (float)NUM_VLEDS;
  float yPerLed = (float)bmp.h / (float)NUM_VLEDS;
  int halfLED = NUM_VLEDS/2;

  ledVector[0] = ((float)sin16(yaw16)/32767.0) * xPerLed; // store the current led vecor
  ledVector[1] = ((float)cos16(yaw16)/32767.0) * yPerLed; // store the current led vecor

  for (int i = 0; i < halfLED ; i++) {
    float x = ledVector[0] * i + (xPerLed / 2.0); // scale the led vector to current index  
    float y = ledVector[1] * i + (yPerLed / 2.0); // scale the led vector to current index  
    
    virt2realLed(halfLED - i - 1, x + halfX,y + halfY); // side A
    virt2realLed(halfLED + i, -x + halfX, -y + halfY);  // side B with inverted pixel vector
  }
}
void virt2realLed(uint16_t vIdx, float x, float y){
  if(vIdx == 0 || vIdx == 13 || vIdx == 124 || vIdx == 137){set12RingPx(vIdx, x, y);} //set all 12 ring LED pixels
  else if ( vIdx >= 1 && vIdx <= 12){setMiniStripsPx(vIdx, x, y, 0 );}   // cpu side mini srips ( reverse order)
  else if ( vIdx >= 14 && vIdx <= 123){ ledPixelPos[vIdx+58][0] = x*100; ledPixelPos[vIdx+58][1] = y*100; } //multiply the XY result by 100 for blending function
  else if ( vIdx >= 125 && vIdx <= 136 ){setMiniStripsPx(vIdx, x, y, 1);}  // battery side mini srips
}
/*
 * 
//create 1 pixel vector from center pixel (P1) to next pixel (P2) --> Vector = P2-P1
//ad rotate vector by 90° cw --> Vector_X * -1, then swap Vector_X & Vector_Y
// then scale the vector to max pixel offset distance 90° from stick (max dist 15mm => 5 pixel) (constant: maxRollPx)

        // vector of one LED distance pointing from the center to cpu side
        ledVector[0] 
        ledVector[1]
        // vecor of one LED distance but perpendicular (clockwise) to the stick
        ledVecor90cw[0]  =  ledVector[1] // for roll angle distance of CPU side LEDs
        ledVecor90cw[1]  = -ledVector[0] // for roll angle distance of CPU side LEDs
        // vecor of one LED distance but perpendicular (counter clockwise) to the stick
        ledVecor90ccw[0] = -ledVector[1] // for roll angle distance of battery side LEDs
        ledVecor90ccw[1] =  ledVector[0] // for roll angle distance of battery side LEDs
        // multiply by rollDistMaxPx and then scale down by specific sin() of the difference between the roll angle and each LEDs offset angle
        angle16Ring[0 to 11] relative led offset
        getOffScaling();
        retrive scaling factor for ring leds:
                          wrap to uint16_t , starting offset 1st led, -1 or 1  relative offset to 1st ring led
        float a = getOffScaling( wrap_u16(ringOff16[1 to 4] + ring_Data_direction * angle16Ring[0 to 11])
*/


void set12RingPx(uint16_t vIdx, float vx, float vy){
  
  for(uint8_t i = 0; i < 12; i++){
    float coef = 0;
    uint16_t real1stIdx;
       if(vIdx == 0  ){ coef = getOffScaling( wrap_u16(ringOff16[0] + angle16Ring[i])); real1stIdx = 0;}
  else if(vIdx == 13 ){ coef = getOffScaling( wrap_u16(ringOff16[1] - angle16Ring[i])); real1stIdx = 60; }
  else if(vIdx == 124){ coef = getOffScaling( wrap_u16(ringOff16[2] + angle16Ring[i])); real1stIdx = 182; }
  else if(vIdx == 137){ coef = getOffScaling( wrap_u16(ringOff16[3] - angle16Ring[i])); real1stIdx = 242; }
  //multiply the XY result by 100 for blending function
  ledPixelPos[real1stIdx + i][0] = (vx + ( ledVector[1] * coef ))*100; // ledVector[] is here already rotated by 90°  and maybe inverted when coef is <0
  ledPixelPos[real1stIdx + i][1] = (vy + (-ledVector[0] * coef ))*100; // ledVector[] is here already rotated by 90°  and maybe inverted when coef is <0
  }
}

void setMiniStripsPx(uint16_t vIdx, float vx, float vy, uint8_t side){
  
  uint16_t rIdx; // real led index of the nightstick
  for(int i = 0; i < 4; i++){
      if(side == 0){ rIdx = stripLedPos[side][i] - vIdx;} // to invert the led order
      else{ rIdx = stripLedPos[side][i] + vIdx;}
      float coef = getOffScaling(stripOff16[side][i]);
  ledPixelPos[rIdx][0] = (vx + ( ledVector[1] * coef ))*100; //multiply the XY result by 100 for blending function
  ledPixelPos[rIdx][1] = (vy + (-ledVector[0] * coef ))*100; //multiply the XY result by 100 for blending function
  }
}

//float sinRingDist(uint16_t ringOffFrist, int rotDir, uint8_t relIdx){ return ((float)sin16(wrap_u16(ringOffFrist + (rotDir * angle16Ring[relIdx]))))/32767.0;}
uint16_t wrap_u16(int unwrapped_u16){return (uint16_t)(unwrapped_u16 % 65536);}

//input: led offset as uint16_t 0 to 65535 --> 0° to 360°
//output -6 to 6 which is the scaling factor for the 90° rotated led-pixel-distance vector
float getOffScaling(int ledOff16){return  ((float)sin16( wrap_u16(roll16 +(32768 - ledOff16)) )/32767.0) * -rollDistMaxPx;} // 32768 is 180° where the sin is 0
  
