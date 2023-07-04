
// FFT Peak max value = 200
void start_FFT(){
  PDM.onReceive(onPDMdata); 
  PDM.setGain(audioGain); 
  PDM.begin(1, SAMPLING_FREQUENCY);
  fftMode = true; // 
  eqStart = millis(); // FFT will start with a blast. to avoid this give it one second to sample enough data. this var will measure the time
}

void stop_FFT(){ //stop & reset variables
  PDM.end(); 
  for (byte band = 0; band <= 7; band++) {EQ[band].peak = 0;EQ[band].lastpeak = 0;}
  samples_ready = false; 
  fftStep = 0;
  fftMode = false;
}
  
void onPDMdata() {// Microphone Callback
  int bytesAvailable = PDM.available();  // query the number of bytes available
  PDM.read(sampleBuffer, bytesAvailable); // read into the sample buffer
  samplesRead = bytesAvailable / 2; // 16-bit, 2 bytes per sample
  samples_ready = true;
}


void FFT_main(){ // split FFT into 3 steps to render led frames inbetween
  if(fftMode){
    if(samples_ready){
  
      switch (fftStep) {
      case 0:
            for(int i = 0; i < samplesRead; i++){ vReal[i] = sampleBuffer[i];  vImag[i] = 0; } //transcribe samples
            FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD); // ~ 6ms
            fftStep++;
        break;
      case 1:
            FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD); // ~ 17ms
            fftStep++;
        break;
        case 2:
            FFT.ComplexToMagnitude(vReal, vImag, SAMPLES); // ~ 5ms
  
            for (int i = 2; i < (SAMPLES/2); i++){  // Don't use sample 0 or 1 and only first SAMPLES/2 are usable. 
                  if (vReal[i] > audioThresh) { // Add a crude noise filter, 10 x amplitude or more
                    double vRealNew;
                    if(i <= 112){ vRealNew = freqCalib[i] * ((float)vReal[i]/400.0); } // apply band calibration for each idx_sample7/2 
                    else{ vRealNew = 15.0 * ((float)vReal[i]/400.0); } // apply in. calib value for samples 113 to N/2
                    byte bandNum = getBand(i);
                    if(bandNum!=8) { setBand(bandNum, (int)vRealNew); }// (int)vReal[i]/EQ[bandNum].amplitude
                  }
            }
            decayBand();
            samples_ready = false;
            fftStep = 0;
        break;
      default: break;
      } // end of case
    } // end of if(samples_ready);
  }
}



byte getBand(int i) { //group all amplitude values into these 8 bands:
  
  if (i<=2 )              return 0;  //0    -> 128  Hz
  if (i ==3   )           return 1;  //129  -> 256  Hz
  if (i >=4   && i<=5 )   return 2;
  if (i >=5   && i<=8 )   return 3;  //266  -> 512  Hz
  if (i >=9   && i<=14 )  return 4;  //513  -> 1024 Hz    
  if (i >=15  && i<= 25 ) return 5;  //1025 -> 2048 Hz    
  if (i >=26  && i<= 44)  return 6;  //2048 -> 4096 Hz  
  if (i >=45  )           return 7;  //4096 -> 7040 Hz   
  //if (i >=111 && i<=128)  return 7;  //7040 -> 8192 Hz     (&&<=128)
  return 8;
}

void setBand(int band, int dsize){
  if(millis() > eqStart+800){ // wait until initial FFT blast ist gone 
    int dmax = 200;
    if (dsize > dmax) dsize = dmax;
    if (dsize > EQ[band].peak) { EQ[band].peak = dsize;}
    if (dsize > eqRange.ampMax) { eqRange.ampMax = dsize; eqRange.ampMax_t = millis();}
    if (dsize < eqRange.ampMin) { eqRange.ampMin = dsize; eqRange.ampMin_t = millis();}
    EQ[band].lastval = dsize;
    EQ[band].lastmeasured = millis();
  }
}




void decayBand(){
      long vnow = millis();
      for (byte band = 0; band <= 7; band++) {
        if(vnow - EQ[band].lastmeasured > 25) { setBand(band, EQ[band].lastval>4 ? EQ[band].lastval-4 : 0); } // auto decay every 50ms on low activity bands
        if (EQ[band].peak > 0) {
          EQ[band].peak -= 2;
          if(EQ[band].peak<=0) { EQ[band].peak = 0;
          }
        }
        if(EQ[band].lastpeak != EQ[band].peak) { EQ[band].lastpeak = EQ[band].peak; } // delete last peak
      }
 // if amplitude min max value is older than 3 sec lower max & rise min value
  if(vnow - eqRange.ampMax_t > eqRange.decay && eqRange.ampMax > 1) { eqRange.ampMax--;}
  if(vnow - eqRange.ampMin_t > eqRange.decay){
    if(eqRange.ampMax < eqRange.ampMin){ eqRange.ampMin = eqRange.ampMax -1; }
    else if(eqRange.ampMin < eqRange.ampMax-1) {eqRange.ampMin ++;}
  }
}

void printEQ(){
  for (byte band = 0; band <= 7; band++) { Serial.print(EQ[band].peak);Serial.print('\t');}
  Serial.println();
}
 
  
