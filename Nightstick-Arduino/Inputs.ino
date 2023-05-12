void setup_Inputs(){ pinMode(BTN_PIN,INPUT); }

void main_Inputs(){ EVERY_N_MILLIS(DELAYMS_READBTN){readBtn();}}

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

    //when btn is not released and the analog value changed -- chk 4 2nd btn 
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

  if(secondBtn == ' '){
         if(firstBtn == 'A'){ Serial.print("Btn A pressed for "); Serial.print( btn_ms ); Serial.print(" ms  ");Serial.println(btn_ms > LONGTIME ? "(LONG)" : "(SHORT)");}
    else if(firstBtn == 'B'){Serial.print("Btn B pressed for "); Serial.print( btn_ms ); Serial.print(" ms  ");Serial.println(btn_ms > LONGTIME ? "(LONG)" : "(SHORT)");}
    else if(firstBtn == 'C'){Serial.print("Btn C pressed for "); Serial.print( btn_ms ); Serial.print(" ms  ");Serial.println(btn_ms > LONGTIME ? "(LONG)" : "(SHORT)");}
  }
  else{
     if(firstBtn == 'A' && secondBtn == 'B'){Serial.println("Btn A and than B pressed");}
     else if(firstBtn == 'B' && secondBtn == 'A'){Serial.println("Btn B and than A pressed");}
     else if(firstBtn == 'C' && secondBtn == 'B'){Serial.println("Btn C and than B pressed");}
     else if(firstBtn == 'B' && secondBtn == 'C'){Serial.println("Btn B and than C pressed");}
     else{Serial.println("Something is wrong. Unknown Button combination");}    
  }
}
