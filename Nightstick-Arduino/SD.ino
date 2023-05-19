void setup_SD(){
  if (!sd.begin(SD_CONFIG)) { 
    error(1);
    //sd.initErrorHalt(&Serial);
    }
}
void testFileIdx(){
  msgln("Testing print bmps in folder for /Nightstick/BMPs/trails/colorless/");
  printBmpsInFld("/Nightstick/BMPs/trails/colorless/");
  msgln("");
  msg("bitmaps in this folder: \t");
  msgln(i2char(bmpCountInFld(SUBFLD_TRAIL,"colorless/")));
  msg("bitmaps in this folder(2nd count method): \t");
  msgln(i2char(bmpCountInFld("/Nightstick/BMPs/trails/colorless/")));
  msgln("");
  msgln("Testing print sub fld in folder for /Nightstick/BMPs/trails/");
  printSubFld("/Nightstick/BMPs/trails/");
  msg("folder in this folder: \t");
  msgln(i2char(fldCountInFld("/Nightstick/BMPs/trails/")));
  msg("folder in this folder(2nd count method): \t");
  msgln(i2char(fldCountInFld("trails")));
//    charBuff[0]='\0'; // clear buffer
//    strcpy(charBuff,MAINPATH);
//    strcat(charBuff,SUBFLD_BMP);
//    if (sd.exists(charBuff) && file.open(charBuff, FILE_READ)){printDirectory(file,0); }
//    file.close();
  
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
void printBmpsInFld(char * fld){
 File32 path;
 
if (sd.exists(fld) && path.open(fld, FILE_READ)){
    while(true) {
       File32 entry =  path.openNextFile();
      if (! entry) { msgln("**nomorefiles**"); break; }
      entry.getName(charBuff,sizeof(charBuff));
      if(isBmp(charBuff)&& entry.isFile()){msgln(charBuff);}
    }
  }
  path.close();
  
}

void printSubFld(char * fld){
 File32 path;  
if (sd.exists(fld) && path.open(fld, FILE_READ)){
    while(true) {
       File32 entry =  path.openNextFile();
      if (! entry) { msgln("**nomorefiles**"); break; }
      if(entry.isDir()){
        entry.getName(charBuff,sizeof(charBuff));
        msgln(charBuff);
        }
    }
  }
  path.close();
}
char * buildtBmpFld(char * categoryFld,const char * subFld){

    (void)buildtCategoryFld(categoryFld);
    if(subFld[0] != '/'){strcat(charBuff,"/");}
    strcat(charBuff,subFld);
    return charBuff;
}
char * buildtCategoryFld(char * categoryFld){
    charBuff[0]='\0'; // clear buffer
    strcpy(charBuff,MAINPATH);
    strcat(charBuff,SUBFLD_BMP);
    if(categoryFld[0] != '/'){strcat(charBuff,"/");}
    strcat(charBuff,categoryFld);
    return charBuff;
}

int16_t bmpCountInFld(char * fullFldPath){return filetypeInFld(fullFldPath,TYPE_BMP);}
int16_t bmpCountInFld(char * categoryFld, char * subFld){ return filetypeInFld(buildtBmpFld(categoryFld,subFld),TYPE_BMP);}
int16_t fldCountInFld(char * path){
  if( cArrIndexOf((cArrStartsWith(path,'/') ? &path[1] : &path[0]), '/' ) == -1){return filetypeInFld(buildtCategoryFld(path),TYPE_FLD);}
  else{ return filetypeInFld(path,TYPE_FLD); }
}


//int16_t filetypeInFld(static_cast<char *>(fldPath), uint8_t filetype){
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

// how to load bmp:
// clear charBuff[] ( charBuff[0] = '\0'; )
// create full filepath to bmp file and store it in charBuff[]
// call BMPtoRAM(charBuff); it will analyze the header file auomaticly
// file is now saved to ram
// every frame update call: 
//---------------------------------
//Led2Pixel(new_angle_in_rad); to obtain newest xy position from the leds on the image
//and then drawRamPixel(); to put the rgb data corrsponding to the XY data into the led array
// finally call ledsShow() to update the stick

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
        bmp.b = bmp.sdbuffer[bmp.buffidx++];
        bmp.g = bmp.sdbuffer[bmp.buffidx++];
        bmp.r = bmp.sdbuffer[bmp.buffidx++];
        // Store BMP Data in Array
        //if( col < 100 && row < 100){
        pixelBuff[col*row+col][0] = bmp.r;
        pixelBuff[col*row+col][1] = bmp.g;
        pixelBuff[col*row+col][2] = bmp.b;

      }         //end col
    }
    buffHeight = bmp.h ; // when bmp is loaded dimensions are kept here
    buffWidth = bmp.w ; ////end row
  }     // end good bmp
  else { msgln("BMP format error"); }
  bmp.file.close();
}

//uint16_t countFiles(File32 path){
//  int fileCount = 0;
//   while (file.openNextFile(&path, O_RDONLY){fileCount++; file.close(); }
//  return fileCount;
//}
//
//
//const char * changeEntry(File32 path, int8_t moveDir){
//  
//  bool pathIsFolder = path.isDirectory();
// path.dirEntry(dir);
// uint16_t fileCount = countFiles(dir);
// uint16_t skippedFiles = 0;
//  uint16_t tmpIdx = path.dirIndex();
//  while(true){
//       if(moveDir == -1 && tmpIdx == 0){tmpIdx = fileCount-1;}
//  else if(moveDir  < 0 && tmpIdx  >  0){tmpIdx --;}
//  else if(moveDir  > 0 && tmpIdx  >= fileCount-1){tmpIdx = 0;}
//  else if(moveDir  > 0 && tmpIdx  < fileCount-1){tmpIdx++;}
//    file.open(dir, tmpIdx,O_RDONLY);
//    charBuff[0] = '\0';
//    file.getName(charBuff,sizeof(charBuff));
//    if(file.isDirectory() && pathIsFolder){file.close(); return charbuff;}
//    else if(file.isFile() && !pathIsFolder && isBmp(charBuff){file.close(); return charbuff;}
//     if(skippedFiles >= fileCount){ file.close(); charBuff[0] = '\0'; return charBuff,}
//     file.close();
//     skippedFiles++;
//  }
//}

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
void Led2Pixel( float alpha1 ) { // angle in rad !!!

  //  float alpha1 = alpha;//*M_PI/180;                        //convert angle to radian SIDE A
  // if(alpha1 > 2*M_PI){alpha1=2*M_PI;}

  float alpha2 = (alpha1 + M_PI) ;// *M_PI/180;                 //convert angle to radian SIDE B

  float halfX = buffWidth / 2;
  float halfY = buffHeight / 2;
  int halfLED = NUM_LEDS / 2;
  float xPerLed = halfX / halfLED;
  float yPerLed = halfY / halfLED;
  for (int i = 0; i < halfLED ; i++) {
    //   ONLY USE  float vars to avoid approximation errors  in process
    //       sin(a) * (pixellength per led * posnr) +offset 1st led from center
    float x = (sin(alpha1) * (xPerLed * (float)i )) + halfX ;
    float y = (cos(alpha1) * (yPerLed * (float)i )) + halfY ;
    



    //testc = alpha1;
    ledPixelPos[halfLED - i - 1][0] = (int)x + 1; // function would begin in center of cirle with #0 -> #0 in stip is on the outside
    ledPixelPos[halfLED - i - 1][1] = (int)y + 1;
//    pixelFadeVal[halfLED -i][0] = floatToSfract15 (((x - (int)x) * 2)-1);
//    pixelFadeVal[halfLED -i][1] = floatToSfract15 (((y - (int)y) * 2)-1);
    
    // SIDE B of STICK
    x= halfX - (x-halfX);
    y= halfY - (y-halfY);
//    x = (sin(alpha2) * (xPerLed * i )) + halfX ;
//    y = (cos(alpha2) * (yPerLed * i )) + halfY ;

    ledPixelPos[i + halfLED][0] = (int)x + 1; // for 2nd half of strip center to outside is ok only adding offset
    ledPixelPos[i + halfLED][1] = (int)y + 1;
  
  }
  //            ledPixelPos[35][0] = 50; //
  //            ledPixelPos[35][1] =  50;// measured center pixel gets middle
}

// grap the x & y data (rgb value) and set the led array accordingly
void drawRamPixel() {
int row, col;
  for (int i = 0; i < NUM_LEDS; i++) {

// old way with 2d[rgb] grid
//    leds[i].r = pixelData[ledPixelPos[i][0]][ledPixelPos[i][1]][0] ;
//    leds[i].g = pixelData[ledPixelPos[i][0]][ledPixelPos[i][1]][1] ;
//    leds[i].b = pixelData[ledPixelPos[i][0]][ledPixelPos[i][1]][2] ;

    col = ledPixelPos[i][0];
    row = ledPixelPos[i][1];
    
        leds[i].r = pixelBuff[col*row+col][0];
        leds[i].g = pixelBuff[col*row+col][1];
        leds[i].b = pixelBuff[col*row+col][2];
  }
}
