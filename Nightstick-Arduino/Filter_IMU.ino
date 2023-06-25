

void start_FILTER_IMU(){
    if (!IMU.begin()) { imuMode = OFF;}
    else{filter.begin(MS2FREQ(DELAYMS_FILTERIMU));imuMode = ON;} // NO DESTRUCTOR - filter.begin will only set / change frequency
  }
  
void stop_FILTER_IMU(){  IMU.end(); imuMode = OFF; }

void main_FILTER_IMU(){
  if(imuMode == ON){
    EVERY_N_MILLIS(DELAYMS_FILTERIMU){
        if (IMU.gyroscopeAvailable()) { IMU.readGyroscope(gx, gy, gz);}
        if (IMU.accelerationAvailable()) {IMU.readAcceleration(ax, ay, az);}
        filter.updateIMU(gx, gy, gz, ax, ay, az);
        //yawLast = yaw; 
        yaw = filter.getYaw(); // rotation along Z --> main rotation aka "yaw" (in degree)
        yaw16 = (uint16_t)((yaw+180)/360.0*65535);
        yaw = RAD(yaw)+M_PI;
        roll = filter.getRoll(); // rotation around the stick axis
        roll16 = (uint16_t)((roll+180)/360.0*65535);
        roll = RAD(roll)+M_PI;
        pitch = filter.getPitch(); // tilt towards gravity
    }
  }
}
