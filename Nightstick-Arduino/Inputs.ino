void setup_Inputs(){ pinMode(BTN_PIN,INPUT); }

void main_Inputs(){ EVERY_N_MILLIS(DELAYMS_READBTN){readBtn();}}


void readBtn(){
    if(millis() < btn.wait){return;}
    
    btn.aVal = (int)(((float)analogRead(BTN_PIN)/ ((float)analogRead(PIN_VBAT) / 510.0 * 1510.0)) * 4095);
    
    if(btn.blocked && btn.aVal > BTN_MIN){ return; } // wait until 2 btn combination returns to low
    if(btn.blocked && btn.aVal < BTN_MIN){ clearBtn(260); return; }
    
    if(btn.aVal > BTN_MIN && !btn.prepare){btn.prepare = true; return;} // drop first measurement to debounce
    else if(btn.aVal < BTN_MIN && btn.firstBtn == ' '){return;}
    else if(btn.aVal < BTN_MIN && btn.prepare && btn.firstBtn == ' '){clearBtn(60); return;} // clear
    
    else if(btn.aVal > BTN_MIN && btn.prepare && btn.firstBtn == ' '){
      
      if(getFirstBtn(btn.aVal) == ' '){ clearBtn(60); return;}
      else{
        btn.aValPressed = btn.aVal; // needed for 2nd Btn
        btn.firstBtn = getFirstBtn(btn.aVal);
        btn.timer = millis();
        return;
      }

    }
    else if(btn.aVal < BTN_MIN && btn.firstBtn != ' '){btnHandler(btn.firstBtn, ' ');clearBtn(260);  return;}

    //when btn is not released and the analog value changed -- check seckend btn push
    else if( btn.aVal > BTN_MIN && btn.firstBtn != ' ' && (btn.aVal > btn.aValPressed + BTN_RANGE || btn.aVal < btn.aValPressed - BTN_RANGE)){
      if(getSecondBtn(btn.aVal, btn.firstBtn) != ' '){btnHandler(btn.firstBtn, getSecondBtn(btn.aVal, btn.firstBtn)); btn.blocked = true; return;}
      else{ clearBtn(60); return;}
    }
}

void clearBtn(int addDelay){
      btn.aVal = 0;
      btn.firstBtn = ' ';
      btn.aValPressed = 0;
      btn.wait = millis() + addDelay;
      btn.prepare =false;
      btn.blocked = false;
}

char getFirstBtn(uint16_t valIn){
           if(valIn > BTN_A_VAL - BTN_RANGE && valIn < BTN_A_VAL + BTN_RANGE){ return 'A'; }
      else if(valIn > BTN_B_VAL - BTN_RANGE && valIn < BTN_B_VAL + BTN_RANGE){ return 'B'; }
      else if(valIn > BTN_C_VAL - BTN_RANGE && valIn < BTN_C_VAL + BTN_RANGE){ return 'C'; }
      else{ return ' '; }
}
char getSecondBtn(uint16_t valIn, char first){
       if(first == 'A' && valIn > BTN_AB_VAL - BTN_RANGE && valIn < BTN_AB_VAL + BTN_RANGE){ return 'B';}
  else if(first == 'B' && valIn > BTN_AB_VAL - BTN_RANGE && valIn < BTN_AB_VAL + BTN_RANGE){ return 'A';}
  else if(first == 'C' && valIn > BTN_BC_VAL - BTN_RANGE && valIn < BTN_BC_VAL + BTN_RANGE){ return 'B';}
  else if(first == 'B' && valIn > BTN_BC_VAL - BTN_RANGE && valIn < BTN_BC_VAL + BTN_RANGE){ return 'C';}
  else{return ' ';}
}

void btnHandler(char firstBtn, char secondBtn){
  unsigned long btn_ms = (millis() - btn.timer);

 uint8_t btnResult = 0;
  if(secondBtn == ' '){
         if(firstBtn == 'A' && btn_ms <  LONGTIME){btnResult = A_SHORT;}
    else if(firstBtn == 'B' && btn_ms <  LONGTIME){btnResult = B_SHORT;}
    else if(firstBtn == 'C' && btn_ms <  LONGTIME){btnResult = C_SHORT;}
    else if(firstBtn == 'A' && btn_ms > LONGTIME){btnResult = A_LONG;}
    else if(firstBtn == 'B' && btn_ms > LONGTIME && btn_ms < MAXTIME){btnResult = B_LONG;}
    else if(firstBtn == 'C' && btn_ms > LONGTIME){btnResult = C_LONG;}
    else if(firstBtn == 'B' && btn_ms > MAXTIME){btnResult = B_ULONG;}
  }
  else{
          if(firstBtn == 'A' && secondBtn == 'B'){btnResult = BTN_AB;}
     else if(firstBtn == 'B' && secondBtn == 'A'){btnResult = BTN_BA;}
     else if(firstBtn == 'C' && secondBtn == 'B'){btnResult = BTN_CB;}
     else if(firstBtn == 'B' && secondBtn == 'C'){btnResult = BTN_BC;}
     else{clearBtn(60);return;}    
  }
  btnResultHandler(btnResult);
}

// wenn LED_Batt BTN A/B short change brightness
/*
LED_STATIC    1
LED_TRAIL     2
LED_ANI       3
LED_FIRE      4
LED_BATT      5

B short change mode
B long change mode
b uLong toggle bt
LED_ANI || LED_FIRE
A || C Short change ani
A || C Long change palette
LED static || LED trails
A || C Short change BMP
A || C Long change Folder
LED Batt
A || C Short change bright
 */
void btnResultHandler(uint8_t btnCode){
  //if(cfg.ledMode == LED_STATIC || cfg.ledMode == LED_TRAIL){ // 
  if(btnCode == B_SHORT && bleMode != BLE_OFF){Serial.print("BLE OFF"); stopBLE();}
  else if(btnCode == B_SHORT && bleMode == BLE_OFF){switchLedMode(1);}
  else if(btnCode == B_LONG){switchLedMode(-1);}
  else if(btnCode == B_ULONG && bleMode == BLE_OFF){Serial.print("BLE ON"); startBLE();}
  
  
  if(cfg.ledMode == LED_FIRE || cfg.ledMode == LED_ANI ){  
    if(btnCode == A_LONG){nextPalette();}
    else if(btnCode == C_LONG){lastPalette(); }
  }
  
  if(cfg.ledMode == LED_ANI){
    if(btnCode == A_SHORT){nextAni();}
    else if(btnCode == C_SHORT){lastAni(); }
  }
  
  if(cfg.ledMode == LED_STATIC || cfg.ledMode == LED_TRAIL){
        switch (btnCode) {
        case A_SHORT: ledsChangeBmp(1);  break;
        case A_LONG:  ledsChangeFld(1);  break;
        case C_SHORT: ledsChangeBmp(-1); break;
        case C_LONG:  ledsChangeFld(-1); break;
        }
  }
  if(cfg.ledMode == LED_BATT){
         if(btnCode == A_SHORT){changeBright(1);}
    else if(btnCode == C_SHORT){changeBright(-1);}
    else if(btnCode == A_LONG){changeBright(5);}
    else if(btnCode == C_LONG){changeBright(-5);}
  }
/*
 * 
    switch (btnCode) {
      case A_SHORT:  msgln("B_LONG");  break;
      case A_LONG:   msgln("B_LONG");  break;
      case C_SHORT:  msgln("B_LONG");  break;
      case C_LONG:   msgln("B_LONG");  break;
      case B_SHORT:  msgln("B_SHORT"); break;
      case B_LONG:   msgln("B_LONG");  break;
      case B_ULONG:  msgln("B_ULONG"); break;
      case BTN_BA:   msgln("BTN_BA");  break;
      case BTN_BC:   msgln("BTN_BC");  break;
      case BTN_AB:   msgln("BTN_AB");  break;
      case BTN_CB:   msgln("BTN_CB");  break;
      default:  break; 
    }
  */  


}
