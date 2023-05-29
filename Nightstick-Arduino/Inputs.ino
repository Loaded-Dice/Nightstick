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
// chk BTN for
/*
A short / long = next bmp/folder
B short / long = Toggle BT / BT pair
C short / long = last  bmp/folder
A -> B
B -> A
B -> C 
C -> B
*/
void btnHandler(char firstBtn, char secondBtn){
  unsigned long btn_ms = (millis() - btn.timer);

 uint8_t btnResult = 0;
  if(secondBtn == ' '){
         if(firstBtn == 'A' && btn_ms <  LONGTIME){btnResult = A_SHORT;}
    else if(firstBtn == 'B' && btn_ms <  LONGTIME){btnResult = B_SHORT;}
    else if(firstBtn == 'C' && btn_ms <  LONGTIME){btnResult = C_SHORT;}
    else if(firstBtn == 'A' && btn_ms >= LONGTIME){btnResult = A_LONG;}
    else if(firstBtn == 'B' && btn_ms >= LONGTIME && btn_ms < ULONGTIME){btnResult = B_LONG;}
    else if(firstBtn == 'C' && btn_ms >= LONGTIME){btnResult = C_LONG;}
    else if(firstBtn == 'C' && btn_ms >= ULONGTIME){btnResult = B_ULONG;}
  }
  else{
          if(firstBtn == 'A' && secondBtn == 'B'){btnResult = BTN_AB;}
     else if(firstBtn == 'B' && secondBtn == 'A'){btnResult = BTN_BA;}
     else if(firstBtn == 'C' && secondBtn == 'B'){btnResult = BTN_BC;}
     else if(firstBtn == 'B' && secondBtn == 'C'){btnResult = BTN_BC;}
     else{clearBtn(60);return;}    
  }
  btnResultHandler(btnResult);
}
void btnResultHandler(uint8_t btnCode){
  if(ledMode == LED_STATIC){ // LED_TRAIL

    switch (btnCode) {
      case A_SHORT: ledsChangeBmp(1);  break;
      case A_LONG:  ledsChangeFld(1);  break;
      case C_SHORT: ledsChangeBmp(-1);  break;
      case C_LONG:  ledsChangeFld(-1);  break;
      case B_ULONG:   break;
      case BTN_AB:    break;
      case BTN_CB:    break;
      default:  break; 
    }
  }
//ledMode
  
}
/*
A_SHORT
B_SHORT
C_SHORT
A_LONG 
B_LONG 
C_LONG 
B_ULONG
BTN_AB

BTN_CB
*/
//BTN_BA
//BTN_BC
