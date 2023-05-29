void setup_FILTER_IMU(){
    if (!IMU.begin()) { msgln("Failed to initialize IMU!"); while (1);}
    filter.begin(MS2FREQ(DELAYMS_FILTERIMU)); // filter to expect 100 measurements per second
  }


void main_FILTER_IMU(){
  EVERY_N_MILLIS(DELAYMS_FILTERIMU){
      if (IMU.gyroscopeAvailable()) { IMU.readGyroscope(gx, gy, gz);}
      if (IMU.accelerationAvailable()) {IMU.readAcceleration(ax, ay, az);}
      filter.updateIMU(gx, gy, gz, ax, ay, az);
      rotAngleLast = rotAngle;
      rotAngle = filter.getYaw(); // rotation along Z --> main rotation aka "yaw" (in degree)
      rotAngle = RAD(rotAngle);
      roll = filter.getRoll(); // rotation around the stick axis
      roll = RAD(roll);
      pitch = filter.getPitch(); // tilt towards gravity
  }
}
void getQuaternion(){
  bool useFast = false; // true uses sin16 & cos16 from FastLed, false uses standard implementation
  float cosHalfRoll = cosHalf(roll , useFast);
  float sinHalfRoll = sinHalf(roll , useFast);
  float cosHalfPitch = cosHalf(pitch , useFast);
  float sinHalfPitch = sinHalf(pitch , useFast);
  float cosHalfYaw = cosHalf(yaw , useFast);
  float sinHalfYaw = sinHalf(yaw , useFast);


  qt.w = cosHalfRoll * cosHalfPitch * cosHalfYaw + sinHalfRoll * sinHalfPitch * sinHalfYaw;
  qt.x = sinHalfRoll * cosHalfPitch * cosHalfYaw - cosHalfRoll * sinHalfPitch * sinHalfYaw;
  qt.y = cosHalfRoll * sinHalfPitch * cosHalfYaw + sinHalfRoll * cosHalfPitch * sinHalfYaw;
  qt.z = cosHalfRoll * cosHalfPitch * sinHalfYaw - sinHalfRoll * sinHalfPitch * cosHalfYaw;
}
//int16_t sin16_avr  ( uint16_t  theta ) is equal to float s = sin(x) * 32767.0;
//int16_t sin16_C ( uint16_t  theta ) is equal to float s = sin(x) * 32767.0;
//int16_t cos16 ( uint16_t  theta ) // float s = cos(x) * 32767.0;
// 16 bit unsigned --> 0 bis 65535
// 16 bit signed   --> -32768 to +32767
//    uint16_t raw = 182.0416 * i;
//    float sinFast = sin16(raw) / 32767.0;
//    float cosFast = cos16(raw) / 32767.0;

float sinHalf(float angleDeg, bool fast){
  if(fast){ return sin16((65535.0/360.0) * angleDeg / 2.0) / 32767.0 ;}
  else{     return sin(RAD(angleDeg)/2.0);}
}
float cosHalf(float angleDeg, bool fast){
  if(fast){ return cos16((65535.0/360.0) * angleDeg / 2.0) / 32767.0 ;}
  else{     return cos(RAD(angleDeg)/2.0);}
}

void getGravity() { //getGravity(VectorFloat *v, Quaternion *q) {
    gravity.x = 2 * (qt.x*qt.z - qt.w*qt.y);
    gravity.y = 2 * (qt.w*qt.x + qt.y*qt.z);
    gravity.z = qt.w*qt.w - qt.x*qt.x - qt.y*qt.y + qt.z*qt.z;
}
float vDotproduct(VectorFloat v1, VectorFloat v2) { //
  float s;
  s= (v1.x*v2.x) + (v1.y*v2.y) + (v1.z*v2.z);
  return s;
}

VectorFloat crossProduct(VectorFloat a, VectorFloat b) {  
  VectorFloat p;
  p.x=       (a.y*b.z) - (a.z*b.y);
  p.z=       (a.z*b.x) - (a.x*b.z);
  p.y=       (a.x*b.y) - (a.y*b.x);
  return p;
}
float vMag(VectorFloat a){ // get length of vector
  float b;
  b=sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
  return b;
}
float getAngleRad(VectorFloat a, VectorFloat b) {
  float s, c;
  s=(a.x*b.x) + (a.y*b.y) + (a.z*b.z);
  c = s / vMag(a) / vMag(b);
  return acos(c);//*57.29578;  -- > or *(180/pi) // radian to degree
}   
float getAngleDeg(VectorFloat a, VectorFloat b) {
   return DEG(getAngleRad(a, b));
}
