/*
other LED animations 
no setup_X() or main_X()
*/
// hue 0 =red, 32 = orange 64 = yellow, 96 = green
//fill_gradient (T *targetArray, uint16_t startpos, CHSV startcolor, uint16_t endpos, CHSV endcolor, TGradientDirectionCode directionCode=SHORTEST_HUES)
//pitch


void bleAni(){

  cfg.ledMode = bakLedMode; // return to backup LedMode when animation is done
  bakLedMode = LED_OFF;
}

int8_t battAnz = 0;
void ledBatt(){
  static bool toggle = true;
  static uint8_t rise = 0;
  EVERY_N_MILLIS(150){toggle = !toggle;} //pBat 0 to 100 fill_solid(leds, NUM_LEDS, CRGB::Black); ledsShow();
  EVERY_N_MILLIS(25){
      if(abs(battAnz - pBat) > 6 && rise < pBat){rise++;}
      else if(abs(battAnz - pBat) > 6 && rise > pBat){rise--;}
      else if(abs(battAnz - pBat) > 6 && rise == pBat){battAnz = pBat;}
    }

 // fill_solid(leds, NUM_LEDS, CRGB::Black); 
  fill_solid(leds, NUM_LEDS, CHSV(160,255,255));
  if((pBat < 10 && toggle) || (pBat >= 10)){
  fill_gradient (leds,76,CHSV(0,255,255),76+rise,CHSV(pBat,255,255),SHORTEST_HUES);
  for (int i = 0; i < 5; i++){leds[72+i]=CHSV(160,255,255); leds[176+i]=CHSV(160,255,255);}
  }

}


void ani_main(){
  if(aniList[cfg.ledAni].active){ aniList[cfg.ledAni].functCall(); }
  else if(!aniList[cfg.ledAni].active && aniNextAuto){nextAni();}
  if(aniNextAuto){ EVERY_N_SECONDS(aniTimeS){ nextAni();} }
}


//create virtual area to spread the roll angle offset
const uint8_t vWidth = 13; // virtual led strip length
const uint8_t vHeight = 138; // area for roll offset exoansion  x=6   +/- 6px

void plasma(){
    static int minNoise = 0;
    static int maxNoise = 0;
    float plasmaSpeed = 25;//0.25; //0.1 to 1
    int plasmaZoom = 2800; // 20 to 80
    ledVector[1] = 1;
    ledVector[0] = 0;
    for( int i = 0; i < (NUM_VLEDS); i++ ){ virt2realLed(i, (vHeight-1)/2, i ); }
    

    uint32_t ms = plasmaSpeed * millis(); 
    for( int i = 0; i < (NUM_LEDS); i++ ){
      int x = ledPixelPos[i][0];
      int y = ledPixelPos[i][1];
      int noiseVal = inoise16_raw(y * plasmaZoom, x * plasmaZoom, ms);
      if(noiseVal > maxNoise){maxNoise = noiseVal; }
      else if(noiseVal < minNoise){minNoise = noiseVal; }
      byte noises =  map(noiseVal,minNoise,maxNoise,8,246);
      //uint8_t bright = getEqBright(noises); // if(fftMode) recive a dimmed brightness for each color (mapped to eq band)... else if(!fftMode) recive 255 brightness
      leds[i] = getCurrentPalColor(noises, 255, 255);
    }
}
//input color  and  get the a brightness for this color scaled by eq peak values
uint8_t getEqBright(uint8_t color){ // hue-range per band 32
  
if(fftMode && millis() > eqStart+800){ // wait for the first fft output blast to fade 
  uint8_t colorBand = color / 32;
  uint8_t posBand = color % 32;
  int8_t rangeMin = posBand < 16 ? colorBand-1 : colorBand ;
  int8_t rangeMax = posBand < 16 ? colorBand : colorBand+1 ;
  if (posBand >= 16){posBand -=16;}
  if(rangeMin == -1 ){ rangeMin = 7;} // wrap eq peak around
  else if(rangeMax == 8){rangeMax = 0;} // wrap eq peak around
  // smooth transitions between two eq band peaks
  uint16_t interpolated = map(posBand,0,15, EQ[rangeMin].peak,EQ[rangeMax].peak); 
  // map the interpolated EQ peak  between the min /map peak within the last 3 sec
  uint8_t new_bright = scale8_video( 255, 255 - map(interpolated,eqRange.ampMin,eqRange.ampMax,0, eqDimMax));
  return new_bright;
  }
  else{ return 255;}
}

pos_f wavePosB = {6.0,68.5};
pos_f wavePosA = {6.0,68.5};
pos_f waveStep = {0.0,0.0};

pos_f getRandomPoint(){  return  {random8( 0, vWidth), random8( 0, vHeight)};}

pos_f getStep(pos_f posCurrent, pos_f posTarget){
  pos_f deltaPos = {(posTarget.x - posCurrent.x) , (posTarget.y - posCurrent.y) };
  float dist = sqrt(sq(deltaPos.x*6.0)+sq(deltaPos.y*6.0))/ 6.0 ; // normalized distance  / 138.0
  return { deltaPos.x / dist ,  deltaPos.y / dist };
}

void waveRings(){
  ledVector[1] = 1;
  ledVector[0] = 0;
    for( int i = 0; i < (NUM_VLEDS); i++ ){ virt2realLed( i, (vHeight-1)/2 ,i ); }
    //virt2realLed(uint16_t vIdx, float vx, float vy){
  //bool waveBlend = false; // move from wavePosA to B or blend
    static uint16_t hueOffA  = 0; // counter for radial color wave motion
    static float zoom = 6; //0.7; // 0.5 is close and 10 is zoomed out
    static uint8_t hueShift = 3;
    byte brightA = 255;
    //------------------------------------
    static float ringDistA = 0.0;
    static float ringWidthA = 15.0;
    //------------------------------------
    
  for( int i = 0; i < (NUM_LEDS); i++ ){
    int x = ledPixelPos[i][0];
    int y = ledPixelPos[i][1];
    
    float waveCenterDistA = sqrt(sq(((float)x - wavePosA.x)*zoom) + sq(((float)y - wavePosA.y )* zoom)) ;

//    if(waveCenterDistA <= ringDistA  && waveCenterDistA >= (ringDistA - ringWidthA) ){
//       brightA = 255-cos8((waveCenterDistA - (ringDistA - ringWidthA))) ; 
//    }
//    else{brightA = 0;}
    
    leds[i] = getCurrentPalColor(sin8(waveCenterDistA + hueOffA), 255, brightA);
    //getTargetPalColor(color, 255, 255);
  }
  EVERY_N_MILLIS(40){
    hueOffA -= hueShift;
    ringDistA += hueShift;
    if((int)wavePosA.x == (int)wavePosB.x || (int)wavePosA.y == (int)wavePosB.y ){ // || wavePosA.x < 0 ||  wavePosA.x > vWidth || wavePosA.y < 0 || wavePosA.y > vHeight ) { 
      wavePosB = getRandomPoint();
      waveStep = getStep(wavePosA,wavePosB);
    }
    else{
      wavePosA.x += waveStep.x;
      wavePosA.y += waveStep.y;
      }
    }
}

void rainbow() { fill_rainbow(leds, NUM_LEDS, gHue, 7); }// FastLED's built-in rainbow generator.

void vAreaSetLedXY(){
  ledVector[1] = 1;
  ledVector[0] = 0;
  // Set the virtual LED XY pos  in a staight line x=6  y = i
  // the function will automatically adjust to the roll angle
  // For these functions X & Y p
  //                                                 vIdx,xPos,yPos
  for( int i = 0; i < (NUM_VLEDS); i++ ){ virt2realLed(i, (vHeight-1)/2, i ); }

//  for (byte y = 0; y < vHeight; y++) {
//    for (byte x = 0; x < vWidth; x++) {
//      int idx = getLedIdxByPos(x,y); // try to find the right LED
//      if(idx != -1){leds[idx] = CRGB::Black;}
//    }

}

int getLedIdxByPos(uint16_t x,uint16_t y){
  for( uint16_t i = 0; i < NUM_LEDS; i++ ){  
    if(ledPixelPos[i][0] == x &&  ledPixelPos[i][1] == y){ 
      return i;
    } 
  }  
  return -1;
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    //CHSV newColor = ColorFromPalette(currentPal,dothue, 200, 255);
    
    leds[safe( beatsin16(i+7,0,NUM_LEDS))] |= getCurrentPalColor(dothue, 255,  255);
    //leds[beatsin16(i+7,0,NUM_LEDS)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }

}
void bpm(){
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 140;
  //CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] =getCurrentPalColor(gHue+(i*2), 255,  beat-gHue+(i*10)); // ColorFromPalette(currentPal, gHue+(i*2), beat-gHue+(i*10));
    
  }
}

void fadeall() { for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(250); } }

void cylon(){ // --------------------------------------------------------------------------
  static int counter = 0;
  static byte hue = 0;
  
  EVERY_N_MILLISECONDS( 10 ) {
    counter++;
    //counter = counter % NUM_LEDS;
    if(counter < NUM_LEDS){leds[safe(counter)] = getCurrentPalColor(hue++, 255,  255);  ledsShow();  fadeall();}
    if(counter >= NUM_LEDS && counter < (2*NUM_LEDS)){leds[safe(NUM_LEDS-(counter-NUM_LEDS))] = getCurrentPalColor(hue++, 255,  255); ledsShow();  fadeall();}
     if(counter >= (2*NUM_LEDS-2)){counter=0;}
    }
}

//---------------------------------------------------------Fire Animation (Similar to balcony lamps)

uint8_t angleShift = 0; // 0 to 11
const uint16_t rows = 16 ; // scrap the top rows to avoid mostly dark leds
const uint16_t cols = 13 ; // one extra row to mash togeter with row 0

/* Flare constants */
const uint8_t flarerows = 6;    /* number of rows (from bottom) allowed to flare */
const uint8_t maxflare = 6;     /* max number of simultaneous flares */
const uint8_t flarechance = 50; /* chance (%) of a new flare (if there's room) */
const uint8_t flaredecay = 14;  /* decay rate of flare radiation; 14 is good */
const uint8_t FPS = 80;
/* This is the map of colors from coolest (black) to hottest. Want blue flames? Go for it! */
const uint32_t colors[] = {
  0x000000, 0x100000, 0x300000, 0x600000, 0x800000, 0xA00000, 0xC02000, 0xC04000, 0xC06000, 0xC08000, 0xBC9F65
  //0x807080
};
const uint8_t NCOLORS = SIZE(colors);

uint8_t pix[rows][cols]; // pixel heat/color buffer

uint8_t nflare = 0;
uint32_t flare[maxflare];

unsigned long t = 0; /* keep time */

void make_fire() {
  uint16_t i, j;
  if ( t > millis() ) return;
  t = millis() + (1000 / FPS);

  // First, move all existing heat points up the display and fade
  for ( i=rows-1; i>0; --i ) {
    for ( j=0; j<cols; ++j ) {
      uint8_t n = 0;
      if ( pix[i-1][j] > 0 )
        n = pix[i-1][j] - 1;
      pix[i][j] = n;
    }
  }

  // Heat the bottom row
  for ( j=0; j<cols; ++j ) {
    i = pix[0][j];
    if ( i > 0 ) {
      pix[0][j] = random(NCOLORS-6, NCOLORS-2);
    }
  }

  // flare
  for ( i=0; i<nflare; ++i ) {
    int x = flare[i] & 0xff;
    int y = (flare[i] >> 8) & 0xff;
    int z = (flare[i] >> 16) & 0xff;
    glow( x, y, z );
    if ( z > 1 ) {
      flare[i] = (flare[i] & 0xffff) | ((z-1)<<16);
    } else {
      // This flare is out
      for ( int j=i+1; j<nflare; ++j ) {
        flare[j-1] = flare[j];
      }
      --nflare;
    }
  }
  newflare();

for ( i=0; i<rows; ++i ) { pix[i][0] = (pix[i][0] / 2 ) + (pix[i][12] / 2 ); //combine row 0 and 12  
  // Set and draw
  for ( i=0; i<rows; ++i ) {
    for ( j=0; j<cols; ++j ) {
      int16_t ledPos = fireMapA(i,j);
          if(ledPos != -1){
              leds[ledPos] = colors[pix[i][(j + angleShift) % 12]];
              leds[fireMapB(i,j)] = colors[pix[i][(j + angleShift + 6) % 12]];
          }
      }
      
    }
  }
  //FastLED.show();
}

// colors[pix[rows][cols]]; --> cols+angleShift

int16_t fireMapA(uint8_t row, uint8_t col){
  uint16_t ledPos = -1;
  if(row == 0){ledPos = 71 - col;}
  else if(row == 13){ledPos = col;}
  if(row > 0 && row < 13){
         if(col == 1){ ledPos = 47 + row;}
    else if(col == 4){ ledPos = 35 + row;}
    else if(col == 7){ ledPos = 23 + row;}
    else if(col == 10){ledPos = 11 + row;}

  }
  return ledPos;
}

int16_t fireMapB(uint8_t row, uint8_t col){
  uint16_t ledPos = -1;
  if(row == 0){ledPos = 182 + col;}
  else if(row == 13){ledPos = 253 - col;}
  if(row > 0 && row < 13){
         if(col == 1){ ledPos = 192 + row;}
    else if(col == 4){ ledPos = 204 + row;}
    else if(col == 7){ ledPos = 216 + row;}
    else if(col == 10){ledPos = 228 + row;}
  }
  return ledPos;
}
uint32_t isqrt(uint32_t n) {
  if ( n < 2 ) return n;
  uint32_t smallCandidate = isqrt(n >> 2) << 1;
  uint32_t largeCandidate = smallCandidate + 1;
  return (largeCandidate*largeCandidate > n) ? smallCandidate : largeCandidate;

}

// Set pixels to intensity around flare
void glow( int x, int y, int z ) {
  int b = z * 10 / flaredecay + 1;
  for ( int i=(y-b); i<(y+b); ++i ) {
    for ( int j=(x-b); j<(x+b); ++j ) {
      if ( i >=0 && j >= 0 && i < rows && j < cols ) {
        int d = ( flaredecay * isqrt((x-j)*(x-j) + (y-i)*(y-i)) + 5 ) / 10;
        uint8_t n = 0;
        if ( z > d ) n = z - d;
        if ( n > pix[i][j] ) { // can only get brighter
          pix[i][j] = n;
        }
      }
    }
  }
}

void newflare() {
  if ( nflare < maxflare && random(1,101) <= flarechance ) {
    int x = random(0, cols);
    int y = random(0, flarerows);
    int z = NCOLORS - 1;
    flare[nflare++] = (z<<16) | (y<<8) | (x&0xff);
    glow( x, y, z );
  }
}



/*
//------------------------------------------------------------
// RGB Orbit---------------------------------------------------------------------------------------------------
bool printColor =false;
void RGBorbit() {
  
  ledVector[1] = 1;
  ledVector[0] = 0;
  for( int i = 0; i < (NUM_VLEDS); i++ ){ virt2realLed(i, (vHeight-1)/2, i ); }
  
  static byte hueOffset  = 0; // counter for radial color wave motion
  static int plasVector = 0; // counter for orbiting plasma center
  static int distFactorCount = 0; // counter for orbiting plasma center
  float zoom = 8; //0.7; // 0.5 is close and 1 is zoomed out
  float xOffMatrix  = -6;// moves the whole animation
  float yOffMatrix  = -68.5
  ;//moves the whole animation 

   // Calculate current center of plasma pattern (can be offscreen)
  float distFactor = 0; //(float)sin8((float)distFactorCount / 128)/255;  // center distance factor should be between 0(center) and 1 (outside)
  float xOffset = (cos8(plasVector / 128)-127)*distFactor + xOffMatrix; //orbiting plasma center x val
  float yOffset = (sin8(plasVector / 128)-127)*distFactor + yOffMatrix; // orbiting plasma center y val


  for( int i = 0; i < (NUM_LEDS); i++ ){
    int x = ledPixelPos[i][0];
    int y = ledPixelPos[i][1];
        // determine the color by the distance (simple theorem of Pythagoras)
        byte color = sin8(sqrt(sq(((float)x + xOffset)*zoom) + sq(((float)y + yOffset)* zoom)) +hueOffset) ;
        //leds[i] = CHSV(color, 255, 255); //= ColorFromPalette( gCurrentPatternNumber, color, 255 ,LINEARBLEND);  //RGB without palettes  = CHSV(color, 255, 255);
        leds[i] = getCurrentPalColor(color, 255, 255);
  }
    hueOffset-= 1; // hue offset wraps at 255 for sin8 , + for direction , change int for hue scroll speed
    //plasVector += 64; // using an int for slower orbit (wraps at 65536)
    float relCenterSpeed =1 ;// 128 - (distFactor * 128);
    //plasVector += (relCenterSpeed); //
    //distFactorCount += 5;
}



void rotationRainbow()  // ---- palettemix
{   
  float zoom = 3; // 5 to 10
    ledVector[1] = 1;
    ledVector[0] = 0;
    for( int i = 0; i < (NUM_VLEDS); i++ ){ virt2realLed(i, (vHeight-1)/2, i ); }
    
    uint32_t ms = millis();
    //int32_t yHueDelta32 = ((int32_t)cos16( ms * (27/1) ) * (350 / vWidth));
    //int32_t xHueDelta32 = ((int32_t)sin16( ms * (39/1) ) * (310 / vHeight));
    int32_t yHueDelta32 = ((int32_t)cos16( (ms * (17.0/1.0)) + vWidth/2) * (zoom*35 / vWidth));
    int32_t xHueDelta32 = ((int32_t)sin16( (ms * (29.0/1.0)) + vHeight/2) * (zoom*31 / vHeight));
    DrawOneFrame( ms / 65536, yHueDelta32 / 32768 + 3*zoom , xHueDelta32 / 32768 );
}

void DrawOneFrame( byte startHue8, int8_t yHueDelta8, int8_t xHueDelta8)
{
    for( int i = 0; i < (NUM_LEDS); i++ ){
    int x = ledPixelPos[i][0];
    int y = ledPixelPos[i][1];
 // byte lineStartHue = startHue8;
 // for( byte y = 0; y < vHeight; y++) {
    //lineStartHue += yHueDelta8;
 //   lineStartHue = 
   // byte pixelHue = (yHueDelta8 * (1+y));     
  //  for( byte x = 0; x < vWidth; x++) {
      
       //leds[out][0][ XYsafe(x, y)]  = CHSV( pixelHue, 255, 255);       
       //leds[out][0][ XYsafe(x, y)]  = ColorFromPalette( gCurrentPalette, pixelHue, 255 ,LINEARBLEND);  
       byte pixelHue = (startHue8 + (yHueDelta8 * (y+1))) + (xHueDelta8 * (x+1));
       leds[i] = getCurrentPalColor(pixelHue, 255, 255);
    }
 //   }
 // }
 // copyLEDs();  
}

*/


/*

void colorWaves() {
  
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;
 
  uint8_t sat8 = beatsin88( 87, 220, 250);
  uint8_t brightdepth = beatsin88( 341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16;//gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 300, 1500);
  
  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis ;
  sLastMillis  = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88( 400, 5,9);
  uint16_t brightnesstheta16 = sPseudotime;
  
  for( uint16_t i = 0 ; i < NUM_VLEDS; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;
    uint16_t h16_128 = hue16 >> 7;
    if( h16_128 & 0x100) {
      hue8 = 255 - (h16_128 >> 1);
    } else {
      hue8 = h16_128 >> 1;
    }

    brightnesstheta16  += brightnessthetainc16;
    uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);
    
    uint8_t index = hue8;
    //index = triwave8( index);
    index = scale8( index, 240);

    CRGB newcolor = ColorFromPalette( currentPal, index, bri8);

    uint16_t pixelnumber = i;
    pixelnumber = (NUM_VLEDS-1) - pixelnumber;
    int16_t realIdx = getRealFirstIdx(pixelnumber);
    if(realIdx >= 0){nblend( leds[realIdx], newcolor, 128);}
  }
  spreadRealFirstIdx();
  mirrorStick(0);
}
*/
