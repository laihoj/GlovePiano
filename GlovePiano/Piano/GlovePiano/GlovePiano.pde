/**
 * <p>Ketai Library for Android: http://ketai.org</p>
 *
 * <p>KetaiBluetooth wraps the Android Bluetooth RFCOMM Features:
 * <ul>
 * <li>Enables Bluetooth for sketch through android</li>
 * <li>Provides list of available Devices</li>
 * <li>Enables Discovery</li>
 * <li>Allows writing data to device</li>
 * </ul>
 * <p>Updated: 2012-05-18 Daniel Sauter/j.duran</p>
 
 */
 
 /**
Quadcopter bluetooth communicator
Mechatronics project Spring 2018
Author: Jaakko Laiho
Based on (loosely): "Updated: 2012-05-18 Daniel Sauter/j.duran" ketai bluetooth example 
 */

//required for BT enabling on startup
import android.content.Intent;
import android.os.Bundle;

//Processing android mode bluetooth library
import ketai.net.bluetooth.*;
import ketai.ui.*;
import ketai.net.*;
import oscP5.*;

//Accelerometer necessities
import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorManager;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;

//Declaring variables for accelerometer
Context context;
SensorManager manager;
Sensor sensor;
AccelerometerListener virtual_accelerometer;

//Declaring variables for Bluetooth
KetaiBluetooth bt;
String info = "";
KetaiList klist;

//Declaring environment variables
ArrayList<String> devicesDiscovered = new ArrayList();
boolean CONFIGURING = true;
boolean CALIBRATING = false;
color BACKGROUND_COLOR = color(255,255,255);
String UIText;

//********************************************************************
// The following code is required to enable bluetooth at startup.
//********************************************************************
void onCreate(Bundle savedInstanceState) {
  super.onCreate(savedInstanceState);
  bt = new KetaiBluetooth(this);
}

void onActivityResult(int requestCode, int resultCode, Intent data) {
  bt.onActivityResult(requestCode, resultCode, data);
}

//********************************************************************

void setup() {
  prepareExitHandler();
  //20 frames per second. Too high framerate -> microcontroller BT buffer becomes crowded
  frameRate(20);
  
  //Android sensor functionality necessities
  context = (Context) surface.getActivity();
  manager = (SensorManager)context.getSystemService(Context.SENSOR_SERVICE);
  sensor = manager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
  virtual_accelerometer = new AccelerometerListener();
  manager.registerListener(virtual_accelerometer, sensor, SensorManager.SENSOR_DELAY_GAME);
  
  
  orientation(PORTRAIT);
  fullScreen(P3D);
  background(255, 128, 0);
  stroke(255);
  textSize(24);

  //start listening for BT connections
  bt.start();

    
}

void draw() {
  drawConfigurationScreen();
  drawUI();
}


//Call back method to manage data received
void onBluetoothDataEvent(String who, byte[] data) {
  color NEW_COLOR = color(random(0,255),random(0,255),random(0,255));
  BACKGROUND_COLOR = NEW_COLOR;
  //if(isConfiguring)
  //  return;

  ////KetaiOSCMessage is the same as OscMessage
  ////   but allows construction by byte array
  //KetaiOSCMessage m = new KetaiOSCMessage(data);
  //if(m.isValid()) {
  //  if(m.checkAddrPattern("/remoteMouse/"))
  //  if(true) {
  //    if(m.checkTypetag("ii"))
  //    if(true) {
  //      remoteMouse.x = m.get(0).intValue();
  //      remoteMouse.y = m.get(1).intValue();
  //    }
  //  }
  //}
}

String getBluetoothInformation() {
  String btInfo = "Server Running: ";
  btInfo += bt.isStarted() + "\n";
  btInfo += "Discovering: " + bt.isDiscovering() + "\n";
  btInfo += "Device Discoverable: "+bt.isDiscoverable() + "\n";
  btInfo += "\nConnected Devices: \n";

  ArrayList<String> devices = bt.getConnectedDeviceNames();
  for(String device: devices) {
    btInfo+= device+"\n";
  }
  return btInfo;
}

String getAccelInformation() {
  String accelInfo = "Android Accelerometer sensor:\n";
  accelInfo += "Acceleration in x-axis: " + virtual_accelerometer.xaccel;
  accelInfo += "\nAcceleration in y-axis: " + virtual_accelerometer.yaccel;
  accelInfo += "\nAcceleration in z-axis: " + virtual_accelerometer.zaccel;
  return accelInfo;
}