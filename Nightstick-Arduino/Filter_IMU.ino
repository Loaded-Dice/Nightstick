
void start_FILTER_IMU(){
    if (myIMU.begin() != 0)  { imuMode = OFF;}
    else{ imuMode = ON;} // NO DESTRUCTOR - filter.begin will only set / change frequency
  }
  
void stop_FILTER_IMU(){  imuMode = OFF; }


void main_FILTER_IMU(){
  if(imuMode == ON){
    unsigned long tmeas;
    int deltaMeas;
    EVERY_N_MILLIS(DELAYMS_FILTER){
        imuTemp = myIMU.readTempC();
        deltat = fusion.deltatUpdate(); //this have to be done before calling the fusion update
        fusion.MahonyUpdate(RAD(myIMU.readFloatGyroX()), RAD(myIMU.readFloatGyroY()), RAD(myIMU.readFloatGyroZ()), myIMU.readFloatAccelX(), myIMU.readFloatAccelY(), myIMU.readFloatAccelZ(), deltat);
        pitch = fusion.getPitch();
        roll = fusion.getRoll();
        float yawNew = fusion.getYaw();
        yaw16 = (uint16_t)(yaw * 182.044444);
        roll16 = (uint16_t)((roll+180)/360.0*65535);
        pitch16 = (uint16_t)((pitch+90)/360.0*65535);
        yawDelta = yawNew-yaw; 
        yaw=yawNew;
        deltaMeas = (millis()-tmeas);
        tmeas=millis();
    }
  }
}

/*
void start_FILTER_IMU(){
    if (!IMU.begin()) { imuMode = OFF;}
    else{filter.begin(MS2FREQ(DELAYMS_FILTERIMU)); imuMode = ON;} // NO DESTRUCTOR - filter.begin will only set / change frequency
  }
  
void stop_FILTER_IMU(){  IMU.end(); imuMode = OFF; }

void main_FILTER_IMU(){
  if(imuMode == ON){
    EVERY_N_MILLIS(DELAYMS_FILTERIMU){
        if (IMU.gyroscopeAvailable()) { IMU.readGyroscope(gx, gy, gz);}
        if (IMU.accelerationAvailable()) {IMU.readAcceleration(ax, ay, az);}
        filter.updateIMU(gx, gy, gz, ax, ay, az);
        //yawDelta = yaw; 
        yaw = filter.getYaw(); // rotation along Z --> main rotation aka "yaw" (in degree)
        yaw16 = (uint16_t)((yaw+180)/360.0*65535);
        yaw = RAD(yaw)+M_PI;
        roll = filter.getRoll(); // rotation around the stick axis
        roll16 = (uint16_t)((roll+180)/360.0*65535);
        roll = RAD(roll)+M_PI;
        pitch = filter.getPitch(); // tilt towards gravity
        
    }
  }
  else{Serial.println("HALLO?");}
}
*/
