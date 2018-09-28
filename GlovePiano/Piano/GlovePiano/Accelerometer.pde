class AccelerometerListener implements SensorEventListener {
  float xaccel,yaccel,zaccel;
  float yaw = 0;
  public void onSensorChanged(SensorEvent event) {
    xaccel = event.values[0];
    yaccel = event.values[1];
    zaccel = event.values[2];    
    
    
  }
  public void onAccuracyChanged(Sensor sensor, int accuracy) {
  }
  float x() {
    return this.xaccel;
  }
  float y() {
    return this.yaccel;
  }
  float z() {
    return this.zaccel;
  }
  float[] getAccels() {
    float[] res = new float[3];
    res[0] = x();
    res[1] = y();
    res[2] = z();
    return res;
  }
  
  float[] getPosition() {
    float x = x();
    float y = y();
    float z = z();
    float[] res = new float[3];
    float roll = floor(degrees(atan2(-x, sqrt(y * y + z * z))));
    float pitch = floor(degrees(atan2(y,z))); 
    res[0] = roll;
    res[1] = pitch;
    res[2] = this.yaw;
    return res;
  }

  String accelsToString() {
    float[] pos = getAccels();
    String res = "x: " + pos[0];
    res += "\ty: " + pos[1];
    res += "\tz: " + pos[2];
    res += "\n";
    return res;
  }
  String rollPitchToString(float[] data) {
    String res = "Roll: " + data[0];
    res += "\tPitch: " + data[1];
    res += "\tYaw: " + data[2];
    res += "\n";
    return res;
  }
  int roll() {
    float[] data = getPosition();
    return (int) data[0];
  }
  int pitch() {
    float[] data = getPosition();
    return (int) data[1];
  }
  int yaw() {
    float[] data = getPosition();
    return (int) data[2];
  }
  
  String toString() {
    float[] pos = getPosition();
    String res = "Roll: " + pos[0];
    res += "\tPitch: " + pos[1];
    res += "\tYaw: " + pos[2];
    res += "\n";
    return res;
  }
  String toSafeDiscreteString() {
    float[] pos = makeSafe(getPosition());
    pos[0] = floor(pos[0]/abs(pos[0]));
    pos[1] = floor(pos[1]/abs(pos[1]));
    //pos[2] = 0;
    //pos[2] = -USER_INPUT_YAW_LEFT_RIGHT_OR_NEUTRAL;  //Activate discrete controls. CCW is negative
    
    return toString(pos);
  }
  
  String toString(float[] data) {
    String res = "";
    for(float f: data) {
      res += floor(f) + ",";
    }
    return res;
  }
  
  float[] makeSafe(float[] data) {
    float[] res = data;
    float x = x(), y = y(), z = z();
    float baseLine = 9.81;
    float safetyMargin = 5;
    float actualReading = sqrt(x*x + y*y + z*z);
    
    
    /*
    DANGER
    */
    if(abs(baseLine - actualReading) > safetyMargin) {
      displayDanger("PHONE DROPPED");
      res[0] = 0;
      res[0] = 0;
      res[0] = this.yaw;
      return res;
    }
    if(abs(res[1]) > 80){
      displayDanger("PEEKABOO!\nUPSIDE DOWN?");
      res[0] = 0;
      res[0] = 0;
      res[0] = this.yaw;
      return res;
    }
    if(abs(res[0]) > 60){
      displayDanger("TOO MUCH ROLL");
      res[0] = 60;
      return res;
    }
    if(abs(res[1]) > 60) {
      displayDanger("TOO MUCH PITCH");
      res[1] = 60;
      return res;
    }
    
    /*
    User experience improvements
    */
    
    //User attemps to keep phone flat
    if(abs(res[0]) <= 10) {
      res[0] = 0;
    }
    if(abs(res[1]) <= 10) {
      res[1] = 0;
    }
    //inputs are trimmed for safe flying
    if(abs(res[0]) > 10) {
      if(res[0] >= 0) {
        res[0] -= 10;
      } else {
        res[0] += 10;
      }
    }
    if(abs(res[1]) > 10) {
      if(res[1] >= 0) {
        res[1] -= 10;
      } else {
        res[1] += 10;
      }
    }
    
    
    
    return res;
  }
}

void displayDanger(String s) {
  displayDanger();
  pushStyle();
  textSize(100);
  textAlign(CENTER);
  fill(0);
  text(s, width/2, height/2 + 200);
  popStyle();
}

void displayDanger() {
  background(255, 128, 0);
  pushStyle();
  textSize(100);
  textAlign(CENTER);
  fill(0);
  text("DANGER!", width/2, height/2);
  popStyle();
}


/*
  Stops power consumption while app is not active
*/
public void onResume() {
  super.onResume();
  if (manager != null) {
    manager.registerListener(virtual_accelerometer, sensor, SensorManager.SENSOR_DELAY_GAME);
  }
}
public void onPause() {
  super.onPause();
  if (manager != null) {
    manager.unregisterListener(virtual_accelerometer);
  }
}