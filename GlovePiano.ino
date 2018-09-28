//2017 stuff

#include "mpu9250.c"
#include <Wire.h> //I2C library
#include <SFE_MicroOLED.h>


/************************************************************
 * Arduino hardware readings
*************************************************************/
//Direct readings from IMU MPU9250
signed int roll = 0;
signed int pitch = 0;
signed int yaw = 0;
//smoothing values by averaging
int averageArrayIndex = 0;
int averageArrayIndexSize = 10;
signed int rollArr[] = {0,0,0,0,0,0,0,0,0,0};
signed int pitchArr[] = {0,0,0,0,0,0,0,0,0,0};
signed int rollAverage = 0;
signed int pitchAverage = 0;





/************************************************************
 * MPU9250 data storage
*************************************************************/
byte accelData[6];
byte tempData[2];
byte gyroData[6];
byte magnetoData[6];


/************************************************************
 * MicroOLED Definition
*************************************************************/
#define PIN_RESET 9  // Connect RST to pin 9
#define DC_JUMPER 1
MicroOLED oled(PIN_RESET, DC_JUMPER);    // I2C declaration


/************************************************************
 * Music stuff
*************************************************************/

/*
 * 1. The key is ready to be played
 *                    TRUE    FALSE
 * HAMMER_READY     |   X
 * HAMMER_RETURNING |           X
 * 
 * 
 * 
 * 2. The key is just played
 *                    TRUE    FALSE
 * HAMMER_READY     |           X
 * HAMMER_RETURNING |           X
 * 
 * 
 * 
 * 3. The key is just released
 *                    TRUE    FALSE
 * HAMMER_READY     |           X
 * HAMMER_RETURNING |   X
*/

boolean HAMMER_READY = true;
boolean HAMMER_RETURNING = false;
double PREV_HAMMER_STRIKE = 0;
double PREV_HAMMER_RELEASE = 0;
double HAMMER_RAPIDITY = 100; //time in milliseconds it takes to ready hammer upon release



/************************************************************
 * Bluetooth
*************************************************************/
//BTA=000666DC8D77
//BTName=GloveDuino
//Baudrt(SW4)=9600
//Mode  =Slav
//Authen=0
//PinCod=1234
//Bonded=0
//Rem=000666DC8D88
boolean TESTING_LED_ON = true;


/*----------------------------------------------------------
 * Setup
------------------------------------------------------------*/

void setup(){

  
  /*********
   * LED to tell powered
  ***********/
  pinMode(13,OUTPUT);
  digitalWrite(13, HIGH);

  /*********
   * Fire up OLED
  ***********/
//  oled.begin();    // Initialize the OLED
//  oled.setFontType(0);  // Set font to type 1
//  oled.clear(ALL); // Clear the display's internal memory
////  oled.display();  // Display what's in the buffer (splashscreen)
////  delay(1000);     // Delay 1000 ms
//  oled.clear(PAGE); // Clear the buffer.
  
  /*********
   * Begin communication streams
  ***********/
  Wire.begin(); // Begin I2C for mpu9250
  Serial.begin(115200);  // bluesmirf always starts up at 115200 by default
  Serial.print("$");  // Print three times individually
  Serial.print("$");
  Serial.print("$");  // Enter command mode
  delay(100);
  Serial.println("U,9600,N");  // Temporarily Change the baudrate to 9600, no parity
  Serial.flush();
  Serial.begin(9600);
  Serial.println("Started");


  

  /*********
   * MPU setup
  ***********/
  //INIT MPU
  // wake up device
  // Clear sleep mode bit (6), enable all sensors
  writeByte(MPU9250_ADDRESS, PWR_MGMT_1, 0x00);
  delay(100); // Wait for all registers to reset
  writeByte(MPU9250_ADDRESS, INT_ENABLE, 0x01);
  // Configure Interrupts and Bypass Enable
  // Set interrupt pin active high, push-pull, hold interrupt pin level HIGH
  // until interrupt cleared, clear on read of INT_STATUS, and enable
  // I2C_BYPASS_EN so additional chips can join the I2C bus and all can be
  // controlled by the Arduino as master.
  writeByte(MPU9250_ADDRESS, INT_PIN_CFG, 0x22);
  // Enable data ready (bit 0) interrupt
  writeByte(MPU9250_ADDRESS, INT_ENABLE, 0x01);
  // Configure gyro sensitivity to full range
  writeByte(MPU9250_ADDRESS, GYRO_CONFIG, 0x11);
  delay(100);
} 


/*----------------------------------------------------------
 * Loop
------------------------------------------------------------*/

void loop() {

  //Get user input from mobile. input results in led ligth changing
  if(Serial.available()) { // If stuff was typed in the serial monitor
    while(Serial.available()) {
      Serial.read();
    }
    toggleTestingLed();
  }

  /*********
   * Read MPU9250
  ***********/

  /*READ ACCELEROMETER*/
  Wire.beginTransmission(SENSOR_I2CADD); //Begin transmission to slave address
  Wire.write(0x3B); //Register address within the sensor where the data is to be read from
  Wire.endTransmission();
  Wire.requestFrom(SENSOR_I2CADD, 6); //Get 6 bytes from the register address 
  int i = 0;
  while(Wire.available()) //If the buffer has data
  {
    accelData[i] = Wire.read(); //Save the data to a variable
    i++;
  }
  int accelX = accelData[1] | (int)accelData[0] << 8;
  int accelY = accelData[3] | (int)accelData[2] << 8;
  int accelZ = accelData[5] | (int)accelData[4] << 8;


  /*READ TEMPERATURE*/
  Wire.beginTransmission(SENSOR_I2CADD); //Begin transmission to slave address
  Wire.write(0x41); //Register address within the sensor where the data is to be read from
  Wire.endTransmission();
  Wire.requestFrom(SENSOR_I2CADD, 2); //Get 2 bytes from the register address 
  i = 0;
  while(Wire.available()) //If the buffer has data
  {
    tempData[i] = Wire.read(); //Save the data to a variable
    i++;
  }
  int temp = tempData[1] | (int)tempData[0] << 8;

  /*READ GYRO*/
  Wire.beginTransmission(MPU9250_ADDRESS); //Begin transmission to slave address
  Wire.write(0x43); //Register address within the sensor where the data is to be read from
  Wire.endTransmission();
  Wire.requestFrom(SENSOR_I2CADD, 6); //Get 6 bytes from the register address 
  i = 0;
  while(Wire.available()) //If the buffer has data
  {
    gyroData[i] = Wire.read(); //Save the data to a variable
    i++;
  }
  int gyroX = gyroData[1] | (int)gyroData[0] << 8;
  int gyroY = gyroData[3] | (int)gyroData[2] << 8;
  int gyroZ = gyroData[5] | (int)gyroData[4] << 8;
  
  /*READ MAGNETOMETRI*/
  //turning magnetometer off and on is a workaround that makes output value update
  writeByte(AK8963_ADDRESS, AK8963_CNTL, 0x00); // Power down magnetometer
  delay(10);
  writeByte(AK8963_ADDRESS, AK8963_CNTL, 0x16);
  delay(10);
  Wire.beginTransmission(AK8963_ADDRESS);
  Wire.write(AK8963_XOUT_L);
  Wire.endTransmission();
  
  byte rawData[6];
  i = 0;
  // Read bytes from slave register address
  Wire.requestFrom(AK8963_ADDRESS, 6);
  while (Wire.available())
  {
    // Put read results in the Rx buffer
    rawData[i++] = Wire.read();
  }

  int magnetoX = ((int16_t)rawData[1] << 8) | rawData[0];
  int magnetoY = ((int16_t)rawData[3] << 8) | rawData[2];
  int magnetoZ = ((int16_t)rawData[5] << 8) | rawData[4];




  
  /*********
   * Store data to values
  ***********/

  /****
   * mapping
  ******/

  //accelerometer stuff
  float x = accelX * 16.0f / 32768.0f;
  float y = accelY * 16.0f / 32768.0f;
  float z = accelZ * 16.0f / 32768.0f;

  //gyroscope stuff
  double x_rot = gyroX * 100.0 / 32768.0;
  double y_rot = gyroY * 100.0 / 32768.0;
  double z_rot = gyroZ * 100.0 / 32768.0;
  

  //magnetometer stuff
  float magx = magnetoX * 10.0f * 4912.0f / 32760.0f;
  float magy = magnetoY * 10.0f * 4912.0f / 32760.0f;
  

  /****
   * store to variables
  ******/
  roll = floor(degrees(atan2(-x, sqrt(y * y + z * z))));
  pitch = floor(degrees(atan2(y,z)));
  yaw = floor(degrees(atan2(magy,magx))); 


  /****
   * Actuate
  ******/

  double NOW = millis();

  //finger is removed from the key, release begins
  if(pitch < 54) { //sign depends on physical orientation of accelmeter. Try irl to get right
    if(!HAMMER_READY){
      if(!HAMMER_RETURNING) { 
        HAMMER_RETURNING = true;
        PREV_HAMMER_RELEASE = NOW;
//        Serial.println("SUCCESS Release: finger released from key");
      } else {
      }
    } else {
    }
  }

  //hammer was succesfully returned, enabling another strike
  if(HAMMER_RETURNING) {
    if(NOW - PREV_HAMMER_RELEASE > HAMMER_RAPIDITY) {
      HAMMER_RETURNING = false;
      HAMMER_READY = true;
//      Serial.println("SUCCESS Return: hammer readied");
    }
  }
 
  //hammer is stricken
  if(pitch > 64) {
    if(HAMMER_READY) {
      if(gyroX > 0) { //sign depends on physical oriantation of accelmeter. Try irl to get right
        HAMMER_READY = false;
        PREV_HAMMER_STRIKE = NOW;
        String res = (String)x_rot;
        
        Serial.println(res);
      } else {
      }
    } else {
      PREV_HAMMER_RELEASE = NOW;
    }
  }
  
  
  delay(15);

  
//  oled.clear(PAGE);     // Clear the page
//  oled.setCursor(0, 0); // Set cursor to top-left
//  // Print can be used to print a string to the screen:
//  oled.print(res);
//  oled.display();       // Refresh the display
//  delay(50);          // Delay a second and repeat
}











/************************************************************
 * Useful functions
*************************************************************/

void toggleTestingLed() {
  if(TESTING_LED_ON) {
    TESTING_LED_ON = false;
    digitalWrite(13, LOW);
  } else {
    TESTING_LED_ON = true;
    digitalWrite(13, HIGH);
  }
}

/*********
 * Instrument
***********/

void hammer_strike() {
  
}

void hammer_release() {
  
}

void hammer_ready() {
  
}

/*********
 * MPU9250 necessities
***********/

uint8_t writeByteWire(uint8_t deviceAddress, uint8_t registerAddress, uint8_t data) {
  Wire.beginTransmission(deviceAddress);  // Initialize the Tx buffer
  Wire.write(registerAddress);      // Put slave register address in Tx buffer
  Wire.write(data);                 // Put data in Tx buffer
  Wire.endTransmission();           // Send the Tx buffer
  // TODO: Fix this to return something meaningful
  return NULL;
}

uint8_t writeByte(uint8_t deviceAddress, uint8_t registerAddress, uint8_t data) {
    return writeByteWire(deviceAddress,registerAddress, data);
}

uint8_t readByte(uint8_t deviceAddress, uint8_t registerAddress) {
    return readByteWire(deviceAddress, registerAddress);
}

// Read a byte from the given register address from device using I2C
uint8_t readByteWire(uint8_t deviceAddress, uint8_t registerAddress) {
  uint8_t data; // `data` will store the register data

  // Initialize the Tx buffer
  Wire.beginTransmission(deviceAddress);
  // Put slave register address in Tx buffer
  Wire.write(registerAddress);
  // Send the Tx buffer, but send a restart to keep connection alive
  Wire.endTransmission(false);
  // Read one byte from slave register address
  Wire.requestFrom(deviceAddress, (uint8_t) 1);
  // Fill Rx buffer with result
  data = Wire.read();
  // Return data read from slave register
  return data;
}

// Read 1 or more bytes from given register and device using I2C
uint8_t readBytesWire(uint8_t deviceAddress, uint8_t registerAddress, uint8_t count, uint8_t * dest) {
  // Initialize the Tx buffer
  Wire.beginTransmission(deviceAddress);
  // Put slave register address in Tx buffer
  Wire.write(registerAddress);
  // Send the Tx buffer, but send a restart to keep connection alive
  Wire.endTransmission(false);
  uint8_t i = 0;
  // Read bytes from slave register address
  Wire.requestFrom(deviceAddress, count);
  while (Wire.available()) {
    // Put read results in the Rx buffer
    dest[i++] = Wire.read();
  }
  return i; // Return number of bytes written
}

uint8_t readBytes(uint8_t deviceAddress, uint8_t registerAddress, uint8_t count, uint8_t * dest) {
    return readBytesWire(deviceAddress, registerAddress, count, dest);
 }






 
/*----------------------------------------------------------
 * END
------------------------------------------------------------*/
