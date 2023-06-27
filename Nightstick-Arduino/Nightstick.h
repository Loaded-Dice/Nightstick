//--------------------------------------------------------------------------------------------------------------------- Common

//System Status indicator variables
//uint8_t bleMode = BLE_OFF; // BLE_OFF , BLE_ADV , BLE_CONN
//uint8_t ledMode = LED_OFF;// LED_TEST; //LED_TRAIL...
//uint8_t imuMode = OFF; // ON OFF
//uint8_t fftMode = OFF; // ON OFF
//---==={DEFINITIONS - COMMON / PINS}===---//
#define LED_PIN     D1
#define SDCS_PIN    D2
#define BTN_PIN     A3
//---==={DEFINITIONS - COMMON / DELAYS}===---//
#define DELAYMS_FILTERIMU 10 // 100Hz
#define DELAYMS_SHOWLED   10 // 100Hz
#define DELAYMS_READBTN   15 //
#define DELAYMS_READBATT  500  //1000
#define DELAYMS_LEDOFF    2000  //
//--=={DEFINITIONS - COMMON / Inputs}==--//


#define BTN_A_VAL       2504
#define BTN_B_VAL       1775
#define BTN_C_VAL       787
#define BTN_AB_VAL      2839
#define BTN_BC_VAL      2034
#define BTN_RANGE   50    //value +/- Range
#define BTN_MIN     500  // if value > BTN_MIN --> btn-pressed else relese
#define LONGTIME    450 // time in ms
#define ULONGTIME  2200
#define A_SHORT   1
#define B_SHORT   2
#define C_SHORT   2
#define A_LONG    4
#define B_LONG    5
#define C_LONG    6
#define B_ULONG   7
#define BTN_AB    8
#define BTN_BA    9
#define BTN_CB    10
#define BTN_BC    11

// ---==={VARIABLES - Inputs}===---//
struct Btn{
    uint16_t aVal = 0;
    char firstBtn = ' ';
    uint16_t aValPressed = 0;
    unsigned long timer;
    unsigned long wait = 0;
    bool prepare =false;
    bool blocked = false;
};
Btn btn;

//---==={DEFINITIONS - COMMON / System}===---//
//#include <avr/dtostrf.h>
#define DEBUG 1
#define SIZE(ARRAY)    (sizeof(ARRAY) / sizeof(ARRAY[0]))
#define ON  true
#define OFF false
#define NEXT  1 // changing folder and bmps
#define LAST -1 // changing folder and bmps
#define ERRLONG  1000 // for internal led blink timing
#define ERRSHORT 300  // for internal led blink timing
#define RAD(A)  (A * 71) / 4068.0  // convert degree to radians
#define DEG(A)  (A*4068) / 71.0    // convert radians to degree
#define RADTO16(A) (A * 10430.378350)
#define DEGTO16(A) (A * 182.044444) // A / 360 * 65535
#define OFF false
#define On true     

#define FREQ2MS(A) 1000/A
#define MS2FREQ(A) 1000/A
#define MAIN_PATH "/Nightstick"  // Main Folder on SD where BMPs Folder is in
#define SUBFLD_BMP "/BMPs"
#define SUBFLD_TRAIL "/trails"
#define SUBFLD_STATIC "/static"
#define CONFIGNAME "Config.csv"
#define FULLPATH_TRAILS (MAIN_PATH SUBFLD_BMP SUBFLD_TRAIL)
#define FULLPATH_STATIC (MAIN_PATH SUBFLD_BMP SUBFLD_STATIC)
//FULLPATH_TRAILS FULLPATH_STATIC
// MAIN_PATH SUBFLD_BMP
#define TYPE_FLD 0
#define TYPE_BMP 1
#define TYPE_CFG 2
#define MAXFILECHARS 51 // 50 + null
//---==={VARIABLES - COMMON / SYSTEM}===---//
uint8_t lastErrID = 0; // 
struct errInfo{  char msg[MAXFILECHARS]; bool logSD; char ch1; uint16_t t1; char ch2; uint16_t t2; bool ledLoop;};

static const errInfo errInfoArr[] PROGMEM = { // delay times defined ERRSHORT or ERRLONG
// |-----------------Error Message-------------------| log? |led1,delay1   ,led2 ,delay2   ,loop?      error ID (Array index)
  {"No Error"                                         ,false,'G' ,0        ,'G'  ,0        ,false},  //  0
  {"No SD Card detected"                              ,false,'R' ,ERRSHORT ,'R'  ,ERRSHORT ,true }   //  1
};

static const uint8_t errInfoCount = sizeof(errInfoArr)/sizeof(errInfo);


char charBuff[MAXFILECHARS*4];  // generic char buffer (BLE has own buffer)
char pathBuff[MAXFILECHARS*4];
//--------------------------------------------------------------------------------------------------------------------- Batt
//--=={DEFINITIONS - Batt}==--//
#define AVG_BATTREAD 4 // average batteryvoltage after n readings
//---==={VARIABLES - Batt}===---//
// vRefSet is 3.0V  bc.  the btn voltage div. analog in won't clip when using btns
float vRefSet = 3.0; //Internal selectable: 3.6 || 3.0 || 2.4 || 1.8 || 1.2
float vBat = 4.2;   // battery volatge (corrected)
uint8_t pBat = 100;   // battery percentage (3.65V to 4.2V);
float rawBat = 0.0;
float maxBat = 4.2;   // save scaled raw analog Batt val (for btn read volt.div.)
const uint16_t vLookUp[]   = {268,  278, 288, 298, 308, 318, 328, 339, 349, 359, 369, 379, 389, 399, 409, 419}; //( vBat*100 )
const uint16_t rawLookUp[] = {1168,1205,1239,1285,1315,1357,1398,1443,1488,1533,1566,1610,1650,1690,1733,1775}; //(Analog voltage @ 12 Bit; vRef = 3.0V)

//--------------------------------------------------------------------------------------------------------------------- LEDs
//---==={DEFINITIONS - LEDs}===---//
#include <FastLED.h> 
FASTLED_USING_NAMESPACE
#include <colorpalettes.h>
#include <Adafruit_NeoPixel.h>
#define FRAMES_PER_SECOND  100 //
#define max_bright 50
#define min_bright 10
#define NUM_LEDS   254 // real number of LEDs
#define NUM_VLEDS  138 // virtual number of LEDs. used for a correct roll and yaw 2D projection
#define SECONDS_PER_PALETTE 10
#define SECONDS_PER_ANIMATION 2

// Dfinitions for ledMode ( determine only the current playing led animation)
#define LED_OFF       0
#define LED_STATIC    1
#define LED_TRAIL     2
#define LED_ANI       3
#define LED_FIRE      4
#define LED_BRIGHT    5
#define LED_BATT      6
#define LED_BLE       7
#define LED_TEST      8   // For testing new stuff
uint8_t ledMode = LED_ANI;// LED_TEST; //LED_TRAIL
uint8_t ledModeLast = LED_ANI;// LED_TEST;

typedef void (*functPtr)();
int8_t currentAni = 0;

struct animations{ functPtr functCall; char * aniName; bool active;};

struct pos_f{  float x; float y;};

CRGB leds[NUM_LEDS];
//CRGBPalette16 currentPalette = OceanColors_p;//PartyColors_p
CRGBPalette16 currentPalette = {
  0xFF0000, 0x7F0000, 0xAB5500, 0x552A00, 0xABAB00, 0x555500, 0x00FF00, 0x007F00,
  0x00AB55, 0x00552A, 0x0000FF, 0x00007F, 0x5500AB, 0x2A0055, 0xAB0055, 0x55002A
};
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);//254

//---==={VARIABLES - LEDs }===---//
uint8_t gHue = 0;
//uint8_t BRIGHTNESS = 40;// 40; // 40 = 155mA ; 3,8h at 600mA
uint8_t gCurrentPatternNumber = 0;

uint16_t ledPixelPos[NUM_LEDS][2]; // x and y position 
float ledVector[2] = {0.0,0.0};
float colTrail = 0.0;   // store the current trails colum
float trailSpeed = 0.5; // 182 (yaw16) units = 1° per bmp colum 
float blendMulti = 100.0;
 
bool paletteActive = true; // if active getColor(h,s,v) returns color from palette as CRGB, otherwise return CHSV(h,s,v)
bool paletteNextAuto = false; // automatic load  iCurrentPal+1 to targetPal after SECONDS_PER_PALETTE
bool paletteFadeAuto = false; // when off 2 palettes can be loaded at the same time without fade

bool aniNextAuto = true;
//shall get overwritten by calibration:
//uint16_t ringOff16[4] & uint16_t stripOff16[2][4]
// would be 12 calibration points
// offset scaled to uint_16 for first ring LEDs (leds to light up in calib: #0,#60,#182,#242)

uint16_t ringOff16[4] = {38229,    36408,   40049,   18204}; //first leds roll offset for each led-ring. other offsets are calculated with angle16Ring[]
int angle16Ring[12] = { 0,   3641,  7282,  16384, 20025, 23665, 32768, 36408, 40049, 49151, 52792, 56433}; // uint16_t angle offset in led ring relative to first LED within the ring
float rollDistMaxPx = 6.0;// max pixel distaance for roll angle 2D projection
//                                cpu side                          battery side
uint16_t stripOff16[2][4] =  {{25486,  9102,  58253, 41870},{30947,  47331, 63715, 14563}};//mini strip led roll offset scaled to uint_16 for mini strips cpu side (leds to light up in calib: #12 ->#23, #24 ->#35, #36 ->#47, #48 ->#59 )
const uint16_t stripLedPos[2][4] = {{24,     36,    48,    60, } ,{69,     81,    93,    105 }}; // mini strip LED starting position
//--------------------------------------------------------------------------------------------------------------------- BLE_COM
//---==={DEFINITIONS - BLE_COM}===---//
#include <bluefruit.h>
#define BLE_FAST_TIMEOUT  90
#define BLE_TIMEOUT       120
#define TERM_CHAR         '\n'
#define BLE_BUFSIZE       150
BLEDis  bledis;  // device information
BLEUart bleuart; // uart over ble
BLEBas  blebas;  // battery service
#define BLE_OFF           0   // BLE OFF
#define BLE_ADV           1   // BLE Service Advertising
#define BLE_CONN          2   // Peer connected
uint8_t bleMode = BLE_OFF;

//---==={VARIABLES - BLE_COM}===---//
char comBufIn[BLE_BUFSIZE];
char comBufOut[BLE_BUFSIZE];
bool newBleData =false;
bool newSerialData =false;

//--------------------------------------------------------------------------------------------------------------------- Filter_IMU
//---==={DEFINITIONS - Filter_IMU}===---//
#include <Arduino_LSM6DS3.h>
//#include <MahonyAHRS.h>
//Mahony filter;
#include<MadgwickAHRS.h>
Madgwick filter;

//MahonyAHRS MadgwickAHRS filter;

uint8_t imuMode = OFF; // ON OFF
//---==={VARIABLES - Filter_IMU}===---//
float roll, pitch, yaw, yawLast;
uint16_t roll16,pitch16,yaw16,yaw16Last;
float ax, ay, az;
float gx, gy, gz;

//--------------------------------------------------------------------------------------------------------------------- SD
//---==={DEFINITIONS - SD}===---//
#include "SdFat.h"
#define SPI_CLOCK SPI_FULL_SPEED
#if HAS_SDIO_CLASS
#define SD_CONFIG SdioConfig(FIFO_SDIO)
#elif  ENABLE_DEDICATED_SPI
#define SD_CONFIG SdSpiConfig(SDCS_PIN, DEDICATED_SPI, SPI_CLOCK)
#else  
#define SD_CONFIG SdSpiConfig(SDCS_PIN, SHARED_SPI, SPI_CLOCK)
#endif  
//#define error(s) sd.errorHalt(&Serial, F(s))
#define FILECHARMAX 41 // 50 +  '\0' (null) 
//---==={VARIABLES - SD }===---//
SdFat32 sd;
File32 dir;
File32 file;

#define MAXPIXEL 40000 //--> 200x200px
#define BUFFPIXEL 512 // number of pixel to read in a row from SD
uint8_t pixelBuff[MAXPIXEL][3]; // --> 120kB

struct bmpInfo{
    File32 file;
    uint32_t fileSize;
    uint32_t headerSize;
    uint16_t width; // BMP image width in pixel
    int16_t height; // BMP image height in pixel
    uint8_t depth; // Bit depth (currently must report 24)
    uint32_t imageOffset; // Start of image data in file
    uint32_t rowSize; // Not always = bmpWidth; may have padding
    uint8_t sdbuffer[3 * BUFFPIXEL];
    uint32_t buffidx = sizeof(sdbuffer); // Current position in sdbuffer
    uint8_t povidx = 0;
    boolean goodBmp = false;
    boolean flip = true;
    int w, h;
    uint32_t pos = 0;
    int r, g, b;
};
bmpInfo bmp;
//--------------------------------------------------------------------------------------------------------------------- CONFIG
//---==={DEFINITIONS - CONFIG}===---//

#define TYPE_NONE   0
#define TYPE_BYTE   1
#define TYPE_INT    2
#define TYPE_FLOAT  3
#define TYPE_BOOL   4
#define TYPE_WORD   5

#define CFG_START      0
#define CFG_BLENAME    1   
#define CFG_BRIGHT     2
#define CFG_STATICBMP  3
#define CFG_STATICFLD  4
#define CFG_TRAILSBMP  5
#define CFG_TRAILSFLD  6
#define CFG_ANI        7
#define CFG_PALETTE    8
#define CFG_LEDMODE    9
#define CFG_ENDE       10

#define CFG_SEPERATOR ";" // must be in double quotation marks because tokenize function strtok expects char array (could be multiple seperator chars)

//---==={VARIABLES - CONFIG}===---//
char lastFld[50]; // char buffer to store last visited folder while checking bmp folders
char lastBmp[50]; // char buffer to store last visited bitmap while checking bmp folders

struct cfgFile{ // initialize with std values  and overwrite atrr from SD id found
  char bleName[30] ="Nightstick";
  uint8_t bright = 35;
  char staticBmp[MAXFILECHARS] = "-";
  char staticFolder[MAXFILECHARS*2] ="-"; // store 2 Folders= trails/elements or static/full
  char trailsBmp[MAXFILECHARS] = "-";
  char trailsFolder[MAXFILECHARS*2] ="-"; // store 2 Folders= trails/elements or static/full
  uint8_t ledAni = 0;
  uint8_t palette = 0;
  uint8_t ledMode = LED_OFF;
};
cfgFile cfg;   

struct cfgInfos{  char cfgVarName[MAXFILECHARS]; uint8_t varType;};
cfgInfos cfgEntry; // holds the property name & file type of entry calles by getCfgInfo(index)  - max is stored here uint8_t cfgInfoCount

static const cfgInfos cfgEntryArray[] PROGMEM = { // read config file properties like name as text and value type from progmem --> for parsing values
  //                                           index
  {"Nightstick Config Start",   TYPE_NONE}, // 0
  {"Bluetooth Name",            TYPE_WORD}, // 1  
  {"Led Helligkeit",            TYPE_BYTE}, // 2  Led Brightness
  {"static Bitmap",             TYPE_WORD}, // 3  Last File loaded
  {"static Ordner",             TYPE_WORD}, // 4  Last active Folder
  {"trails Bitmap",             TYPE_WORD}, // 5  Last File loaded
  {"trails Ordner",             TYPE_WORD}, // 6  Last active Folder
  {"Animation",                 TYPE_BYTE}, // 7  Last played animation
  {"Farbpalette",               TYPE_BYTE}, // 8  Last used color palette
  {"Led Modus",                 TYPE_BYTE}, // 9  Last LED mode (fire/animaton/bmp...)
  {"Nightstick Config Ende",    TYPE_NONE}  // 10
};
static const uint8_t cfgInfoCount = sizeof(cfgEntryArray)/sizeof(cfgEntry);

// conversion buffer (auto filled by isVarType(char*) function)
uint8_t convByte;
int convInt;
float convFloat;
bool convBool;

//--------------------------------------------------------------------------------------------------------------------- LEDs
//---==={DEFINITIONS - FFT}===---//
#include <PDM.h>
#include <arduinoFFT.h>
//#include <FastLED_timers.h>
arduinoFFT FFT = arduinoFFT();

#define SAMPLES 256  // Must be a power of 2
#define SAMPLING_FREQUENCY 16000

//---==={VARIABLES - FFT}===---//


//Audio variables (ForFFT & PDM)
volatile static bool samples_ready = false;
uint8_t fftStep = 0; // split FFT into 3 steps to render led frames inbetween
uint8_t audioGain = 30; // gain value of the PDM microphone (default 20)
bool fftMode = false; //  start_Audio() --> true; stop_Audio() -->false
unsigned long eqStart; // FFT will start with a blast. to avoid this give it one second to sample enough data. this var will measure the time

short sampleBuffer[SAMPLES];// buffer to read samples into (from PDM), each sample is 16-bits
double vReal[SAMPLES]; //input/output data buffer for fft
double vImag[SAMPLES]; // data buffer for fft
volatile int samplesRead; // number of samples read

struct eqBand {
  uint16_t amplitude;
  int peak;
  int lastpeak;
  uint16_t lastval;
  unsigned long lastmeasured;
};

eqBand EQ[8] = {// ampl fract,peak,lastpeak,lastval,lastmeasured 
  {   110, 0, 0, 0, 0},  //0    -> 128 Hz     i <=2
  {   200, 0, 0, 0, 0},  //129  -> 256 Hz     i >=3 && i<=4
  {   200, 0, 0, 0, 0},  //266  -> 512 Hz     i >=5 && i<=8
  {   200, 0, 0, 0, 0},  //513  -> 1024 Hz    i >=9 && i<=16
  {   200, 0, 0, 0, 0},  //1025 -> 2048 Hz    i >=17 && i<= 32
  {   200, 0, 0, 0, 0},  //2048 -> 4096 Hz    i >=33 && i<= 64
  {   170, 0, 0, 0, 0},  //4096 -> 7040 Hz    i >=65  && i<=110
  {   50,  0, 0, 0, 0}   //7040 -> 8192 Hz    i >=111 (&&<=128)
};

//-----------------------------------------------------------------------------

//char * AniNames[] = {"plasma","waveRings","rainbow","cylon","bpm","juggle"};
//const uint8_t numAnis = 6;
//int8_t currentAni = 0;
