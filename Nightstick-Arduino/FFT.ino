
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
const float freqCalib[113] = {0.0,  0.0, 1.3079793,  1.069392, 0.784344, 0.9103336,  1.0166674,  0.862052, 0.7109092,  0.6427657,  0.7753275,  0.6113147,  0.4977038,  0.6528794,  0.4102594,  0.3913894,  0.7237714,  0.5113062,  0.491581, 0.6531831,  0.8559368,  1.4665207,  1.9665577,  2.1739053,  2.4351664,  2.1376235,  1.8633525,  1.8255845,  2.180384, 1.5464601,  1.6231678,  2.450974, 2.2835728,  2.2933477,  2.4481844,  2.6828074,  2.9802801,  3.1908219,  3.1528289,  3.3493042,  3.9719504,  5.3594922,  6.0969157,  7.5512116,  6.8002132,  6.2995524,  7.049264, 8.5025804,  8.6423623,  8.5025804,  6.2421385,  5.8940065,  5.8308792,  7.1547174,  5.6450937,  4.7016548,  2.9953562,  2.289182, 1.8821454,  2.0298621,  1.9277527,  1.6978568,  2.00691,  1.6753267,  1.6483045,  1.9788069,  2.20481,  2.2698795,  2.2922805,  2.339455, 2.2609692,  2.3103107,  2.9767121,  3.5016091,  3.7774394,  3.5720545,  3.2492898,  3.0435062,  3.0108962,  3.2872719,  3.3061042,  2.6788108,  2.5514675,  2.6003714,  2.9922521,  3.7972216,  5.1530411,  4.964938, 5.2497917,  5.4095423,  6.6076133,  8.1143134,  9.2289323,  9.2289323,  8.9301363,  8.1143134,  7.7601315,  6.8622778,  6.7801759,  7.435715, 8.5025804,  10.2971722, 13.7245555, 16.2286268, 17.8602727, 21.4158875, 22.3687344, 20.6004929, 21.4158875, 19.8597521, 22.3687344, 20.6004929, 18.4663333};


void FFT_main(){ // split FFT into 3 steps to render led frames inbetween

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
                if (vReal[i] > 1000) { // Add a crude noise filter, 10 x amplitude or more
                  if(i <= 112){ vReal[i] = freqCalib[i] * ((float)vReal[i]/400.0); } // apply band calibration for each idx_sample7/2 
                  else{ vReal[i] = 15.0 * ((float)vReal[i]/400.0); }
                  byte bandNum = getBand(i);
                  if(bandNum!=8) { setBand(bandNum, (int)vReal[i]); }// (int)vReal[i]/EQ[bandNum].amplitude
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
}

void printEQ(){
  for (byte band = 0; band <= 7; band++) { Serial.print(EQ[band].peak);Serial.print('\t');}
  Serial.println();
}
 
  
