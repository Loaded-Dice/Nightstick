
void setup_LEDs(){
  memset(pixelBuff,0,sizeof(pixelBuff));
  strip.begin();
  strip.setBrightness(cfg.bright);
  strip.show(); // Initialize all pixels to 'off'
  ledsClear();
}

void main_LEDs(){
 
if(ledMode == LED_OFF && ledModeLast == LED_OFF){return;}

 if(ledMode == LED_OFF && ledModeLast != ledMode){ledsClear();ledsClear();ledsClear();}
    
   EVERY_N_MILLIS(DELAYMS_SHOWLED) {
    gHue++;
    //if(ledMode != ledModeLast && ledMode == LED_STATIC){chkBmpLoaded();};
     if (ledMode != LED_STATIC && ledMode != LED_TRAIL && blendMulti == 100){blendMulti = 1;} //set multiplier for blending
    switch (ledMode) {
      case LED_STATIC: Led2Pixel_static(); drawRamPixel(true);  break;
      case LED_TRAIL:  Led2Pixel_trails(); drawRamPixel(true);  break; // only for testing Led2Pixel_static is for static bmps!
      case LED_ANI: ani_main();  break; //RGBorbit(); plasma(); waveRings();
      case LED_FIRE: make_fire();  break;
      case LED_BRIGHT:  break;
      case LED_BATT:  break;
      case LED_BLE:  break;
      case LED_TEST:   break;
      default:  break; 
    }
  ledsShow();
  }
  paletteFade();
  ledModeLast = ledMode; 
}


void fastToNeo(){for(int i = 0; i < NUM_LEDS; i++){ strip.setPixelColor(i, leds[i].r, leds[i].g, leds[i].b); }}

void ledsShow(){ fastToNeo(); strip.show();}

void ledsClear(){fill_solid(leds, NUM_LEDS, CRGB::Black); ledsShow();} 

void mirrorStick(uint8_t side){ // 0 or 1
  for(int i = 0; i < NUM_LEDS/2; i++){
    if(side == 0){
      int16_t sourcePos = (NUM_LEDS/2)-1-i;
      if(sourcePos >= 12 && sourcePos <= 48){ leds[sourcePos+182]=leds[sourcePos]; }
      else{leds[127+i] = leds[126-i]; }
    }
    else if(side == 1){
      int16_t sourcePos = (NUM_LEDS/2)+i;
      
    }
  }
}
/*
   how to load bmp to POV display:
   clear charBuff[] ( charBuff[0] = '\0'; )
   create full filepath to bmp file and store it in charBuff[]
   call BMPtoRAM(charBuff); it will analyze the header file auomaticly
   Example: BMPtoRAM("/Nightstick/BMPs/trails/pattern/chevron_multi_black.bmp")
   file is now saved to ram
   every frame update call: 
  ---------------------------------
  Led2Pixel_static(); to obtain newest xy position from the leds on the image
  and then drawRamPixel(); to put the rgb data corrsponding to the XY data into the led array
  finally call ledsShow() to update the stick

*/
//     BMPtoRAM("/Nightstick/BMPs/trails/pattern/chevron_multi_black.bmp")

// calculate the x & y positions of the led projected on the bmp file

void Led2Pixel_trails(){
  float deltaYaw = ((float)yaw16 - (float)yaw16Last)/182.0 * trailSpeed; // == x / 182 units per degree * scrolling speed (px/ rotation-delta)
  colTrail = wrap_float(colTrail + deltaYaw, bmp.w);
  int halfLED = NUM_VLEDS/2;
  int row = 0;
  for (int i = 0; i < halfLED ; i++) {
    row =  i % bmp.h ;
    virt2realLed(halfLED - i - 1, colTrail,row); // side A
    virt2realLed(halfLED + i,     colTrail,row);  // side B with inverted pixel vector
  }
  yaw16Last = yaw16;
}

float wrap_float(float input, float maxVal){ 

  if(input < 0.0){
    while(input < 0.0){input += maxVal; }
    return input;
  }
  else if(input > maxVal){
    while(input > maxVal){ input -= maxVal; }
    return input;
  }
  return input;
}

void drawRamPixel(bool interpolation) {
  
  int row, col,overRow,overCol;
  int8_t blendDirCol = 0; // 1 or 0 or -1
  float blendRow, blendCol;
  CRGB p1, p2, p3, p4;
  for (int i = 0; i < NUM_LEDS; i++) {
    col = uint16_t(ledPixelPos[i][0]/100.0); //
    row = uint16_t(ledPixelPos[i][1]/100.0); //
    p1 = getPixel(col,row);
    
    if(interpolation){ //Bilinear interpolation
      
      blendCol = ((float)ledPixelPos[i][0]/100.0)-((int)(ledPixelPos[i][0]/100.0)); // range from 0 to 1
      blendRow = ((float)ledPixelPos[i][1]/100.0)-((int)(ledPixelPos[i][1]/100.0)); // range from 0 to 1
      blendCol = (blendCol-0.5)*255; // range from -255 to 255
      blendRow = (blendRow-0.5)*255; // range from -255 to 255
      overCol = col + ((int)blendCol/abs((int)blendCol)); // set overlay column to col +/- 1
      
      if(ledMode == LED_TRAIL){ col = wrap_int(col,bmp.w); overCol = wrap_int(overCol,bmp.w);}
      
      if(blendCol != 0.0){ p2 = getPixel(overCol,row); nblend(p1,p2,(uint8_t)abs(blendCol)); }
      
      if(ledMode == LED_STATIC){
        overRow = row + ((int)blendRow/abs((int)blendRow)); // set overlay row to row +/- 1
        p3 = getPixel(col,overRow);
        p4 = getPixel(overCol,overRow);
        nblend(p3,p4,(int)abs(blendCol));
        nblend(p1,p3,(int)abs(blendRow));
      }
    }
    leds[i] = p1;
  }
}

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

// Use THIS function below to distribute virual LED ( vIdx (virtual index) 0 to 137. 
// Also the X&Y positions are the real pixel positions and get scaled up automatically by ledMode
// to achieve higher precision within the range of an uint16_t. For LED_STATIC && LED_TRAIL its x100
// otherwise x1
// input the virtual LED index vIdx together with the virtual X&Y position 
// (vx & vy should have been alread rotated by the yaw angle but not by the roll angle)

void virt2realLed(uint16_t vIdx, float vx, float vy){ // this function sets ledPixelPos[][] according to roll angle
  
       if ( vIdx == 0 || vIdx == 13 || vIdx == 124 || vIdx == 137){ set12RingPx(vIdx, vx, vy); } //set all 12 ring LED pixels
  else if ( vIdx >= 1 && vIdx <= 12){setMiniStripsPx(vIdx, vx, vy, 0 );}   // cpu side mini srips ( reverse order)
  else if ( vIdx >= 14 && vIdx <= 123){ ledPixelPos[vIdx+58][0] = (uint16_t)(vx*blendMulti); ledPixelPos[vIdx+58][1] = (uint16_t)(vy*blendMulti); } //multiply the XY result by 100 for blending function
  else if ( vIdx >= 125 && vIdx <= 136 ){setMiniStripsPx(vIdx, vx, vy, 1);}  // battery side mini srips
}

void virt2realLed(uint16_t vIdx, CRGB pixel){ // this funtion does'nt make use of the roll angle and each mini strips of one sinde are equal
  uint16_t rIdx;
  if ( vIdx == 0 || vIdx == 13 || vIdx == 124 || vIdx == 137){
           if(vIdx == 0  ){ rIdx = 0;  }
      else if(vIdx == 13 ){ rIdx = 60; }
      else if(vIdx == 124){  rIdx = 182;}
      else if(vIdx == 137){  rIdx = 242;}
      for(uint8_t i = 0; i < 12; i++){  leds[rIdx +i] = pixel; }
  }
  else if ( vIdx >= 1 && vIdx <= 12){for(int i = 0; i < 4; i++){  leds[stripLedPos[0][i] - vIdx] = pixel; }}
  else if ( vIdx >= 14 && vIdx <= 123){leds[vIdx+58] = pixel;}
  else if ( vIdx >= 125 && vIdx <= 136 ){for(int i = 0; i < 4; i++){  leds[stripLedPos[1][i] + vIdx] = pixel; }}

}

int16_t getRealFirstIdx(uint16_t vIdx){ // return the first led of the corresponding real led array pos. section - after placing the colors they need to get spread out with spreadRealFirstIdx()
      if(vIdx == 0  ){ return 0;  }
      else if (vIdx >= 1 && vIdx <= 12){ return stripLedPos[0][0] - vIdx; }
      else if(vIdx == 13 ){ return 60; }
      else if(vIdx >= 14 && vIdx <= 123){return  vIdx+58 ;}
      else if(vIdx == 124){ return 182;}
      else if(vIdx >= 125 && vIdx <= 136 ){return stripLedPos[1][0] + vIdx; } 
      else if(vIdx == 137){ return 242;}
      else{ return -1;}
}

void spreadRealFirstIdx(){
  for(uint8_t i = 1; i < 12; i++){  // spread out all rings
    leds[0 + i] = leds[0]; 
    leds[60 + i] = leds[60];
    leds[182 + i] = leds[182];
    leds[242 + i] = leds[242];
  }
  for(uint8_t i = 0; i < 12; i++){
    for(uint8_t j = 1; j < 4; j++){
      leds[stripLedPos[0][j] - i] =  leds[stripLedPos[0][0] - i];
      leds[stripLedPos[1][j] + i] =  leds[stripLedPos[1][0] + i];
    }
  }
}

void set12RingPx(uint16_t vIdx, float vx, float vy){
  
  for(uint8_t i = 0; i < 12; i++){
    float coef = 0; // can be positiv or negative  to point the offset in different directions
    uint16_t real1stIdx;
       if(vIdx == 0  ){ coef = getOffScaling( wrap_u16(ringOff16[0] + angle16Ring[i])); real1stIdx = 0;  }
  else if(vIdx == 13 ){ coef = getOffScaling( wrap_u16(ringOff16[1] - angle16Ring[i])); real1stIdx = 60; }
  else if(vIdx == 124){ coef = getOffScaling( wrap_u16(ringOff16[2] + angle16Ring[i])); real1stIdx = 182;}
  else if(vIdx == 137){ coef = getOffScaling( wrap_u16(ringOff16[3] - angle16Ring[i])); real1stIdx = 242;}
  //multiply the XY result by 100 for blending function


      if(ledMode == LED_STATIC || ledMode == LED_ANI){
        ledPixelPos[real1stIdx + i][0] = (vx + ( ledVector[1] * coef ))*blendMulti; // ledVector[] is here already rotated by 90°  and maybe inverted when coef is <0
        ledPixelPos[real1stIdx + i][1] = (vy + (-ledVector[0] * coef ))*blendMulti; // ledVector[] is here already rotated by 90°  and maybe inverted when coef is <0
      }
      else if( ledMode == LED_TRAIL ){
        ledPixelPos[real1stIdx + i][0] = wrap_float(vx + coef, bmp.w)*blendMulti; // ledVector[] is here already rotated by 90°  and maybe inverted when coef is <0
        ledPixelPos[real1stIdx + i][1] = vy*blendMulti; // ledVector[] is here already rotated by 90°  and maybe inverted when coef is <0

      }
  }
}

void setMiniStripsPx(uint16_t vIdx, float vx, float vy, uint8_t side){
  
  uint16_t rIdx; // real led index of the nightstick
  for(int i = 0; i < 4; i++){
      if(side == 0){ rIdx = stripLedPos[side][i] - vIdx;} // to invert the led order
      else{ rIdx = stripLedPos[side][i] + vIdx;}
      float coef = getOffScaling(stripOff16[side][i]);
      
      if(ledMode == LED_STATIC || ledMode == LED_ANI){
        ledPixelPos[rIdx][0] = (vx + ( ledVector[1] * coef ))*blendMulti; //multiply the XY result by 100 for blending function
        ledPixelPos[rIdx][1] = (vy + (-ledVector[0] * coef ))*blendMulti; //multiply the XY result by 100 for blending function
      }
      else if( ledMode == LED_TRAIL ){
        ledPixelPos[rIdx][0] = wrap_float(vx +  coef, bmp.w )*blendMulti; //multiply the XY result by 100 for blending function
        ledPixelPos[rIdx][1] =    (vy * blendMulti); //multiply the XY result by 100 for blending function
      }
  }
}

// returns a CRGB object from the desired image x&y pixel position
CRGB getPixel(uint16_t colX, uint16_t rowY){ return CRGB(pixelBuff[arrayPos(colX,rowY)][0],pixelBuff[arrayPos(colX,rowY)][1],pixelBuff[arrayPos(colX,rowY)][2]);} // recive the bmp pixel color

//used by getPixel() (above) to convert X & Y into pixel buffer array positions of the bitmap file
uint16_t arrayPos(uint16_t colX, uint16_t rowY){return (colX*bmp.w+rowY);} //convert the 2D X/Y information to the flat pixelBuff layout

unsigned int wrap_int(int input, int maxVal){ if(input >= maxVal){ return (input - maxVal);} if(input < 0){return (input + maxVal); } return input;}

uint16_t wrap_u16(int unwrapped_u16){return (uint16_t)(unwrapped_u16 % 65536);}

//input: led offset as uint16_t 0 to 65535 --> 0° to 360°; return: -6 to 6 which is the scaling factor for the 90° rotated led-pixel-distance vector
float getOffScaling(int ledOff16){return  ((float)sin16( wrap_u16(roll16 +(32768 - ledOff16)) )/32767.0) * -rollDistMaxPx;} // 32768 is 180° where the sin is 0

//CHSV rgb2hsv_approximate( const CRGB& rgb);
CHSV rgb2hsv(CRGB input) {
  CHSV output;
    double rd = (double) input.r/255.0;
    double gd = (double) input.g/255.0;
    double bd = (double) input.b/255.0;
    double tw_max = max(rd, max(gd, bd)); // calculate the threeway max
    double tw_min = min(rd, min(gd, bd)); // calculate the threeway min
    double h, s, v = tw_max;

    double d = tw_max - tw_min;
    s = tw_max == 0 ? 0 : d / tw_max;

    if (tw_max == tw_min) {  h = 0;  } // achromatic
    else {
        if (tw_max == rd) { h = (gd - bd) / d + (gd < bd ? 6 : 0);} 
        else if (tw_max == gd) { h = (bd - rd) / d + 2;} 
        else if (tw_max == bd) { h = (rd - gd) / d + 4;}
        h /= 6;
    }
    output.h = h*255;
    output.s = s*255;
    output.v = v*255;
    return output;
}

uint16_t safe(int ledIdx){ return ledIdx % NUM_LEDS;}


// input -64 to 64 | output -128 to 128
int8_t getQuadSpeed(int8_t gravAngle){ return (gravAngle / abs(gravAngle)) * (255-quadwave8(128+gravAngle)) ;}

//CRGB   ColorFromPalette (const CRGBPalette16 &pal, uint8_t index, uint8_t brightness=255, TBlendType blendType=LINEARBLEND)
CRGB getCurrentPalColor(uint8_t hue, uint8_t sat, uint8_t bright = 255){
  CRGB ledColor;
  if(paletteActive){ ledColor = ColorFromPalette(currentPal, hue, bright, LINEARBLEND); }
  else{ledColor = CHSV(hue,sat,bright);}
  return ledColor;
}

CRGB getCurrentPalColor(CHSV input){
  CRGB ledColor = input;;
  if(paletteActive){ ledColor = ColorFromPalette(currentPal, input.h, input.v, LINEARBLEND); }
  return ledColor;
}

CRGB getTargetPalColor(uint8_t hue, uint8_t sat, uint8_t bright = 255){
  CRGB ledColor;
  if(paletteActive){ ledColor = ColorFromPalette(targetPal, hue, bright, LINEARBLEND); }
  else{ledColor = CHSV(hue,sat,bright);}
  return ledColor;
}

CRGB getTargetPalColor(CHSV input){
  CRGB ledColor = input;;
  if(paletteActive){ ledColor = ColorFromPalette(targetPal, input.h, input.v, LINEARBLEND); }
  return ledColor;
}

bool fadeNow = false;

void paletteFade(){
  
  if(!paletteFadeAuto){return;}
  if(paletteNextAuto){
    EVERY_N_SECONDS( SECONDS_PER_PALETTE ) {  nextPalette(); }
  }
  
  if(fadeNow){
      EVERY_N_MILLISECONDS(20) { nblendPaletteTowardPalette( currentPal, targetPal, 24);  }//32
      EVERY_N_MILLISECONDS(200) { 
        if(paletteEqualsPalette(currentPal, targetPal)){
          iCurrentPal = iTargetPal; 
          fadeNow = false;
          if(DEBUG){ Serial.println("fade pal off"); }
          }
        }
  }
}

void nextPalette(){ newPaletteIdx(iCurrentPal + 1);}

void lastPalette(){ newPaletteIdx(iCurrentPal - 1);}

void newPaletteIdx(int8_t idxPalette){
  iTargetPal = wrap_int(idxPalette, numPalettes);
  targetPal  = gradientPalettes[iTargetPal];
  if(paletteFadeAuto) { 
    fadeNow = true; 
    if(DEBUG){ 
      Serial.println("fade palette on"); 
      Serial.print("current palette: "); Serial.print(paletteNames[iCurrentPal]);
      Serial.print("\t - \t new target palette: "); Serial.println(paletteNames[iTargetPal]);
      }
    }
}

bool paletteEqualsPalette( CRGBPalette16& current, CRGBPalette16& target){
    uint8_t* p1 = (uint8_t*)current.entries;
    uint8_t* p2 = (uint8_t*)target.entries;
    const uint8_t totalChannels = sizeof(CRGBPalette16);
    for( uint8_t i = 0; i < totalChannels; ++i) { if( p1[i] != p2[i] ) { return false; } }
    return true;
}

void nextAni(){ 
if(DEBUG){Serial.print("now:\t# "); Serial.print(currentAni);Serial.print(" / "); Serial.print(numAnis); Serial.print('\t'); Serial.println(aniList[currentAni].aniName);}
currentAni = (currentAni + 1) % numAnis;
if(DEBUG){Serial.print("next:\t# "); Serial.print(currentAni);Serial.print(" / "); Serial.print(numAnis); Serial.print('\t'); Serial.println(aniList[currentAni].aniName);}

}
void lastAni(){
  if(currentAni - 1 < 0){ currentAni = numAnis-1; }
  else{currentAni--;}
 }

void newAniIdx(int8_t idxAni){ currentAni = wrap_int(idxAni, numAnis); }
