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

//--=={DEFINITIONS - COMMON / Inputs}==--//
#define BTN_NONE    0
#define BTN_A       1
#define BTN_B       2
#define BTN_C       3
#define BTN_AB      4
#define BTN_BA      5
#define BTN_CB      6
#define BTN_BC      7
#define BTN_A_VAL       2504
#define BTN_B_VAL       1775
#define BTN_C_VAL       787
#define BTN_AB_VAL      2839
#define BTN_BC_VAL      2034
#define BTN_RANGE   50    //value +/- Range
#define BTN_MIN     500  // if value > BTN_MIN --> btn-pressed else relese
#define LONGTIME    350 // time in ms
#define BTN_SHORT   0
#define BTN_LONG    1
 */

void readBtn(){

    btn.aVal = (int)(((float)analogRead(BTN_PIN)/ ((float)analogRead(PIN_VBAT) / 510.0 * 1510.0)) * 4095);
    if(btn.firstBtn == ' '
    if(btn.waitActive && btn.aVal < BTN_MIN){
          btn.waitActive = false;
          btn.wait = millis() + 600;
          btn.aValPressed = 0;
          btn.firstBtn = ' ';
          Serial.println("release");

    }
    else if(!btn.waitActive && millis() > btn.wait ){
        
        if(btn.firstBtn == ' ' && getFirstBtn(btn.aVal) != ' '){
          btn.firstBtn = getFirstBtn(btn.aVal);
          btn.wait = millis() + 60;
          btn.timer = millis();
          btn.aValPressed = btn.aVal;
          return;
        }
        
             if(btn.firstBtn == 'A' && btn.aVal < BTN_MIN){ btnHandler(BTN_A);}
        else if(btn.firstBtn == 'B' && btn.aVal < BTN_MIN){ btnHandler(BTN_B);}
        else if(btn.firstBtn == 'C' && btn.aVal < BTN_MIN){ btnHandler(BTN_C);}
        else if((btn.firstBtn != ' ') &&  (getBtnResult(btn.aVal, btn.firstBtn) != BTN_NONE)){btnHandler(getBtnResult(btn.aVal, btn.firstBtn));}
        
        //else if((btn.firstBtn != ' ') && ( btn.aVal > (btn.aValPressed + BTN_RANGE) || btn.aVal < (btn.aValPressed - BTN_RANGE))){
        
           //  if(getBtnResult(btn.aVal, btn.firstBtn) != BTN_NONE){btnHandler(getBtnResult(btn.aVal, btn.firstBtn));}
       // }

    }
}




/*



    //-------------------------------------------

    

        if(btn.pressed ==  BTN_NONE && btn.aVal > BTN_MIN){
            
            if(getBtnChar(btn.aVal) != BTN_NONE){
              btn.pressed = getBtnChar(btn.aVal);
              btn.timer = millis();
              btn.aValPressed = btn.aVal;
              }
        }
        else if(btn.pressed != BTN_NONE){
            if((btn.pressed == BTN_A ||btn.pressed == BTN_B ||btn.pressed == BTN_C) && btn.aVal < BTN_MIN){
              
            }
            else if((btn.pressed == BTN_A ||btn.pressed == BTN_B || btn.pressed == BTN_C) ){ 
              
              if(getBtnChar(btn.aVal) != BTN_NONE && ( btn.aVal < btn.aValPressed - BTN_RANGE || btn.aVal > btn.aValPressed + BTN_RANGE) ){btn.pressed = getBtnChar(btn.aVal);
            }

              
            if(btn.aVal < BTN_MIN){ 
            if( btn.aVal < btn.aValPressed - BTN_RANGE || btn.aVal > btn.aValPressed + BTN_RANGE){ 
              if(getBtnChar(btn.aVal) != BTN_NONE){
              btn.pressed = getBtnChar(btn.aVal);
              }
        
          getBtnChar(btn.aVal)
          
          if(btn.pressed != BTN_NONE){btnHandler();}
          }
        else if(btn.pressed != BTN_NONE && btn.aVal < BTN_MIN){ 
          
          btnHandler();
          }
    }
 //   if(handle){btnHandler();}
*/

char getFirstBtn(uint16_t valIn){
           if(valIn > BTN_A_VAL - BTN_RANGE && valIn < BTN_A_VAL + BTN_RANGE){ return 'A'; }
      else if(valIn > BTN_B_VAL - BTN_RANGE && valIn < BTN_B_VAL + BTN_RANGE){ return 'B'; }
      else if(valIn > BTN_C_VAL - BTN_RANGE && valIn < BTN_C_VAL + BTN_RANGE){ return 'C'; }
      else{ return ' '; }
}
uint8_t getBtnResult(uint16_t valIn, char first){
       if(first == 'A' && valIn > BTN_AB_VAL - BTN_RANGE && valIn < BTN_AB_VAL + BTN_RANGE){ return BTN_AB;}
  else if(first == 'B' && valIn > BTN_AB_VAL - BTN_RANGE && valIn < BTN_AB_VAL + BTN_RANGE){ return BTN_BA;}
  else if(first == 'C' && valIn > BTN_BC_VAL - BTN_RANGE && valIn < BTN_BC_VAL + BTN_RANGE){ return BTN_CB;}
  else if(first == 'B' && valIn > BTN_BC_VAL - BTN_RANGE && valIn < BTN_BC_VAL + BTN_RANGE){ return BTN_BC;}
  else{return BTN_NONE;}
}

/*
uint8_t getBtnChar(uint16_t valIn, ){
  
  if(btn.pressed == BTN_NONE){
           if(valIn > BTN_A_VAL - BTN_RANGE && valIn < BTN_A_VAL + BTN_RANGE){return BTN_A;Serial.println("*1*");}
      else if(valIn > BTN_B_VAL - BTN_RANGE && valIn < BTN_B_VAL + BTN_RANGE){return BTN_B;Serial.println("*2*");}
      else if(valIn > BTN_C_VAL - BTN_RANGE && valIn < BTN_C_VAL + BTN_RANGE){return BTN_C;Serial.println("*3*");}
      else{return BTN_NONE; Serial.println("*4*");}
  }
  else if(btn.pressed == BTN_A && valIn > BTN_AB_VAL - BTN_RANGE && valIn < BTN_AB_VAL + BTN_RANGE){return BTN_AB;Serial.println("*a*");}
  else if(btn.pressed == BTN_B && valIn > BTN_AB_VAL - BTN_RANGE && valIn < BTN_AB_VAL + BTN_RANGE){return BTN_BA;Serial.println("*b*");}
  else if(btn.pressed == BTN_C && valIn > BTN_BC_VAL - BTN_RANGE && valIn < BTN_BC_VAL + BTN_RANGE){return BTN_CB;Serial.println("*c*");}
  else if(btn.pressed == BTN_B && valIn > BTN_BC_VAL - BTN_RANGE && valIn < BTN_BC_VAL + BTN_RANGE){return BTN_BC;Serial.println("*d*");}
  else{return ' ';Serial.println("*e*");}
}
*/
void btnHandler(uint8_t result){
  unsigned long btn_ms = (millis() - btn.timer);
  if(result == BTN_NONE){return;}
  switch(result){
    case BTN_A  : Serial.print("Btn A pressed for "); Serial.print( btn_ms ); Serial.print(" ms  ");Serial.println(btn_ms > LONGTIME ? "(LONG)" : "(SHORT)");
    case BTN_B  : Serial.print("Btn B pressed for "); Serial.print( btn_ms ); Serial.print(" ms  ");Serial.println(btn_ms > LONGTIME ? "(LONG)" : "(SHORT)");
    case BTN_C  : Serial.print("Btn C pressed for "); Serial.print( btn_ms ); Serial.print(" ms  ");Serial.println(btn_ms > LONGTIME ? "(LONG)" : "(SHORT)");
    case BTN_AB : Serial.println("Btn A and than B pressed");
    case BTN_BA : Serial.println("Btn B and than A pressed");
    case BTN_CB : Serial.println("Btn C and than B pressed");
    case BTN_BC : Serial.println("Btn B and than C pressed");
    default : Serial.println("Something is wrong");
  }
  btn.waitActive = true;

}
