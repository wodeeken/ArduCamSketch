#include <SoftwareSerial.h>
#include <Wire.h>
#include <ArduCAM.h>
#include <SPI.h>
#include "memorysaver.h"
const int CS = 7;
bool is_header = false;
int mode = 0;

const int PinMotionSensor_Dig = 4;
const int ImagesPerCurrentTrigger = 3;
int currentTriggerCount = -1;
// XBEE specific.
SoftwareSerial XBEE_Serial(8,9); // RX, TX

ArduCAM myCAM( OV2640, CS );
uint8_t read_fifo_burst(ArduCAM myCAM);
void setup() {
  // put your setup code here, to run once:
  uint8_t vid, pid;
  uint8_t temp;

  Wire.begin();
  //Serial.begin(921600);
  //Serial.begin(115200);
  // begin XBee.
  // default 9600.
  XBEE_Serial.begin(38400);
  
  pinMode(CS, OUTPUT);
  digitalWrite(CS, HIGH);

  // initialize SPI:
  SPI.begin();

  //Reset the CPLD
  myCAM.write_reg(0x07, 0x80);
  delay(100);
  myCAM.write_reg(0x07, 0x00);
  delay(100);
  while(1){
    //Check if the ArduCAM SPI bus is OK
    myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
    temp = myCAM.read_reg(ARDUCHIP_TEST1);
    if (temp != 0x55){
      Serial.println(F("ACK CMD SPI interface Error! END"));
      XBEE_Serial.write("ACK CMD SPI interface Error! END");
      delay(1000);
      continue;
    }else{
      //Serial.println(F("ACK CMD SPI interface OK. END"));
      break;
    }
  }

  while(1){
    //Check if the camera module type is OV2640
    myCAM.wrSensorReg8_8(0xff, 0x01);
    myCAM.rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
    myCAM.rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);
    if ((vid != 0x26 ) && (( pid != 0x41 ) || ( pid != 0x42 ))){
      Serial.println(F("ACK CMD Can't find OV2640 module! END"));
      XBEE_Serial.println("ACK CMD Can't find OV2640 module! END");
      delay(1000);
      continue;
    }
    else{
      //Serial.println(F("ACK CMD OV2640 detected. END"));
      break;
    } 
  }
  myCAM.set_format(JPEG);
  myCAM.InitCAM();
  //myCAM.OV2640_set_JPEG_size(OV2640_1600x1200);
  //myCAM.OV2640_set_JPEG_size(OV2640_176x144);
  //myCAM.OV2640_set_JPEG_size(OV2640_352x288);
  myCAM.OV2640_set_JPEG_size(OV2640_640x480);
  delay(1000);
  myCAM.clear_fifo_flag();
  // setup motion input.
  pinMode(PinMotionSensor_Dig, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  uint8_t temp = 0xff, temp_last = 0;
  bool is_header = false;
 
  
    // Check motion sensor.
    if(currentTriggerCount < ImagesPerCurrentTrigger && currentTriggerCount != -1){
    
     
      
     
        mode = 1;
        temp = 0xff;
        
        myCAM.flush_fifo();
        myCAM.clear_fifo_flag();
        //Start capture
        delay(250);
        myCAM.start_capture();
        delay(250);
        if (myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK))
        {
          delay(50);
          read_fifo_burst(myCAM);
          //Clear the capture done flag
          myCAM.clear_fifo_flag();
        }
        currentTriggerCount++;  
      
    
    }else if( digitalRead(PinMotionSensor_Dig) == 1){
      currentTriggerCount = 0;
    }else{
      currentTriggerCount = -1;
    }

}
uint8_t read_fifo_burst(ArduCAM myCAM)
{
  uint8_t temp = 0, temp_last = 0;
  uint32_t length = 0;
  length = myCAM.read_fifo_length();
  if (length >= MAX_FIFO_SIZE) //512 kb
  {
    Serial.println(F("ACK CMD Over size. END"));
    XBEE_Serial.println(F("ACK CMD Over size. END"));
    return 0;
  }
  if (length == 0 ) //0 kb
  {
    Serial.println(F("ACK CMD Size is 0. END"));
    XBEE_Serial.println(F("ACK CMD Size is 0. END"));
    return 0;
  }
  myCAM.CS_LOW();
  myCAM.set_fifo_burst();//Set fifo burst mode
  temp =  SPI.transfer(0x00);
  length --;
  while ( length-- )
  {
    temp_last = temp;
    temp =  SPI.transfer(0x00);
    if (is_header == true)
    {
      //Serial.write(temp);
      XBEE_Serial.write(temp);
    }
    else if ((temp == 0xD8) & (temp_last == 0xFF))
    {
      is_header = true;
      //Serial.write(temp_last);
      //Serial.write(temp);
      XBEE_Serial.write(temp_last);
      XBEE_Serial.write(temp);
    }
    if ( (temp == 0xD9) && (temp_last == 0xFF) ) //If find the end ,break while,
    break;
    delayMicroseconds(15);
  }
  myCAM.CS_HIGH();
  is_header = false;
  return 1;
}
