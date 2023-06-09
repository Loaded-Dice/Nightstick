
void setup_LEDs(){
  strip.begin();
  strip.setBrightness(cfg.bright);
  strip.show(); // Initialize all pixels to 'off'
}
void main_LEDs(){
 
if(ledMode == LED_OFF && ledModeLast == LED_OFF){return;}

 if(ledMode == LED_OFF && ledModeLast != ledMode){ledsClear();ledsClear();ledsClear();}

   EVERY_N_MILLIS(DELAYMS_SHOWLED) {
    gHue++;
    //if(ledMode != ledModeLast && ledMode == LED_STATIC){chkBmpLoaded();};
    switch (ledMode) {
      case LED_STATIC: Led2Pixel_static(); drawRamPixel(true);  break;
      case LED_TRAIL: Led2Pixel_trails();   break; // only for testing Led2Pixel_static is for static bmps!
      case LED_ANI:  break;
      case LED_FIRE: make_fire();  break;
      case LED_BRIGHT:  break;
      case LED_BATT:  break;
      case LED_BLE:  break;
      case LED_TEST:  break;
      default:  break; 
    }
  ledsShow();
  }
  ledModeLast = ledMode; 
}


void fastToNeo(){for(int i = 0; i < NUM_LEDS; i++){ strip.setPixelColor(i, leds[i].r, leds[i].g, leds[i].b); }}

void ledsShow(){ fastToNeo(); strip.show();}

void ledsClear(){fill_solid(leds, NUM_LEDS, CRGB::Black); ledsShow();} 

void mirrorStick(){
  for(int i = 0; i < 127; i++){
      int16_t sourcePos = 126-i;
      if(sourcePos >= 12 && sourcePos <= 48){ leds[sourcePos+182]=leds[sourcePos]; }
      else{leds[127+i] = leds[126-i]; }
  }
}
/*
   how to load bmp to POV display:
   clear charBuff[] ( charBuff[0] = '\0'; )
   create full filepath to bmp file and store it in charBuff[]
   call BMPtoRAM(charBuff); it will analyze the header file auomaticly
   file is now saved to ram
   every frame update call: 
  ---------------------------------
  Led2Pixel_static(); to obtain newest xy position from the leds on the image
  and then drawRamPixel(); to put the rgb data corrsponding to the XY data into the led array
  finally call ledsShow() to update the stick

*/


// calculate the x & y positions of the led projected on the bmp file
//

void Led2Pixel_trails() { // angle in rad !!!
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

CRGB getPixel(uint16_t colX, uint16_t rowY){ return CRGB(pixelBuff[arrayPos(colX,rowY)][0],pixelBuff[arrayPos(colX,rowY)][1],pixelBuff[arrayPos(colX,rowY)][2]);} // recive the bmp pixel color

uint16_t arrayPos(uint16_t colX, uint16_t rowY){return (colX*bmp.w+rowY);} //convert the 2D X/Y information to the flat pixelBuff layout

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
  
/*
INFO
//create 1 pixel vector from center pixel (P1) to next pixel (P2) --> Vector = P2-P1
//ad rotate vector by 90° cw --> Vector_X * -1, then swap Vector_X & Vector_Y
// then scale the vector to max pixel offset distance 90° from stick (max dist 15mm => 5 pixel) (constant: maxRollPx)

        // vector of one LED distance pointing from the center to cpu side
        ledVector[0] 
        ledVector[1]
        // vecor of one LED distance but perpendicular (clockwise) to the stick
        ledVecor 90° cw[0]  =  ledVector[1] // for roll angle distance of CPU side LEDs
        ledVecor 90° cw[1]  = -ledVector[0] // for roll angle distance of CPU side LEDs
        // vecor of one LED distance but perpendicular (counter clockwise) to the stick
        ledVecor 90° ccw[0] = -ledVector[1] // for roll angle distance of battery side LEDs
        ledVecor 90° ccw[1] =  ledVector[0] // for roll angle distance of battery side LEDs
        // multiply by rollDistMaxPx and then scale down by specific sin() of the difference between the roll angle and each LEDs offset angle
        angle16Ring[0 to 11] relative led offset
        getOffScaling();
        retrive scaling factor for ring leds:
                          wrap to uint16_t , starting offset 1st led, -1 or 1  relative offset to 1st ring led
        float a = getOffScaling( wrap_u16(ringOff16[1 to 4] + ring_Data_direction * angle16Ring[0 to 11])
*/
