#include <Adafruit_NeoPixel.h>
int h = 9;
int m = 30;
#define PIXEL_PIN 10
#define PIXEL_COUNT 330
#define PIXEL_TYPE WS2812B
#define ON 1
#define OFF 0
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);
#include <Arduino.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"
#include "BluefruitConfig.h"

#if SOFTWARE_SERIAL_AVAILABLE
#include <SoftwareSerial.h>
#endif

String content;

#define FACTORYRESET_ENABLE         1
#define MINIMUM_FIRMWARE_VERSION    "0.6.6"
#define MODE_LED_BEHAVIOUR          "MODE"

Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

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
  {30, 38},    //ten
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

int start_red = 0;
int start_green = 0;
int start_blue = 0;
int end_red = 0;
int end_green = 0;
int end_blue = 0;


String readString;
bool isPatternComplete = false;
String mode = "solid";
int wait = 20;

void setup() {
  Serial.begin(115200);
  strip.begin();
  resetNeo();

  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  Serial.println( F("OK!") );

  if ( FACTORYRESET_ENABLE )
  {
    Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() ) {
      error(F("Couldn't factory reset"));
    }
  }

  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  ble.info();

  Serial.println(F("Please use Adafruit Bluefruit LE app to connect in UART mode"));
  Serial.println(F("Then Enter characters to send to Bluefruit"));
  Serial.println();

  ble.verbose(false);

  while (! ble.isConnected()) {
    delay(500);
  }

  Serial.println(F("******************************"));

  if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) )
  {
    Serial.println(F("Change LED activity to " MODE_LED_BEHAVIOUR));
    ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
  }

  Serial.println(F("Switching to DATA mode!") );
  ble.setMode(BLUEFRUIT_MODE_DATA);

  Serial.println(F("******************************"));
}

void loop() {
  char n, inputs[BUFSIZE + 1];
  if (Serial.available()) {
    n = Serial.readBytes(inputs, BUFSIZE);
    inputs[n] = 0;
    Serial.println(inputs);
    ble.print(inputs);
  }

  while (ble.available()) {
    char c = (char)ble.read();
    if (c == '/') {
      check(content);
      content = "";
    } else {
      content.concat(c);
    }
  }
  /*if (mode == "gradient") {
    modeGradient();
    }

    if (mode == "rainbow") {
    rainbow();
    }

    if (mode == "solid") {
    solid();
    }

    if (mode == "gradient2") {
    }

    if (mode == "fade") {
    solidFade();
    }*/
}

void check(String data) {
  Serial.println(data);
  colorToggle(data);
  _setTime();
}

int colorToggle(String command) {
  switch (command.charAt(0)) {
    case 'r': {
        setRed(command);
      } break;
    case 'g': {
        setGreen(command);
      } break;
    case 'b': {
        setBlue(command);
      } break;
    case 's': {
        setStartGradient(command);
      } break;
    case 'e': {
        setEndGradient(command);
      } break;
    case 'm': {
        _setMode(command);
      } break;
    case 'z': {
        resetNeo();
      } break;
  }
  if (mode == "gradient") {
    modeGradient();
  }

  if (mode == "rainbow") {
    rainbow();
  }

  if (mode == "solid") {
    solid();
  }

  if (mode == "gradient2") {
  }

  if (mode == "fade") {
    solidFade();
  }
}

void _setTime() {
  /* LOGIC FOR WORD "MINUTES" */
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (m == 0 || m == 3 || m == 6 || m == 9) { //If minute is 0, 15, 30, or 45           //
    SET_MINUTES(OFF);                                             //Turn off word "MINUTES"                 //
  } else {                                                        //If minute equals 1, 2, 4, 5, 7, or 8    //
    SET_MINUTES(ON);                                              //Turn on word "MINUTES"                  //
  }                                                                                                         //
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////

  /* LOGIC FOR CURRENT HOUR + WORDS "TO" AND "PAST" */
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (m == 0) {                                              //If minute is zero                       //
    SET_TO(OFF);                                                  //Turn off word "TO"                      //
  } else if (m <= 6) {                                       //If minute is less than or equal to 30   //
    SET_PAST(ON);                                                 //Turn on word "PAST"                     //
    SET_HOUR(h);                                               //Set the current hour                    //
  } else if (m > 6) {                                        //If minute is more than 30               //
    SET_TO(ON);                                                   //Turn on word "TO"                       //
    SET_HOUR(h + 1);                                           //Set one hour ahead for "TO" logic       //
  }                                                                                                         //
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////

  /* LOGIC FOR CURRENT HOUR + WORDS "TO" AND "PAST" */
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (m == 5 || m == 7) {                               //If minute is 25 or 35, turn on "FIVE"   //
    SET_MINUTE(m);                                           //Set current minute                      //
    SET_FIVE(ON);                                                 //Turn on word "FIVE"                     //
  } else {                                                        //If minute doesn't equal 25 or 35, set   //
    SET_MINUTE(m);                                           //Set current minute                      //
    SET_FIVE(OFF);                                                //Turn off word "FIVE"                    //
  }                                                                                                         //
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

void show(int led, int red, int green, int blue) {
  if (led >= IT[0] && led <= IT[1] ||                          //IT
      led >= IS[0] && led <= IS[1] ||                          //IS
      led >= CURRENT_MINUTE[0] && led <= CURRENT_MINUTE[1] ||  //FIVE/TEN/QUARTER/TWENTY/HALF
      led >= FIVE[0] && led <= FIVE[1] ||                      //FIVE
      led >= MINUTES[0] && led <= MINUTES[1] ||                //MINUTES
      led >= TO_PAST[0] && led <= TO_PAST[1] ||                //TO/PAST
      led >= CURRENT_HOUR[0] && led <= CURRENT_HOUR[1] ||      //ONE/TWO/THREE/FOUR/FIVE/SIX/SEVEN/EIGHT/NINE/TEN/ELEVEN/TWELVE
      led >= OCLOCK[0] && led <= OCLOCK[1]) {                  //OCLOCK
    strip.setPixelColor(led, red, green, blue);
  } else {
    strip.setPixelColor(led, 0, 0, 0);
  }
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

void setRed(String command) {
  command.remove(0, 2);
  r = command.toInt();
}
void setGreen(String command) {
  command.remove(0, 2);
  g = command.toInt();
}
void setBlue(String command) {
  command.remove(0, 2);
  b = command.toInt();
}

/* FUNCTIONS TO SET GRADIENT START/END COLORS */
void setStartGradient(String command) {
  command.remove(0, 2);
  start_red = getValue(command, ',', 0).toInt();
  start_green = getValue(command, ',', 1).toInt();
  start_blue = getValue(command, ',', 2).toInt();
}
void setEndGradient(String command) {
  command.remove(0, 2);
  end_red = getValue(command, ',', 0).toInt();
  end_green = getValue(command, ',', 1).toInt();
  end_blue = getValue(command, ',', 2).toInt();
}

/* FUNCTIONS TO SET PATTERN/COLOR MODE */
void _setMode(String command) {
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
      newRed = start_red;
    }
    if (start_green != end_green) {
      if (start_green < end_green) {
        newGreen = round(start_green + (led * 0.77));
      } else {
        newGreen = round(start_green - (led * 0.77));
      }
    } else {
      newGreen = start_green;
    }
    if (start_blue != end_blue) {
      if (start_blue < end_blue) {
        newBlue = round(start_blue + (led * 0.77));
      } else {
        newBlue = round(start_blue - (led * 0.77));
      }
    } else {
      newBlue = start_blue;
    }
    show(led, newRed, newGreen, newBlue);
  }
  strip.show();
}

void red_green() {
  start_red = 255;
  for (int i = 0; i < 255; i++) {
    start_red--;
    int newRed;
    int newGreen;
    for (int led = 0; led < 330; led++) {
      newRed = abs(start_red - (led * 0.77));
      newGreen = 255 - newRed;
      show(led, newRed, newGreen, 0);
    }
    strip.show();
  }

  start_green = 255;
  for (int i = 0; i < 255; i++) {
    start_green--;
    int newRed;
    int newGreen;
    for (int led = 0; led < 330; led++) {
      newGreen = abs(start_green - (led * 0.77));
      newRed = 255 - newGreen;
      show(led, newRed, newGreen, 0);
    }
    strip.show();
  }
}

void red_blue() {
  start_red = 255;
  for (int i = 0; i < 255; i++) {
    start_red--;
    int newRed;
    int newGreen;
    for (int led = 0; led < 330; led++) {
      newRed = abs(start_red - (led * 0.77));
      newGreen = 255 - newRed;
      show(led, newRed, newGreen, 0);
    }
    strip.show();
  }

  start_blue = 255;
  for (int i = 0; i < 255; i++) {
    start_green--;
    int newRed;
    int newBlue;
    for (int led = 0; led < 330; led++) {
      newBlue = abs(start_blue - (led * 0.77));
      newRed = 255 - newBlue;
      show(led, newRed, 0, newBlue);
    }
    strip.show();
  }
}

void solidFade() {
  int newRed = 255;
  int newGreen = 0;
  for (int i = 0; i < 255; i++) {
    newRed--;
    newGreen++;
    for (int led = 0; led < 330; led++) {
      show(led, newRed, newGreen, 0);
    }
    strip.show();
  }
  for (int i = 0; i < 255; i++) {
    newRed++;
    newGreen--;
    for (int led = 0; led < 330; led++) {
      show(led, newRed, newGreen, 0);
    }
    strip.show();
  }
}

void solid() {
  for (int led = 0; led < PIXEL_COUNT; led++) {
    //strip.setPixelColor(led, r, g, b);
    show(led, r, g, b);
  }
  strip.show();
}

void rainbow() {
  uint16_t led, j;
  for (j = 0; j < 256 * 5; j++) {
    for (led = 0; led < strip.numPixels(); led++) {
      if (led >= IT[0] && led <= IT[1] ||                         //IT
          led >= IS[0] && led <= IS[1] ||                          //IS
          led >= CURRENT_MINUTE[0] && led <= CURRENT_MINUTE[1] ||  //FIVE/TEN/QUARTER/TWENTY/HALF
          led >= FIVE[0] && led <= FIVE[1] ||                      //FIVE
          led >= MINUTES[0] && led <= MINUTES[1] ||                //MINUTES
          led >= TO_PAST[0] && led <= TO_PAST[1] ||                //TO/PAST
          led >= CURRENT_HOUR[0] && led <= CURRENT_HOUR[1] ||      //ONE/TWO/THREE/FOUR/FIVE/SIX/SEVEN/EIGHT/NINE/TEN/ELEVEN/TWELVE
          led >= OCLOCK[0] && led <= OCLOCK[1]) {                  //OCLOCK
        strip.setPixelColor(led, Wheel(((led * 256 / PIXEL_COUNT) + j) & 255));
      } else {
        strip.setPixelColor(led, 0, 0, 0);
      }
    }
    strip.show();
  }
}

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

uint32_t Wheel2(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 127) {
    return strip.Color(255 - WheelPos * 2, 0, WheelPos * 2);
  }
  if (WheelPos > 127) {
    WheelPos -= 127;
    return strip.Color(WheelPos * 2, 0, 255 - WheelPos * 2);
  }
}

void resetNeo() {
  for (int i = 0; i < PIXEL_COUNT; i++) {
    strip.setPixelColor(i, 0, 0, 0);
  }
  strip.show();
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
