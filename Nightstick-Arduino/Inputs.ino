void setup_Inputs(){  pinMode(BTN_PIN,INPUT); }

uint8_t btnValCount = 0;
uint8_t btnRng = 40;
uint16_t btnP = BTN_NONE;
uint16_t btnPLast = BTN_NONE;
uint16_t btnValLast ;
unsigned long btnTimer;
unsigned long btnDelay;
bool btnRst = false;


void readBtn(){
  if(millis() > btnDelay){
    //float btnVal_f = ((float)analogRead(BTN_PIN)/maxBat)*1000.0;
    float btnVal_f = ((float)analogRead(BTN_PIN));
    uint16_t btnVal = btnVal_f;

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

}
