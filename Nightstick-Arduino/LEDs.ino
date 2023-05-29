
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
      case LED_STATIC: Led2Pixel_static(rotAngle); drawRamPixel(true);  break;
      case LED_TRAIL: Led2Pixel_trails(rotAngle);   break; // only for testing Led2Pixel_static is for static bmps!
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
//rollStaticTest(); 

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

//void chkBmpLoaded(){
//  
//  
//  if(strcmp(cfg.staticBmp,"") == 0 || strcmp(cfg.staticBmp,"-") == 0 || strcmp(cfg.staticFolder,"") == 0 || strcmp(cfg.staticFolder,"-") == 0){//search for first bmp - open nextFile until path != dir 
//
//    } 
//  if(strcmp(cfg.staticFolder,"") == 0 || strcmp(cfg.staticFolder,"-") == 0){;} 
//  strcmp
//}
//
//
//
//void loadFirstValid(){
//  bool errBmp = false;
//  File32 path;
//  charBuff[0] = '\0';
//  strcpy(charBuff,MAIN_PATH);
//  strcat(charBuff,"/BMPs");
//  if(!sd.exists(charBuff)){errBmp = true;} // throw error!
//  else{
//  path.open(path, FILE_READ);
//  
//  cfg.staticFolder[0] = '\0';
//  cfg.staticBmp[0] = '\0';
//  
//  while(true){
//    File32 entry =  path.openNextFile();
//    if (! entry) {errBmp = true; break; }
//    charBuff[0] = '\0';
//    if (entry.isDir()) {
//      entry.getName(charBuff,sizeof(charBuff)); 
//      
//      strcat(cfg.staticFolder,"/");
//      strcat(cfg.staticFolder,charBuff);
//      }
//  
//  }
//  }
//  // handle error 
//}


void rainbow() { fill_rainbow(leds, NUM_LEDS, gHue, 7); }// FastLED's built-in rainbow generator.
  



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
