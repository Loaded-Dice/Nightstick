//--------------------------------------------------------------------------------------------------------------------- Common
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
#include <avr/dtostrf.h>
#define DEBUG 1
#define SIZE(ARRAY)    (sizeof(ARRAY) / sizeof(ARRAY[0]))
#define ON  true
#define OFF false
#define ERRLONG  1000 // for internal led blink timing
#define ERRSHORT 300  // for internal led blink timing
#define RAD(A)  (A * 71) / 4068.0  // convert degree to radians
#define DEG(A)  (A*4068) / 71.0    // convert radians to degree
#define FREQ2MS(A) 1000/A
#define MS2FREQ(A) 1000/A
#define MAINPATH "/Nightstick"  // Main Folder on SD where BMPs Folder is in
#define SUBFLD_BMP "/BMPs"
#define SUBFLD_TRAIL "/trails"
#define SUBFLD_STATIC "/static"
#define CONFIGNAME "Config.csv"
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
#define NUM_LEDS   254 // 110 Leds @ center stick

#define LED_OFF       0

#define LED_STATIC    1
#define LED_TRAIL     2
#define LED_ANI       3
#define LED_FIRE      4
#define LED_BRIGHT    5
#define LED_BATT      6

#define LED_BLE       7
#define LED_TEST      8   // For testing new stuff
uint8_t ledMode = LED_TEST;
uint8_t ledModeLast = LED_TEST;
CRGB leds[NUM_LEDS];
CRGBPalette16 currentPalette = OceanColors_p;//PartyColors_p
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);//254

//---==={VARIABLES - LEDs }===---//
uint8_t gHue = 0;
//uint8_t BRIGHTNESS = 40;// 40; // 40 = 155mA ; 3,8h at 600mA
uint8_t gCurrentPatternNumber = 0;

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
#include <MahonyAHRS.h>
#include <Arduino_LSM6DS3.h>
#include "helper_3dmath.h"
#define AXIS_X {1,0,0}
#define AXIS_Y {0,1,0}
#define AXIS_Z {0,0,1}
Mahony filter;

//---==={VARIABLES - Filter_IMU}===---//
VectorFloat accl;
VectorFloat gyro;
VectorFloat axisStick = AXIS_X; // change depending on µC orientation inside the Nightstick
float tiltAngle;
float rotAngle;
float rotAngleLast;
float rotAngle2;
float roll, pitch, yaw;
float heading;
float ax, ay, az;
float gx, gy, gz;
Quaternion qt;
VectorFloat gravity;

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
int16_t buffHeight; // when bmp is loaded dimensions are kept here
int16_t buffWidth; //
uint16_t ledPixelPos[NUM_LEDS][2]; // x and y position 


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

//static const uint8_t cfgInfoCount = 9;

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
