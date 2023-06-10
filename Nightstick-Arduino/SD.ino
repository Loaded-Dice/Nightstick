/*
  Load next / last bmp and folder:
  ledsChangeBmp(int8_t loadDirection) // LAST -1 , NEXT 1
  ledsChangeFld(int8_t loadDirection) // LAST -1 , NEXT 1
  

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
void removeBmp(char * pathToBmp){
  if(sd.exists(pathToBmp)){
    if(sd.remove(pathToBmp)){Serial.print("removed : ");Serial.print('\t');Serial.print(pathToBmp);}
    else{Serial.print("Error - can't remove : ");Serial.print('\t');Serial.print(pathToBmp);}
    }
  else{Serial.print("file does not exist : ");Serial.print('\t');Serial.print(pathToBmp);}
}

void printBmpHeader(){
    Serial.print("fileSize"); Serial.print('\t');Serial.println(bmp.fileSize);
    Serial.print("headerSize"); Serial.print('\t');Serial.println(bmp.headerSize);
    Serial.print("width"); Serial.print('\t');Serial.println(bmp.width); // BMP image width in pixel
    Serial.print("height"); Serial.print('\t');Serial.println(bmp.height); // BMP image height in pixel
    Serial.print("depth"); Serial.print('\t');Serial.println(bmp.depth); // Bit depth (currently must report 24)
    Serial.print("imageOffset"); Serial.print('\t');Serial.println(bmp.imageOffset); // Start of image data in file
    Serial.print("rowSize"); Serial.print('\t');Serial.println(bmp.rowSize); // Not always = bmpWidth; may have padding
    Serial.print("goodBmp"); Serial.print('\t');Serial.println(bmp.goodBmp) ;
    Serial.print("flip"); Serial.print('\t');Serial.println(bmp.flip);
}

void ledsChangeBmp(int8_t loadDirection){ // LAST -1 , NEXT 1
  
  if(ledMode == LED_TRAIL ){
    strcpy(cfg.trailsBmp, getOtherBmp(toPathBuff(FULLPATH_TRAILS, cfg.trailsFolder, true) ,cfg.trailsBmp, loadDirection)); 
    Serial.println(charBuff);
    BMPtoRAM(toCharBuff(pathBuff,cfg.trailsBmp,true));
    }
  else if(ledMode == LED_STATIC){
    strcpy(cfg.staticBmp, getOtherBmp(toPathBuff(FULLPATH_STATIC, cfg.staticFolder, true) ,cfg.staticBmp, loadDirection)); 
    Serial.println(charBuff);
    BMPtoRAM(toCharBuff(pathBuff,cfg.staticBmp,true));
    }
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
      else{msgln("BAD BITMAP"); Serial.println(bmpFilePath);}
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
