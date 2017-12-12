#define buzzer_pin 4 //Define buzzer driving pin 

#include <U8glib.h>
#include <Microduino_Motor.h>
#include <Microduino_ColorLED.h> //引用彩灯库

#include <Microduino_Gesture.h>

// Global Variables
Gesture gestureSensor;

#define PIN            D12         //彩灯引脚
#define NUMPIXELS      7        //级联彩灯数量
#define val_max 255
#define val_min 0

ColorLED strip = ColorLED(NUMPIXELS, PIN); //将ColorLED类命名为strip，并定义彩灯数量和彩灯引脚号
ColorLED strip_1 = ColorLED(NUMPIXELS, D13); //将ColorLED类命名为strip，并定义彩灯数量和彩灯引脚号

Motor MotorLeft(MOTOR0_PINA, MOTOR0_PINB);
Motor MotorRight(MOTOR1_PINA, MOTOR1_PINB);
// setup u8g object, please remove comment from one of the following constructor calls
// IMPORTANT NOTE: The complete list of supported devices is here: http://code.google.com/p/u8glib/wiki/device

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);

void draw(String s, int row) {
  // graphic commands to redraw the complete screen should be placed here  
  u8g.setFont(u8g_font_7x13);
  u8g.setPrintPos(0, 20*row); 
  // call procedure from base class, http://arduino.cc/en/Serial/Print
  u8g.print(s);
}
void printToTTF(String s, int row){
  u8g.firstPage();  
  do {
    draw(s, row);
  } while( u8g.nextPage() );
}

//音高
int song[] = {
  262, 262, 294, 262, 349, 330,
  262, 262, 294, 262, 392, 349,
  262, 262, 523, 440, 349, 330, 294,
  466, 466, 440, 349, 392, 349
};

//音长
int noteDurations[] = {
  4, 4, 2, 2, 2, 1,
  4, 4, 2, 2, 2, 1,
  4, 4, 2, 2, 2, 2, 1,
  4, 4, 2, 2, 2, 1
};

void setup_gesture(){
  // Initialize Gesture (configure I2C and initial values)
  if ( gestureSensor.begin() ) {
    Serial.println(F("Gesture initialization complete"));
  } else {
    Serial.println(F("Something went wrong during Gesture init!"));
  }

  if (gestureSensor.setGestureGain(GGAIN_2X)) { //GGAIN_1X GGAIN_2X GGAIN_4X GGAIN_8X
    Serial.println(F("Gesture setGestureGain complete"));
  } else {
    Serial.println(F("Something went wrong during APDS-9960 setGestureGain!"));
  }

  // Start running the Gesture gesture sensor engine
  if ( gestureSensor.enableGestureSensor(true) ) {
    Serial.println(F("Gesture sensor is now running"));
  } else {
    Serial.println(F("Something went wrong during gesture sensor init!"));
  }
  }

void setup() {
  //这个setup函数是内置的，每个程序必须实现这个函数，这个函数在程序运行开始执行一遍，用于初始化各种传感器

  pinMode(buzzer_pin, OUTPUT); //初始化蜂鸣器的输出口

  Serial.begin(9600); //串口初始化，波特率

  MotorLeft.begin();   //电机MotorLeft初始化
  MotorRight.begin();  //电机MotorRight初始化

  strip.begin();   //彩灯初始化
  strip.show(); //灯亮

  strip_1.begin();   //彩灯初始化
  strip_1.show(); 

  setup_gesture(); //手势传感器初始化，关于手势传感器：http://wiki.microduinoinc.com/Sensor-Gesture
}

void turn(int dir){
  //用于左转右转的函数
  int speed_temmp = 50; //转向速度
  MotorLeft.setSpeed(speed_temmp*dir); //设置电机转速 左侧
  MotorRight.setSpeed(speed_temmp*dir); //设置电机转速 右侧
  delay(1000); // 转都多久
  MotorLeft.setSpeed(0); //停下
  MotorRight.setSpeed(0); //停下
  }
void loop() {
  //这个loop函数是内置的，每个程序必须实现这个函数，在执行setup以后，一直循环执行
//  song_play();
//  delay(100);
//  rainbowCycle(20);
//  delay(100);
  int detect = 0; //用于记录是否检测到手势的标志位
  int gesture = 0; //用于记录检测到手势是什么的的标志位
  if ( gestureSensor.isGestureAvailable() ) {
    //判断手势传感器是否有可用数据
    gesture = gestureSensor.readGesture(); //如果可用，读取手势
    switch ( gesture ) {
      case DIR_UP:
        Serial.println("UP");
        detect = 1;
        break;
      case DIR_DOWN:
        Serial.println("DOWN");
        detect = 1;
        break;
      case DIR_LEFT:
        Serial.println("LEFT");
        turn(1);
        break;
      case DIR_RIGHT:
        Serial.println("RIGHT");
        turn(-1);
        break;
      case DIR_NEAR:
        Serial.println("NEAR");
        break;
      case DIR_FAR:
        Serial.println("FAR");
        break;
      default:
        Serial.println("NONE");
    }
  }
  if(detect==1){
    int dir=0;
    if(gesture==DIR_UP){
      dir = 1;
      }
    if(gesture==DIR_DOWN){
      dir = -1;
      }
    song_play(dir);
    }
}
int state =0;

void song_play(int dir)
{
  //奏乐和行走
  for (int thisNote = 0; thisNote < 25; thisNote++)
  {
    //每一个thisNote代表一个音符
    int noteDuration = 1000 / noteDurations[thisNote]; //每个音符的时长

    //显示音长
    String to_print;
    for (int i = 0; i < noteDurations[thisNote]; i++){
      to_print = to_print + "|||";
      }
    printToTTF(to_print, 2);

    //前进/后退
    MotorLeft.setSpeed((noteDurations[thisNote]-1)*20*dir);   //设置电机MotorLeft速度为100
    MotorRight.setSpeed(-(noteDurations[thisNote]-1)*20*dir);  //设置电机MotorRight速度为100
    
    //蜂鸣器
    tone(buzzer_pin, song[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.20;
    delay(pauseBetweenNotes);
    noTone(buzzer_pin);
  }
  MotorLeft.setSpeed(0);   //设置电机MotorLeft速度为100
  MotorRight.setSpeed(0);  //设置电机MotorRight速度为100
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
      strip_1.setPixelColor(i, Wheel_1(((i * 256 / strip_1.numPixels()) + j) & 255));
    }
    strip.show();
    strip_1.show();
    delay(wait);
  }
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel_1(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip_1.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip_1.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip_1.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
