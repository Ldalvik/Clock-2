#include <SerialBufferRK.h>
#include <neopixel.h>

SYSTEM_THREAD(ENABLED);

#define PIXEL_PIN D6
#define PIXEL_COUNT 330
#define PIXEL_TYPE WS2812B
#define ON 1
#define OFF 0
Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

int ALL_HOURS[14][2] = {
  {0, 0},      //
  {138, 150},  //one
  {164, 175},  //two
  {122, 137},  //three
  {176, 189},  //four
  {192, 205},  //five
  {239, 247},  //six
  {221, 238},  //seven
  {206, 220},  //eight
  {248, 258},  //nine
  {262, 271},  //ten
  {272, 288},  //eleven
  {309, 329},  //twelve
  {138, 150}   //one (for past half an hour)
};

int ALL_MINUTES[12][2] = {
  {0, 0},      //
  {80, 90},    //five
  {30, 40},    //ten
  {58, 78},    //quarter
  {41, 57},    //twenty
  {41, 57},    //twenty (five)
  {15, 29},    //half
  {41, 57},    //twenty (five)
  {41, 57},    //twenty
  {58, 78},    //quarter
  {30, 40},    //ten
  {80, 90}     //five
};

int IT[2] = {1, 7};
int IS[2] = {8, 14};
int CURRENT_MINUTE[2] = {0, 0};
int FIVE[2] = {0, 0};
int MINUTES[2] = {0, 0};
int TO_PAST[2] = {0, 0};
int CURRENT_HOUR[2] = {0, 0};
int OCLOCK[2] = {289, 308};

int r = 255;
int g = 0;
int b = 0;

int start_red = 255;
int start_green = 0;
int start_blue = 0;
int end_red = 0;
int end_green = 255;
int end_blue = 0;

int hour;
int minute;

String content = "";
String readString;
SerialBuffer<4096> serBuf(Serial1);

String mode = "solid";
int wait = 20;

void threadFunction(void *param);
Thread thread("testThread", threadFunction);

volatile int counter = 0;
unsigned long lastReport = 0;
system_tick_t lastThreadTime = 0;
void setup() {
  Serial1.begin(115200);
  strip.begin();
  Time.zone(-4);
  resetNeo();
  setTime();
}

void loop() {
  while (serBuf.available()) {
    content = serBuf.readStringUntil('/');
    parseCommand(content);
    //Particle.publish("received", content);
    content = "";
  }
}

void parseCommand(String command) {
  switch (command.charAt(0)) {
    case 'r': {
        setRed(command);
        //Particle.publish("r", String(command));
      } break;
    case 'g': {
        setGreen(command);
        //Particle.publish("g", String(command));
      } break;
    case 'b': {
        setBlue(command);
        //Particle.publish("b", String(command));
      } break;
    case 's': {
        setStartGradient(command);
        //Particle.publish("start", String(command));
      } break;
    case 'e': {
        setEndGradient(command);
        //Particle.publish("end", String(command));
      } break;
    case 'm': {
        setMode(command);
        Particle.publish("end", String(command));
      } break;
    case 'z': {
        resetNeo();
      } break;
  }
}

void resetNeo() {
  for (int i = 0; i < PIXEL_COUNT; i++) {
    strip.setPixelColor(i, 0, 0, 0);
  }
  strip.show();
}

void setTime() {
  hour = Time.hourFormat12(); //Set current hour
  minute = Time.minute() / 5; //Set current minute

  /* LOGIC FOR WORD "MINUTES" */
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (minute == 0 || minute == 3 || minute == 6 || minute == 9) { //If minute is 0, 15, 30, or 45           //
    SET_MINUTES(OFF);                                             //Turn off word "MINUTES"                 //
  } else {                                                        //If minute equals 1, 2, 4, 5, 7, or 8    //
    SET_MINUTES(ON);                                              //Turn on word "MINUTES"                  //
  }                                                                                                         //
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////

  /* LOGIC FOR CURRENT HOUR + WORDS "TO" AND "PAST" */
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (minute == 0) {                                              //If minute is zero                       //
    SET_TO(OFF);                                                  //Turn off word "TO"                      //
  } else if (minute <= 6) {                                       //If minute is less than or equal to 30   //
    SET_PAST(ON);                                                 //Turn on word "PAST"                     //
    SET_HOUR(hour);                                               //Set the current hour                    //
  } else if (minute > 6) {                                        //If minute is more than 30               //
    SET_TO(ON);                                                   //Turn on word "TO"                       //
    SET_HOUR(hour + 1);                                           //Set one hour ahead for "TO" logic       //
  }                                                                                                         //
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////

  /* LOGIC FOR CURRENT HOUR + WORDS "TO" AND "PAST" */
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (minute == 5 || minute == 7) {                               //If minute is 25 or 35, turn on "FIVE"   //
    SET_MINUTE(minute);                                           //Set current minute                      //
    SET_FIVE(ON);                                                 //Turn on word "FIVE"                     //
  } else {                                                        //If minute doesn't equal 25 or 35, set   //
    SET_MINUTE(minute);                                           //Set current minute                      //
    SET_FIVE(OFF);                                                //Turn off word "FIVE"                    //
  }                                                                                                         //
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////

  /* LOGIC FOR COLORS AND PATTERNS */
  if (mode == "gradient") {
    modeGradient();
  }

  if (mode == "rainbow") {
    rainbowCycle();
  }

  if (mode == "solid") {
    solid();
  }
}

void threadFunction(void *param) {
	while(true) {
	    //setTime();
		os_thread_delay_until(&lastThreadTime, 1000*5);
	}
	// You must not return from the thread function
}

/* FUNCTIONS TO SET WORDS ON/OFF */
void SET_MINUTES(int state) {
  if (state) {
    MINUTES[0] = 92;
    MINUTES[1] = 113;
  } else {
    MINUTES[0] = 0;
    MINUTES[1] = 0;
  }
}

void SET_TO(int state) {
  if (state) {
    TO_PAST[0] = 114;
    TO_PAST[1] = 121;
  } else {
    TO_PAST[0] = 0;
    TO_PAST[1] = 0;
  }
}

void SET_PAST(int state) {
  if (state) {
    TO_PAST[0] = 151;
    TO_PAST[1] = 162;
  } else {
    TO_PAST[0] = 0;
    TO_PAST[1] = 0;
  }
}
void SET_FIVE(int state) {
  if (state) {
    FIVE[0] = 80;
    FIVE[1] = 90;
  } else {
    FIVE[0] = 0;
    FIVE[1] = 0;
  }
}

void SET_MINUTE(int m) {
  CURRENT_MINUTE[0] = ALL_MINUTES[m][0];
  CURRENT_MINUTE[1] = ALL_MINUTES[m][1];
}

void SET_HOUR(int h) {
  CURRENT_HOUR[0] = ALL_HOURS[h][0];
  CURRENT_HOUR[1] = ALL_HOURS[h][1];
}

/* FUNCTIONS TO SET SOLID COLORS */
void setRed(String command) {
  command.remove(0, 2);
  r = command.toInt();
  setTime();
}

void setGreen(String command) {
  command.remove(0, 2);
  g = command.toInt();
  setTime();
}

void setBlue(String command) {
  command.remove(0, 2);
  b = command.toInt();
  setTime();
}

/* FUNCTIONS TO SET GRADIENT START/END COLORS */
void setStartGradient(String command) {
  command.remove(0, 2);
  start_red = getValue(command, ',', 0).toInt();
  start_green = getValue(command, ',', 1).toInt();
  start_blue = getValue(command, ',', 2).toInt();
  setTime();
}

void setEndGradient(String command) {
  command.remove(0, 2);
  end_red = getValue(command, ',', 0).toInt();
  end_green = getValue(command, ',', 1).toInt();
  end_blue = getValue(command, ',', 2).toInt();
  setTime();
}

/* FUNCTIONS TO SET PATTERN/COLOR MODE */
void setMode(String command) {
  command.remove(0, 2);
  mode = command;
}

void modeGradient() {
  for (int led = 0; led < PIXEL_COUNT; led++) {
    int newRed;
    int newGreen;
    int newBlue;

    if (start_red != end_red) {
      if (start_red < end_red) {
        newRed = round(start_red + (led * 0.77));
      } else {
        newRed = round(start_red - (led * 0.77));
      }
    } else {
      newRed = 0;
    }
    if (start_green != end_green) {
      if (start_green < end_green) {
        newGreen = round(start_green + (led * 0.77));
      } else {
        newGreen = round(start_green - (led * 0.77));
      }
    } else {
      newGreen = 0;
    }
    if (start_blue != end_blue) {
      if (start_blue < end_blue) {
        newBlue = round(start_blue + (led * 0.77));
      } else {
        newBlue = round(start_blue - (led * 0.77));
      }
    } else {
      newBlue = 0;
    }
    show(led, newRed, newGreen, newBlue);
  }
  strip.show();
}

void solid() {
    for (int led = 0; led < PIXEL_COUNT; led++) {
        show(led, r, g, b);
    }
    strip.show();
}

void show(int led, int r, int g, int b) {
  if ( led >= IT[0] && led <= IT[1] ||                          //IT
       led >= IS[0] && led <= IS[1] ||                          //IS
       led >= CURRENT_MINUTE[0] && led <= CURRENT_MINUTE[1] ||  //FIVE/TEN/QUARTER/TWENTY/HALF
       led >= FIVE[0] && led <= FIVE[1] ||                      //FIVE
       led >= MINUTES[0] && led <= MINUTES[1] ||                //MINUTES
       led >= TO_PAST[0] && led <= TO_PAST[1] ||                //TO/PAST
       led >= CURRENT_HOUR[0] && led <= CURRENT_HOUR[1] ||      //ONE/TWO/THREE/FOUR/FIVE/SIX/SEVEN/EIGHT/NINE/TEN/ELEVEN/TWELVE
       led >= OCLOCK[0] && led <= OCLOCK[1]) {                  //OCLOCK
    strip.setPixelColor(led, r, g, b);
  } else {
    strip.setPixelColor(led, 0, 0, 0);
  }
}

void rainbow() {
  uint16_t led, j;
  for(j=0; j<256; j++) {
    for(led=0; led < PIXEL_COUNT; led++) {
        if ( led >= IT[0] && led <= IT[1] ||                         //IT
            led >= IS[0] && led <= IS[1] ||                          //IS
            led >= CURRENT_MINUTE[0] && led <= CURRENT_MINUTE[1] ||  //FIVE/TEN/QUARTER/TWENTY/HALF
            led >= FIVE[0] && led <= FIVE[1] ||                      //FIVE
            led >= MINUTES[0] && led <= MINUTES[1] ||                //MINUTES
            led >= TO_PAST[0] && led <= TO_PAST[1] ||                //TO/PAST
            led >= CURRENT_HOUR[0] && led <= CURRENT_HOUR[1] ||      //ONE/TWO/THREE/FOUR/FIVE/SIX/SEVEN/EIGHT/NINE/TEN/ELEVEN/TWELVE
            led >= OCLOCK[0] && led <= OCLOCK[1]) {                  //OCLOCK
                strip.setPixelColor(led, Wheel((led+j) & 255));
        } else {
                strip.setPixelColor(led, 0, 0, 0);
        }
    }
    delay(wait);
    strip.show();
  }
}

void rainbowCycle() {
  uint16_t led, j;
  for(j=0; j<256; j++) { // 5 cycles of all colors on wheel
    for(led=0; led < PIXEL_COUNT; led++) {
        if ( led >= IT[0] && led <= IT[1] ||                         //IT
            led >= IS[0] && led <= IS[1] ||                          //IS
            led >= CURRENT_MINUTE[0] && led <= CURRENT_MINUTE[1] ||  //FIVE/TEN/QUARTER/TWENTY/HALF
            led >= FIVE[0] && led <= FIVE[1] ||                      //FIVE
            led >= MINUTES[0] && led <= MINUTES[1] ||                //MINUTES
            led >= TO_PAST[0] && led <= TO_PAST[1] ||                //TO/PAST
            led >= CURRENT_HOUR[0] && led <= CURRENT_HOUR[1] ||      //ONE/TWO/THREE/FOUR/FIVE/SIX/SEVEN/EIGHT/NINE/TEN/ELEVEN/TWELVE
            led >= OCLOCK[0] && led <= OCLOCK[1]) {                  //OCLOCK
                strip.setPixelColor(led, Wheel((led+j) & 255));
        } else {
                strip.setPixelColor(led, 0, 0, 0);
        }
    }
    //delay(wait);
    strip.show();
  }
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

/* OTHER */
String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
