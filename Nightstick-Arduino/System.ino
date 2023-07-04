void setup_System(){
  setupInernalLed();
}

void setupInernalLed(){
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  digitalWrite(LED_RED, HIGH); // High = off
  digitalWrite(LED_GREEN, HIGH);// High = off
  digitalWrite(LED_BLUE, HIGH);// High = off
}

void setBoardLed(char ch, char state){
   bool state_bool;
  if(state == '1'){state_bool = HIGH;}
  else if(state == '0'){state_bool = LOW;}
  else{msgln("wrong channel state");return;}
  setBoardLed(ch, state_bool);
  // msg(ch); msg(" Led channel is now "); msgln(state ? "HIGH" : "LOW");
}

void setBoardLed(char ch, bool state){
  int chPin;
  if(ch=='R'){chPin = LED_RED;}
  else if(ch=='G'){chPin = LED_GREEN;}
  else if(ch=='B'){chPin = LED_BLUE;}
  else{msgln("wrong channel char");return;}
  digitalWrite(chPin, !state);
}

//struct errInfo{  char msg[MAXFILECHARS]; bool logSD; char ch1; uint16_t t1; char ch2; uint16_t t2; bool LedLoop;};
void error(uint8_t errID){
  lastErrID = errID;
  if (errID == 0) {return;}
  errInfo err;
  getErrInfo(errID, &err);
  
  if(DEBUG && Serial){Serial.println(err.msg);}
  //if(err.logSD){append2log(err.msg);}
  setErrBlink(&err);
}

void setErrBlink(errInfo* err){ //char ch1, uint16_t delay1, char ch2, uint16_t delay2, bool loopForever
    while(true){
      setBoardLed(err->ch1,true);  delay(err->t1);  setBoardLed(err->ch1,false); delay(330) ;
      setBoardLed(err->ch2,true);  delay(err->t2);  setBoardLed(err->ch2,false); delay(2500);
      setBoardLed(err->ch1,true);  delay(err->t1);  setBoardLed(err->ch1,false); delay(330) ;
      setBoardLed(err->ch2,true);  delay(err->t2);  setBoardLed(err->ch2,false); delay(2500);
      if(!err->ledLoop){break;}
  }
}

void getErrInfo(uint8_t index, errInfo* thisErr){ if(index < errInfoCount){memcpy_P(&thisErr, &errInfoArr[index], sizeof(errInfo));} }

void msg(const char *msgIn){
  if (msgIn == NULL) return;
  Serial.print(msgIn);
}
void msgln(const char *msgIn){
  if (msgIn == NULL) return;
  msg(msgIn); msg("\r\n");
}

// ------------------------------------------ CHAR ARRAY -> TO -> VAROUS TYPES ---------------------------------------------------

bool isBmp(char* filename){return (strcmp(&filename[strlen(filename)-4],".bmp") == 0 || strcmp(&filename[strlen(filename)-4],".BMP") == 0);}
bool isCfg(char* filename){return (strcmp(&filename[strlen(filename)-4],".csv") == 0 || strcmp(&filename[strlen(filename)-4],".CSV") == 0);}
bool isCsv(char* filename){return isCfg(filename);}

bool isByte(char* str) { //uint8_t result result = atoi(str);
    if(strlen(str) > 3) return false;
    for(uint8_t i = 0; i < strlen(str); i++){ if(!isDigit(*str+i) || str[i] == '.'){return false;} }
    if (atoi(str) < 256) {convByte = atoi(str); return true;} 
  return false;
}

bool isInt(char* str){ //
    if(strlen(str) > 11) return false;
      for(uint8_t i = 0; i < strlen(str); i++){  
        if(i == 0 && str[i] == '-'){i++;}
        if(!isDigit(str[i]) || str[i] == '.'){ return false;} 
        }
  convInt = atoi(str);
  return true;
}

bool isFloat(char* str){ //float result = atof(str);
  uint8_t nDot = 0;
      for(uint8_t i = 0; i < strlen(str); i++){
        if(i == 0 && str[i] == '-'){i++;}
        if(!(isDigit(str[i]) || str[i] == '.')){ return false;} //|| str[0] == '-') i
        if(str[i] == '.'){nDot++;}
      }
    if (nDot > 1) { return false; } 
    convFloat = atof(str);
    return true;
}

bool isBool(char* str){
  if(strlen(str)>5)return false;
  char str2[5];
  strcpy(str2,str);
  cArrToLower(str2);
       if ((strcmp(str2, "true")  == 0) || (strcmp(str2, "an")  == 0) || (strcmp(str2, "1") == 0)){return true;}
  else if ((strcmp(str2, "false") == 0) || (strcmp(str2, "aus") == 0) || (strcmp(str2, "0") == 0)){return true;}
  else {return false;}
}

bool isWord(char* str){
  for(uint8_t i = 0; i < strlen(str); i++){  if( (strchr( " .-_!+()", str[i]) == NULL) && !(isAlphaNumeric(str[i]))){ return false; } }
  return true;
}

void cArrToLower(char* str) { cArrChangeCase(str,false);}
void cArrToUpper(char* str) { cArrChangeCase(str,true); }
bool cArrStartsWith(char* str, char chk){return (str[0] == chk);}
bool cArrEndsWith(char* str, char chk){return (str[strlen(str)-1] == chk);}
int16_t cArrIndexOf(char* str, char chk){ for(int i = 0; i < strlen(str); i++){if(str[i]==chk){return i;}} return -1;}

void cArrTrim(char* str){
 while( cArrEndsWith(str, ' ') || cArrEndsWith(str, '\t') || cArrStartsWith(str, ' ') || cArrStartsWith(str, '\t')){
  if( cArrEndsWith(str, ' ') || cArrEndsWith(str, '\t')){cArrTrimRight(str);}
  if( cArrStartsWith(str, ' ') || cArrStartsWith(str, '\t')){cArrTrimLeft(str);}
 }
}
void cArrTrimLeft(char* str){  uint16_t cArrLen = strlen(str); strcpy(str, &str[1]); str[cArrLen-1] = '\0'; } 
void cArrTrimLeft(char* str, uint16_t len){  uint16_t cArrLen = strlen(str); strcpy(str, &str[len]); str[cArrLen-len] = '\0'; }

void cArrTrimRight(char* str){ str[strlen(str)-1] = '\0';}
void cArrTrimRight(char* str, uint16_t len){ str[strlen(str)-len] = '\0';}

 void cArrChangeCase(char* str, bool toUpper){
  for(int i = 0; i < strlen(str); i++){ // x |= (1 << n);
    if((( str[i]  >= 'A' ) && ( str[i]  <= 'Z' )) || (( str[i]  >= 'a' ) && ( str[i]  <= 'z' ))){
      if(toUpper){str[i] &= ~(1 << 5);}
      else{str[i] |= (1 << 5);} // when upper case letter make it lower case -> to upper case str[i] &= ~(1 << 5);
    }
  }
}
// ------------------------------------------ VAROUS TYPES -> TO -> CHAR ARRAY ---------------------------------------------------
const char * f2char(float fval){ charBuff[0] = '\0'; dtostrf(fval, 4, 2,charBuff); return charBuff;}
const char * i2char(int ival){charBuff[0] = '\0'; itoa(ival,charBuff,10);  return charBuff;}
const char * ul2char(unsigned long ulvar){charBuff[0] = '\0'; ultoa(ulvar,charBuff, 10);  return charBuff;}//(ulong,charBuffer,radix)(radix 2 = BIN | 10 = DEC | 16 = HEX)
char *dtostrf (double val, signed char width, unsigned char prec, char *sout) {
  char fmt[20];
  sprintf(fmt, "%%%d.%df", width, prec);
  sprintf(sout, fmt, val);
  return sout;
}
char * ms2Time(unsigned long ms){ //any ms long in --> return char array Xd XXh XXm  (max width = 11)
  ms /= 1000;
  charBuff[0] = '\0';
  sprintf(charBuff,"%ud %02uh %02um",(unsigned int)(ms/86400),(unsigned int)((ms / 3600) % 24),(unsigned int)((ms/60) % 60));
  return charBuff;
}
