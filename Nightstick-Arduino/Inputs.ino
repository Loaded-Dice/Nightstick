void setup_Inputs(){  pinMode(BTN_PIN,INPUT); }

uint8_t btnValCount = 0;
uint8_t btnRng = 40;
uint16_t btnP = BTN_NONE;
uint16_t btnPLast = BTN_NONE;

unsigned long btnTimer;
unsigned long btnDelay;
bool btnRst = false;

void main_Inputs(){
  EVERY_N_MILLIS(DELAYMS_READBTN){readBtn();}
}

uint16_t btnVal = 0; 
uint16_t btnValLast = 0; 

void readBtn(){
  if(millis() > btnDelay){
    //float btnVal_f = ((float)analogRead(BTN_PIN)/maxBat)*1000.0;
    //float btnVal_f = ((float)analogRead(BTN_PIN));
    
    btnVal = analogRead(BTN_PIN);
    
    if(btnVal > btnValLast + 20 || btnVal < btnValLast - 20){
        delay(2);
        btnVal = analogRead(BTN_PIN);
        //Serial.print("Btn val raw: "); 
        //Serial.println(btnVal);
        charBuff[0]='\0'; // clear buffer
        strcpy(charBuff,i2char(rawBat));
        strcat(charBuff,"  ");
        strcat(charBuff,i2char(btnVal));
        if(bleMode == BLE_CONN){sendBLE(charBuff);}
        btnValLast = btnVal;
    }

           if(btnVal > BTN_A  - btnRng && btnVal < BTN_A  + btnRng){btnVal = BTN_A;}
      else if(btnVal > BTN_B  - btnRng && btnVal < BTN_B  + btnRng){btnVal = BTN_B;}
      else if(btnVal > BTN_C  - btnRng && btnVal < BTN_C  + btnRng){btnVal = BTN_C;}
      else if(btnVal > BTN_AB - btnRng && btnVal < BTN_AB + btnRng){btnVal = BTN_AB;}
      else if(btnVal > BTN_BC - btnRng && btnVal < BTN_BC + btnRng){btnVal = BTN_BC;}
      else {btnVal = BTN_NONE;}
      
      if((btnVal == BTN_NONE && btnValLast != BTN_NONE)||(btnVal != BTN_AB && btnValLast == BTN_AB)||(btnVal != BTN_BC && btnValLast == BTN_BC)) {
        btnP = btnValLast; 
        btnValLast = BTN_NONE; 
        btnVal = BTN_NONE;
        btnHandler();
        btnDelay = millis() + 500;
        }
      if(btnVal != btnValLast) { btnTimer = millis();  btnValLast = btnVal;}
  }
}


void btnHandler(){
  if (millis() - btnTimer < LONGTIME){
    
  }
}
