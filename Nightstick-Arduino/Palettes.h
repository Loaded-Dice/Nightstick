DEFINE_GRADIENT_PALETTE( rainbow1 ) {
      0,  255,  0,  0, // Red
     32,  171, 85,  0, // Orange
     64,  171,171,  0, // Yellow
     96,    0,255,  0, // Green
    128,    0,171, 85, // Aqua
    160,    0,  0,255, // Blue
    192,   85,  0,171, // Purple
    224,  171,  0, 85, // Pink
    255,  255,  0,  0};// and back to Red

DEFINE_GRADIENT_PALETTE( fancy1 ) {
    0,   1,  9, 84,
   40,  42, 24, 72,
   84,   6, 58,  2,
  168,  88,169, 24,
  211,  42, 24, 72,
  255,   1,  9, 84};

//-----------------------------------smooth
//bordeaux gold blue dark - smooth
DEFINE_GRADIENT_PALETTE( smooth1 ) {
    0,  23,  4, 32,
   63,  98, 31, 52,
  127, 224,138, 24,
  191,   7, 55,164,
  255,  23,  4, 32};

//orange lila smooth
DEFINE_GRADIENT_PALETTE( smooth2 ) {
    0, 227,101,  3,
  117, 194, 18, 19,
  255,  92,  8,192};

// sunrise-------------------------------------

DEFINE_GRADIENT_PALETTE( sunrise1 ) {
    0, 255,255, 45,
   43, 208, 93,  1,
  137, 224,  1,242,
  181, 159,  1, 29,
  255,  63,  4, 68};

DEFINE_GRADIENT_PALETTE( sunrise2 ) {
    0, 120,  0,  0,
   22, 179, 22,  0,
   51, 255,104,  0,
   85, 167, 22, 18,
  135, 100,  0,103,
  198,  16,  0,130,
  255,   0,  0,160};

//--------------other & misc

DEFINE_GRADIENT_PALETTE( other1 ) {
    0, 234,231,  1,
   15, 171, 43,  6,
   40, 121,  0,  0,
   53,  95,  1, 29,
   71,  73,  1,168,
   94,  38, 63,221,
  109, 115, 51,221,
  127,  38, 63,221,
  147,  73,  1,168,
  181, 203, 28,  1,
  193, 155, 16, 11,
  216,  73,  1,168,
  255,   1,  4, 29};

//fire&ice
DEFINE_GRADIENT_PALETTE( other2 ) {
    0,  80,  2,  1,
   51, 206, 15,  1,
  101, 242, 34,  1,
  153,  16, 67,128,
  204,   2, 21, 69,
  255,   1,  2,  4};

DEFINE_GRADIENT_PALETTE( other3 ) {
    0, 215, 74,  6,
   30,  74, 22, 53,
   61,  18,  6, 27,
   96,   5, 36, 85,
  114,   1,  8, 30,
  140,   1,  8, 30,
  158,   5, 36, 85,
  193,  18,  6, 27,
  224,  74, 22, 53,
  255, 215, 74,  6};

DEFINE_GRADIENT_PALETTE( other4 ) {
    0, 255,156,  0,
  127,   0,195, 18,
  216,   1,  0, 39,
  255,   1,  0, 39};

DEFINE_GRADIENT_PALETTE( other5 ) {
    0,   3, 82,  4,
   38,   9, 13, 16,
   89, 184, 77,105,
  127, 201,213,156,
  165, 222, 88,170,
  216, 190, 63,  4,
  255,   3, 82,  4};

//firelike-----------------------------------------
DEFINE_GRADIENT_PALETTE( firelike1 ) {
    0,   0,  0,  0,
   95, 255,  0,  0,
  191, 255,255,  0,
  255, 255,255,255};

DEFINE_GRADIENT_PALETTE( firelike2 ) {
  0,     0,  0,  0,   //black
128,   255,  0,  0,   //red
224,   255,255,  0,   //bright yellow
255,   255,255,255 }; //full white

DEFINE_GRADIENT_PALETTE( ocean1 ) {
    0, 130,197,255,
   89,   5, 73,180,
  165,   4, 23, 54,
  249,   0,  1,  8,
  255,   0,  1,  8};

DEFINE_GRADIENT_PALETTE( ocean2 ) {
    0, 157,221,247,
   38,   0, 56,111,
   63,   1, 15, 49,
   66, 184,231,250,
  153,  22,119,184,
  255,   2, 45, 89};

  
char * paletteNames[] = {
  "rainbow1", 
  "fancy1",  
  "smooth1",  
  "smooth2",  
  "sunrise1",  
  "sunrise2",  
  "other1",  
  "other2", 
  "other3", 
  "other4", 
  "other5", 
  "firelike1",  
  "firelike2",  
  "ocean1",  
  "ocean2",  
};
  
const TProgmemRGBGradientPalettePtr gradientPalettes[] = {
  rainbow1,
  fancy1,
  smooth1,
  smooth2,
  sunrise1,
  sunrise2,
  other1,
  other2,
  other3,
  other4,
  other5,
  firelike1,
  firelike2, 
  ocean1, 
  ocean2, 
}; 
