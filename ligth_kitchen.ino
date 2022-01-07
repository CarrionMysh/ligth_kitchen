#define COLOR_DEBTH 1
#include <color_utility.h>
// #include <FastLEDsupport.h>
#include <microLED.h>
// #include <tinyLED.h>

#include <Wire.h>
#include <SparkFun_APDS9960.h>

#define APDS9960_INT    2 // Needs to be an interrupt pin
#define STRIP_PIN 3     // пин ленты
#define NUMLEDS 106     //общее число светодиодов в ленте
#define moika_num 34    //число сетодиодов над мойкой

SparkFun_APDS9960 apds = SparkFun_APDS9960();
int isr_flag = 0;
microLED<NUMLEDS, STRIP_PIN, MLED_NO_CLOCK, LED_WS2818, ORDER_GRB, CLI_AVER> strip;
byte state_strip = 0;
int bri = 255;      //общая яркость
mData led_stat;
unsigned int rnd;
unsigned long time_on, timeout = 1000*60*60*3;  //1000*60*60*3 3 часа
boolean on = false;

void setup() {
  Serial.begin(9600);

  strip.setBrightness(bri);
  strip.clear();
  strip.show();
  delay(1);
  pinMode(APDS9960_INT, INPUT);
  attachInterrupt(0, interruptRoutine, FALLING);

  //debug   поставить свой обработчик ошибок с выводом на ленту
  if ( apds.init() ) {
    apds.setGestureGain(GGAIN_1X);  // GGAIN= _1X _2X _4X _8X
    apds.setGestureLEDDrive(LED_DRIVE_100MA);//LED_DRIVE=_100MA _50MA _25MA _12_5MA

    Serial.println(F("APDS-9960 initialization complete"));
  } else {
    Serial.println(F("Something went wrong during APDS-9960 init!"));
  }

  // Start running the APDS-9960 gesture sensor engine
  if ( apds.enableGestureSensor(true) ) {
    Serial.println(F("Gesture sensor is now running"));
  } else {
    Serial.println(F("Something went wrong during gesture sensor init!"));
  }
  //debug off
}

void loop() {
  if ( isr_flag == 1 ) {
    detachInterrupt(0);
    handleGesture();
    isr_flag = 0;
    attachInterrupt(0, interruptRoutine, FALLING);
  }
  if (on) {
    if (millis()-time_on >timeout) {
      state_strip = 0;
      strip.clear();
      strip.show();
      delayMicroseconds(50);
      on = false;
    }
  }
}

void interruptRoutine() {
  isr_flag = 1;
}

void handleGesture() {
  if ( apds.isGestureAvailable() ) {
    switch ( apds.readGesture() ) {
      case DIR_LEFT_G:
        // Serial.println("LEFT");
        // Serial.println(state_strip);
        time_on = millis();
        on = true;
        switch (state_strip) {
          case 1:
            moika_led_on(moika_num, NUMLEDS);
            state_strip = 2;
            break;
          case 0:
            moika_led_on(0, moika_num);
            state_strip = 1;
            break;
        }
        break;

      case DIR_RIGHT_G:
        // Serial.println("RIGHT");
        byte rnd=random(12);
        on=false;
        switch (state_strip) {
          case 1:
            if (rnd > 8) moika_led_off_1(moika_num);
            if ((rnd > 4) && (rnd <=8) ) moika_led_off_2(moika_num);
            if (rnd <=4) moika_led_off_3(moika_num);
            break;
          case 2:
            if (rnd > 8) moika_led_off_1(NUMLEDS);
            if ((rnd > 4) && (rnd <=8) ) moika_led_off_2(NUMLEDS);
            if (rnd <=4) moika_led_off_3(NUMLEDS);
            break;
        }

        state_strip = 0;
        break;
      default:
        // Serial.println("NONE");
        break;
    }
  }
}

void moika_led_on(unsigned int num_start, unsigned int num_end) {
  byte temp_bri,tt;
  // byte R,G,B;
  for (unsigned int i = num_start; i < num_end; i++) {
    // strip.leds[i] = mWhite;
    temp_bri = map(i,num_start,num_end, 1, bri);
    // Serial.print("temp_bri");Serial.println(temp_bri);
    switch (state_strip) {
      case (0):
        strip.leds[i] = mWhite;
        strip.setBrightness(temp_bri);
        strip.show();
        delay(5);
        break;
      case (1):
        for (unsigned int bb = num_start; bb <= i; bb++){
          tt = map(temp_bri,0,bri,0,255);
          strip.leds[bb] = mRGB(tt,tt,tt);
        }
        strip.show();
        delay(1);
        break;
    }
  }
  strip.setBrightness(bri);
  strip.show();
}

void moika_led_off_1(unsigned int mm) {
  unsigned int nn;
  for (unsigned int i = 0; i < mm; i++) {
    while (true) {
      nn = random(mm);
      led_stat = strip.get(nn);
      if (led_stat != 0) {
        strip.leds[nn] = 0;
        strip.show();
        delay(1);
        break;
      }
    }
  }
  return;
}

void moika_led_off_2 (unsigned int mm){
  for (int i = mm-1; i>=0; i--){
    // Serial.println(gg);
    strip.leds[i] = 0;
    strip.show();
    delayMicroseconds(50);
  }
  strip.clear();
  strip.show();
  delayMicroseconds(50);
}

void moika_led_off_3 (unsigned int mm){
  strip.clear();
  strip.show();
  delay(15);
  strip.fill(0,mm-1, mWhite);
  strip.show();
  delay(100);
  strip.clear();
  strip.show();
  delay(25);
  strip.fill(0,mm-1, mWhite);
  strip.show();
  delay(200);
  strip.clear();
  strip.show();
  delay(10);
  strip.fill(0,mm-1, mWhite);
  strip.show();
  delay(100);
  for(int  i=bri; i>=(bri/100*2); i=i-2){
    strip.setBrightness(i);
    strip.show();
    delayMicroseconds(100);
  }
  for (int  i=(bri/100*20); i<=(bri/100*80); i=i+5){
    strip.setBrightness(i);
    strip.show();
    delayMicroseconds(50);
  }
  strip.setBrightness(bri);
  strip.show();
  delay(20);
  strip.clear();
  strip.show();
  delay(1);
}
