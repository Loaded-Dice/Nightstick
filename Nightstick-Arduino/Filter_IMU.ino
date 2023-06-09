void setup_FILTER_IMU(){
    if (!IMU.begin()) { msgln("Failed to initialize IMU!"); while (1);}
    filter.begin(MS2FREQ(DELAYMS_FILTERIMU)); // filter to expect 100 measurements per second
  }


void main_FILTER_IMU(){
  EVERY_N_MILLIS(DELAYMS_FILTERIMU){
      if (IMU.gyroscopeAvailable()) { IMU.readGyroscope(gx, gy, gz);}
      if (IMU.accelerationAvailable()) {IMU.readAcceleration(ax, ay, az);}
      filter.updateIMU(gx, gy, gz, ax, ay, az);
      //yawLast = rotAngle;
      yaw = filter.getYaw(); // rotation along Z --> main rotation aka "yaw" (in degree)
      yaw16 = (uint16_t)((yaw+180)/360.0*65535);
      yaw = RAD(yaw)+M_PI;
      roll = filter.getRoll(); // rotation around the stick axis
      roll16 = (uint16_t)((roll+180)/360.0*65535);
      roll = RAD(roll)+M_PI;
      pitch = filter.getPitch(); // tilt towards gravity
  }
}
