

void setup_Batt(){
  _eAnalogReference vRefAlias;
  if(vRefSet == 3.6){vRefAlias = AR_INTERNAL;}
  else if(vRefSet == 3.0){vRefAlias = AR_INTERNAL_3_0;}
  else if(vRefSet == 2.4){vRefAlias = AR_INTERNAL_2_4;}
  else if(vRefSet == 1.8){vRefAlias = AR_INTERNAL_1_8;}
  else if(vRefSet == 1.2){vRefAlias = AR_INTERNAL_1_2;}
  else{vRefSet = 3.0; vRefAlias = AR_INTERNAL_3_0;}
  
  pinMode(PIN_VBAT,INPUT);
  pinMode(VBAT_ENABLE,OUTPUT);
  digitalWrite(VBAT_ENABLE,LOW);
  analogReference(vRefAlias);
  analogReadResolution(12);
}

uint8_t nMeasBat = 0; // keep track of measurement count 
int tempBat = 0; // adding up read values to get avg vale

void readBatt(){
  //measured and interpolated  linReg  vBatReal = 1.1204*vBat - 0.2652
  //vBat correction factor = 1.1204
  //vBat correction offset = -0.2652
  //51 & 100 is the resistance relation of the voltage divider
  tempBat += analogRead(PIN_VBAT);
  nMeasBat++;
  if(nMeasBat >= AVG_BATTREAD || vBat == 0.0){
    float f_tempBat = ((float)tempBat / (float)nMeasBat);
    
     vBat = f_tempBat / 4096.0 * vRefSet / 51.0 * 151.0 * 1.1204 - 0.2652; 
     pBat = map(vBat,3.65,4.2,0,100);
     blebas.write(pBat);
     tempBat = 0;
     nMeasBat = 0;
     f_tempBat = (f_tempBat / 51.0 * 151.0);
     maxBat = f_tempBat; // save scaled raw battery volage to have static btnRead values btn A pressed @3.6V  analogRead(BTN_PIN)/maxBat = @4.2V  analogRead(BTN_PIN)/maxBat

  }
}
