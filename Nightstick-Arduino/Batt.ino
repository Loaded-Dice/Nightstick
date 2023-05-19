void setup_Batt(){
  
  pinMode(PIN_VBAT,INPUT);
  pinMode(VBAT_ENABLE,OUTPUT);
  digitalWrite(VBAT_ENABLE,LOW); // keep this pin low 
  analogReference(AR_INTERNAL_3_0);
  analogReadResolution(12);
}

void main_Batt(){ EVERY_N_MILLIS(DELAYMS_READBATT){  readBatt(); }}


uint8_t nMeasBat = 0; // keep track of measurement count 
int tempBat = 0; // adding up read values to get avg vale

void readBatt(){
  tempBat += analogRead(PIN_VBAT);
  nMeasBat++;
  if(nMeasBat >= AVG_BATTREAD || vBat == 0.0){
    float f_tempBat = ((float)tempBat / (float)nMeasBat);
    rawBat = f_tempBat;
     vBat = (float)vBatLookUp(rawBat) / 100; // use lookup table to convert raw value to voltage
     if(vBat <3.35){pBat = 0;}
     else if(vBat >= 4.17){pBat = 100;}
     else{pBat = map(vBat,3.35,4.17,0,100);}
     
     if(bleMode != BLE_OFF){blebas.write(pBat);}
     tempBat = 0;
     nMeasBat = 0;
     f_tempBat = (f_tempBat / 51.0 * 151.0);
     maxBat = f_tempBat; // save scaled raw battery volage to have static btnRead values btn A pressed @3.6V  analogRead(BTN_PIN)/maxBat = @4.2V  analogRead(BTN_PIN)/maxBat

  }
}
uint16_t vBatLookUp(uint16_t raw){ // returns vBat*100
  if( raw < rawLookUp[0]){ return vLookUp[0];}
  else if(raw > rawLookUp[SIZE(rawLookUp)-1]){return vLookUp[SIZE(rawLookUp)-1];}
  else{
    for(uint8_t i = 0; i < SIZE(rawLookUp); i++){
      if(raw > rawLookUp[i] && raw < rawLookUp[i+1]){return map(raw,rawLookUp[i],rawLookUp[i+1],vLookUp[i],vLookUp[i+1]);}    
    }
  }
  return 0;
}
