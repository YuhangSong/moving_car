#define buzzer_pin 4 //Define buzzer driving pin 

#include <U8glib.h>
#include <Microduino_Motor.h>
#include <Microduino_ColorLED.h> //引用彩灯库

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
int song[] = {
  262, 262, 294, 262, 349, 330,
  262, 262, 294, 262, 392, 349,
  262, 262, 523, 440, 349, 330, 294,
  466, 466, 440, 349, 392, 349
};

int noteDurations[] = {
  4, 4, 2, 2, 2, 1,
  4, 4, 2, 2, 2, 1,
  4, 4, 2, 2, 2, 2, 1,
  4, 4, 2, 2, 2, 1
};

void setup() {
  pinMode(buzzer_pin, OUTPUT);
  Serial.begin(115200); //串口初始化
  Serial.println("Microduino_Motor!");

  MotorLeft.begin();   //电机MotorLeft初始化
  MotorRight.begin();  //电机MotorLeft初始化

  strip.begin();   //彩灯初始化
  strip.show();
  strip_1.begin();   //彩灯初始化
  strip_1.show();
}

void loop() {
  song_play();
  delay(100);
  rainbowCycle(20);
  delay(100);
}
int state =0;
void song_play()
{
  for (int thisNote = 0; thisNote < 25; thisNote++)
  {
    int noteDuration = 1000 / noteDurations[thisNote];
    String to_print;
    for (int i = 0; i < noteDurations[thisNote]; i++){
      to_print = to_print + "|||";
      }
    printToTTF(to_print, 2);
    MotorLeft.setSpeed((noteDurations[thisNote]-2)*50);   //设置电机MotorLeft速度为100
    MotorRight.setSpeed((noteDurations[thisNote]-2)*50);  //设置电机MotorRight速度为100
    tone(buzzer_pin, song[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.20;
    delay(pauseBetweenNotes);
    noTone(buzzer_pin);
  }
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
