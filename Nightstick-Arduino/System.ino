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
const char *  getErrMsg(uint8_t numErr){
    switch (numErr) {
      case 0: return "No Error"; break;
      case 1: return "sendBLE bleBuf overflow"; break;
      case 2: return "No SD Card"; break;
      case 3: return ""; break;
      case 4: return ""; break;
      case 5: return ""; break;
      default: return "Unknown Error"; break;
    }
}

void msg(const char *msgIn){
  if (msgIn == NULL) return;
  Serial.print(msgIn);
}
void msgln(const char *msgIn){
  if (msgIn == NULL) return;
  msg(msgIn); msg("\r\n");
}

// ------------------------------------------ CHAR ARRAY -> TO -> VAROUS TYPES---------------------------------------------------

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
void cArrTrim(char* str){
 while( cArrEndsWith(str, ' ') || cArrEndsWith(str, '\t') || cArrStartsWith(str, ' ') || cArrStartsWith(str, '\t')){
  if( cArrEndsWith(str, ' ') || cArrEndsWith(str, '\t')){cArrTrimRight(str);}
  if( cArrStartsWith(str, ' ') || cArrStartsWith(str, '\t')){cArrTrimLeft(str);}
 }
}
void cArrTrimLeft(char* str){  uint16_t cArrLen = strlen(str); strcpy(str, &str[1]); str[cArrLen-1] = '\0'; } 

void cArrTrimRight(char* str){ str[strlen(str)-1] = '\0';}

 void cArrChangeCase(char* str, bool toUpper){
  for(int i = 0; i < strlen(str); i++){ // x |= (1 << n);
    if((( str[i]  >= 'A' ) && ( str[i]  <= 'Z' )) || (( str[i]  >= 'a' ) && ( str[i]  <= 'z' ))){
      if(toUpper){str[i] &= ~(1 << 5);}
      else{str[i] |= (1 << 5);} // when upper case letter make it lower case -> to upper case str[i] &= ~(1 << 5);
    }
  }
}
// ------------------------------------------ VAROUS TYPES -> TO -> CHAR ARRAY ---------------------------------------------------
char attrBuf[21];
//const char * f2char(float fval){ attrBuf[0] = '\0'; dtostrf(fval, 4, 2,attrBuf); return attrBuf;}
const char * i2char(int ival){attrBuf[0] = '\0'; itoa(ival,attrBuf,10);  return attrBuf;}
const char * ul2char(unsigned long ulvar){attrBuf[0] = '\0'; ultoa(ulvar,attrBuf, 10);  return attrBuf;}//(ulong,charBuffer,radix)(radix 2 = BIN | 10 = DEC | 16 = HEX)

char * ms2Time(unsigned long ms){ //any ms long in --> return char array Xd XXh XXm  (max width = 11)
  ms /= 1000;
  attrBuf[0] = '\0';
  sprintf(attrBuf,"%ud %02uh %02um",(unsigned int)(ms/86400),(unsigned int)((ms / 3600) % 24),(unsigned int)((ms/60) % 60));
  return attrBuf;
}
