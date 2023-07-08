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
//#define DELAYMS_FILTERIMU 10 // 100Hz or 10 ms fixed
#define DELAYMS_SHOWLED   25 // 100Hz
#define DELAYMS_FILTER    25 // 100Hz
#define DELAYMS_READBTN   30 //
#define DELAYMS_READBATT  300  //1000
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
#define MAXTIME  900
#define A_SHORT   1
#define B_SHORT   2
#define C_SHORT   3
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
#define DEBUG false
#define SIZE(ARRAY)    (sizeof(ARRAY) / sizeof(ARRAY[0]))
#define ON  true
#define OFF false
#define NEXT  1 // changing folder and bmps
#define LAST -1 // changing folder and bmps
#define ERRLONG  1000 // for internal led blink timing
#define ERRSHORT 300  // for internal led blink timing
#define RAD(A)  (A * 71) / 4068.0  // convert degree to radians
#define DEG(A)  (A*4068) / 71.0    // convert radians to degree
#define RADTO16(A) (A * 10430.378350) // A / (2 * M_PI) * 65535
#define DEGTO16(A) (A * 182.044444) // A / 360 * 65535
#define U16TORAD(A) (A / 10430.378350) //
#define U16TOGED(A) (A / 182.044444) // A / 360 * 65535
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
const char *  modeNames[] = {"Off","Static","Trail","Animation","Fire","Battery"};
// Dfinitions for ledMode ( determine only the current playing led animation)
#define LED_OFF       0
#define LED_STATIC    1
#define LED_TRAIL     2
#define LED_ANI       3
#define LED_FIRE      4
#define LED_BATT      5
#define LED_BLE       6
#define LED_TEST      7   // For testing new stuff

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
char fileBuff[MAXFILECHARS];
char folderBuff[MAXFILECHARS];
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
uint8_t initBatt = 0;
const uint16_t vLookUp[]   = {268,  278, 288, 298, 308, 318, 328, 339, 349, 359, 369, 379, 389, 399, 409, 419}; //( vBat*100 )
const uint16_t rawLookUp[] = {1168,1205,1239,1285,1315,1357,1398,1443,1488,1533,1566,1610,1650,1690,1733,1775}; //(Analog voltage @ 12 Bit; vRef = 3.0V)
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
  uint8_t bright = 25;
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


CRGB leds[NUM_LEDS];

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);//254

#include "Palettes.h"
//---==={VARIABLES - LEDs }===---//-----------------------------------------------------------------------------------------

//uint8_t ledMode = LED_STATIC;// LED_TEST; //LED_TRAIL // LED_BRIGHT // LED_STATIC
uint8_t ledModeLast = LED_STATIC;// LED_TEST;

typedef void (*functPtr)();
struct animations{ functPtr functCall; char * aniName; bool active;};
//int8_t currentAni = 0;
uint8_t paletteTimeS = 20;
uint8_t aniTimeS = 60;

struct pos_f{  float x; float y;};
uint8_t gHue = 0;
//uint8_t BRIGHTNESS = 40;// 40; // 40 = 155mA ; 3,8h at 600mA
uint8_t gCurrentPatternNumber = 0;

uint16_t ledPixelPos[NUM_LEDS][2]; // x and y position 
float ledVector[2] = {0.0,0.0};
float colTrail = 0.0;   // store the current trails colum
float trailSpeed = 0.5; // 182 (yaw16) units = 1° per bmp colum 
float blendMulti = 100.0;
 
bool paletteActive = true; // if active getColor(h,s,v) returns color from palette as CRGB, otherwise return CHSV(h,s,v)
bool paletteNextAuto = false; // automatic load  iCurrentPal+1 to targetPal after paletteTimeS
bool paletteFadeAuto = true; // when off 2 palettes can be loaded at the same time without fade
bool aniNextAuto = false;
bool fadeNow = false;
//uint8_t currentAni = 0; //cfg.ledAni
//uint8_t iCurrentPal = 0; // cfg.palette
uint8_t iTargetPal = 1;
const uint8_t numPalettes = SIZE(gradientPalettes); //sizeof( gradientPalettes) / sizeof( TProgmemRGBGradientPalettePtr );
CRGBPalette16 currentPal = gradientPalettes[cfg.palette];
CRGBPalette16 targetPal  = gradientPalettes[iTargetPal];


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
// Own online generated UUID for the Nightstick

//BLEService nightstickService("68158000-6355-4571-8642-BBD34048E5A5"); // online generated SERVICE UUID for the Nightstick
////BLEService nightstickService(BLEUuid(0x1801));
//BLECharacteristic nightstickAuth("68158001-6355-4571-8642-BBD34048E5A5");
//BLECharacteristic nightstickAuth(BLEUuid(0x1008));
////UUID16_CHR_ILLUMINANCE
//UUID16_CHR_FIXED_STRING_16
//UUID16_CHR_FIXED_STRING_24
//UUID16_CHR_FIXED_STRING_36
//UUID16_CHR_FIXED_STRING_8
//UUID16_CHR_UNSPECIFIED
//UUID16_CHR_OTS_OBJECT_NAME
//UUID16_CHR_OTS_OBJECT_TYPE
uint8_t bleMode = BLE_OFF;
uint8_t bakLedMode = LED_OFF; // backup led mode to return after BLE animation

//---==={VARIABLES - BLE_COM}===---//
char comBufIn[BLE_BUFSIZE];
char comBufOut[BLE_BUFSIZE];
bool newBleData =false;
bool newSerialData =false;

//--------------------------------------------------------------------------------------------------------------------- Filter_IMU
//---==={DEFINITIONS - Filter_IMU}===---//
#include "LSM6DS3.h"
#include "Wire.h"
LSM6DS3 myIMU(I2C_MODE, 0x6A);    //I2C device address 0x6A
#include "SensorFusion.h" //SF
SF fusion;
float deltat;
float imuTemp = 0.0;
unsigned long tmeas;
int deltaMeas;
uint8_t imuMode = OFF; // ON OFF
//---==={VARIABLES - Filter_IMU}===---//
float roll, pitch, yaw, yawDelta;
uint16_t roll16,pitch16,yaw16,yaw16Last;
float ax, ay, az;
float gx, gy, gz;
float accl[3];
float gyro[3];

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
uint16_t audioThresh = 1000;
uint8_t eqDimMax = 64;
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
  // for calculating the min / max peak in the last 3000ms

};
struct eqAmpRange{
  int ampMin = 5000;
  unsigned long ampMin_t = 0;
  int ampMax = 0;
  unsigned long ampMax_t= 0;
  const uint16_t decay = 3000; // slowly return ampMax to 0 and rise ampMin to ampMax after 3000 ms 
};
eqAmpRange eqRange;

eqBand EQ[8] = {// ampl fract,peak,lastpeak,lastval,lastmeasured, ampMin ,ampMin timer, ampMax, ampMax timer
  {   110, 0, 0, 0, 0},  //0    -> 128 Hz     i <=2
  {   200, 0, 0, 0, 0},  //129  -> 256 Hz     i >=3 && i<=4
  {   200, 0, 0, 0, 0},  //266  -> 512 Hz     i >=5 && i<=8
  {   200, 0, 0, 0, 0},  //513  -> 1024 Hz    i >=9 && i<=16
  {   200, 0, 0, 0, 0},  //1025 -> 2048 Hz    i >=17 && i<= 32
  {   200, 0, 0, 0, 0},  //2048 -> 4096 Hz    i >=33 && i<= 64
  {   170, 0, 0, 0, 0},  //4096 -> 7040 Hz    i >=65  && i<=110
  {   50,  0, 0, 0, 0}   //7040 -> 8192 Hz    i >=111 (&&<=128)
};

// measured fft correction values to get a more linear output
const float freqCalib[113] = {0.0,  0.0, 1.3079793,  1.069392, 0.784344, 0.9103336,  1.0166674,  0.862052, 0.7109092,  0.6427657,  0.7753275,  0.6113147,  0.4977038,  0.6528794,  0.4102594,  0.3913894,  0.7237714,  0.5113062,  0.491581, 0.6531831,  0.8559368,  1.4665207,  1.9665577,  2.1739053,  2.4351664,  2.1376235,  1.8633525,  1.8255845,  2.180384, 1.5464601,  1.6231678,  2.450974, 2.2835728,  2.2933477,  2.4481844,  2.6828074,  2.9802801,  3.1908219,  3.1528289,  3.3493042,  3.9719504,  5.3594922,  6.0969157,  7.5512116,  6.8002132,  6.2995524,  7.049264, 8.5025804,  8.6423623,  8.5025804,  6.2421385,  5.8940065,  5.8308792,  7.1547174,  5.6450937,  4.7016548,  2.9953562,  2.289182, 1.8821454,  2.0298621,  1.9277527,  1.6978568,  2.00691,  1.6753267,  1.6483045,  1.9788069,  2.20481,  2.2698795,  2.2922805,  2.339455, 2.2609692,  2.3103107,  2.9767121,  3.5016091,  3.7774394,  3.5720545,  3.2492898,  3.0435062,  3.0108962,  3.2872719,  3.3061042,  2.6788108,  2.5514675,  2.6003714,  2.9922521,  3.7972216,  5.1530411,  4.964938, 5.2497917,  5.4095423,  6.6076133,  8.1143134,  9.2289323,  9.2289323,  8.9301363,  8.1143134,  7.7601315,  6.8622778,  6.7801759,  7.435715, 8.5025804,  10.2971722, 13.7245555, 16.2286268, 17.8602727, 21.4158875, 22.3687344, 20.6004929, 21.4158875, 19.8597521, 22.3687344, 20.6004929, 18.4663333};
