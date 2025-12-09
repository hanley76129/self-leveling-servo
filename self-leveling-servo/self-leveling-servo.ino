//mpu register : https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf
#include <Wire.h>   //Wire Library
#include <Servo.h>  //Servo Library

Servo servo;  //create object for the Servo Library
const int MPU_addr = 0x68;  //default I2C address of the MPU is 0x68
int start_angle = 90;       // default position of the servo
int user_offset;            // stores what offset the user wants
float X_out, Y_out, Z_out;  //variables set up for the info I take out of out of the mpu
float roll, pitch;          // variable for roll and pitch


void setup() {
  //Begins the I2C communication
  Serial.begin(9600);                 // opens serial port and sets data rate (baud) to 9600
  Serial.println("Initial Offset ");  // prints text to the serial port
  while (Serial.available() == 0);   // sees the number of bytes available to read, waiting for input in the serial port
  Serial.setTimeout(10);  // sets the wait time between each input in the serial port to be 10 miliseconds
  Wire.begin();// initializes wire library

  //my code breaks sometimes and for some reason adding an empty random register fixes it 
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission(true);
  delay(50);

  Wire.beginTransmission(MPU_addr);  // starts transmission with the mpu6050

  Wire.write(0x1B);  // GYRO_CONFIG register located here, write: tells which register we are writing to, pg 14 on mpu register
  Wire.write(0x10);  // using bit 4, sets full scale range of gyroscope (bit 4 in binary is 10)

  Wire.endTransmission();  // stops transmission with the mpu6050

  //intializes the servo
  servo.attach(3);           // using d3 for servo
  servo.write(start_angle);  // intialize servo angle
  delay(10);
}

void loop() {
  //starts reading the registers we need
  Wire.beginTransmission(MPU_addr);  //starts transmission with the mpu
  Wire.write(0x3B);  // to start, reads register ACCEL_XOUT_H
  Wire.endTransmission(false);  // restarts transmission, doesn't end it
  Wire.requestFrom(MPU_addr, 4 , true);  //reads ACCEL_XOUT_H, ACCEL_XOUT_H, ACCEL_YOUT_H, ACCEL_YOUT_H(true ends the transmission after finished)

  X_out = (Wire.read() << 8 | Wire.read()); // this makes it so that it reads 8 bits because i2c reads 8 bits at a time 
  X_out = X_out / 16384;  // the value is divided by 16384 because thats the LSB Sensitivity on page 29 of the register 
  Y_out = (Wire.read() << 8 | Wire.read());
  Y_out = Y_out / 16384;
  
  //in the default position I want, the x_out value is -1, and y_out is 0
  X_out+=1; // 

  //Change X_out and Y_out to angles
  X_out=X_out*100;
  Y_out=Y_out*100;

  //prints on the serial monitor
  Serial.print("Y = ");
  Serial.print(Y_out);
  Serial.println(" degrees");
  Serial.print("X = ");  
  Serial.print(X_out);
  Serial.println(" degrees");
  
  //checking for serial input, saves the  user input as the offset 
  if (Serial.available() > 0) {
    user_offset = Serial.readString().toInt();
  }
  Serial.print("Offset: ");
  Serial.println(user_offset);

  servo.write(start_angle + user_offset - Y_out); //servo always pointing up
  delay(100);
}