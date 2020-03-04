#include <math.h>

//------------------------------------------//
//                Pin setup
//------------------------------------------//

// Digital input pin
int ambientFocusSwitch = A4;

//Analog input pin
int brightness = A3;

// Pull these pin high for on, low for off
int warmOnOff = A2;
int rgbOnOff = A1;
int coldOnOff = A0;

// These control values as PWM
int blueBrightness = D0;
int otherBrightness = D1;

//subject to change, midday blue level
//should follow a sinusoid of period 1 hr, small amplitude

//------------------------------------------//
//               Custom Types
//------------------------------------------//

// Current supported timezones
enum timezoneType
{
    NONE,
    EST,
    PST,
    AST,
};

//------------------------------------------//
//              Global Variables
//------------------------------------------//

// Start this as invalid timezone
timezoneType userTimezone = NONE;

//------------------------------------------//
//              Init functions
//------------------------------------------//

void setupPins()
{
    // Analog input
    pinMode(brightness, AN_INPUT);

    // Digital input
    pinMode(ambientFocusSwitch, INPUT_PULLUP);

    // Digital outputs
    pinMode(rgbOnOff, OUTPUT);
    pinMode(coldOnOff, OUTPUT);
    pinMode(warmOnOff, OUTPUT);

    // PWM outputs
    pinMode(blueBrightness, OUTPUT);
    pinMode(otherBrightness, OUTPUT);

    // FOR DEBUGGING PURPOSES
    pinMode(D7, OUTPUT);
}

int cloudSetTimezone(String timezone)
{
    // Expand this to support more timezones
    if (Time.isDST()) {
      Time.setDSTOffset(Time.getDSTOffset());
    }

    if (timezone == "EST")
    {
        //userTimezone = EST;
        Time.zone(-5 + Time.getDSTOffset());
        Serial.print(Time.now());
        //Serial.print(Time.zone(-5 + addDst));


        // FOR DEBUGGING PURPOSES
        digitalWrite(D7, HIGH);
        return 1;
    }
    else
    {
        // FOR DEBUGGING PURPOSES
        digitalWrite(D7, LOW);
        return 0;
    }
}

String modBValue() {//(int sunrise, int sunset)

  int h = Time.hour();

  if (h < 10)
  {
    return "am";//earl
  }
  else if (h > 20)
  {
    return "pm";
  }
  else
  {
    return "midday";
  }
}

int correctBBrightness(String phase, int hour) {
  int b = 0;
  //446-477 nm bounds for optimization are both 255

  if (phase == "am") {
    b = 255;
  }
  else if (phase == "midday") {
    hour = 20;
    double delta = -1 *84 * cos((hour-10) * M_PI / 10) + 84;
    int deltaInt = (int)floor(delta);

    b = 255 - deltaInt;
    //midday adjustment function
  }
  else if (phase == "pm") {
    b = 87; //  2700: (255, 169, 87)
  }

  return b;
}

void setupCloudFunctions()
{
    Particle.function("SetTimezone", cloudSetTimezone);
}

//------------------------------------------//
//              Main init
//------------------------------------------//
void setup()
{
    // Set everything up
    //beginDST(); necessary?
    setupPins();
    setupCloudFunctions();

    digitalWrite(rgbOnOff, HIGH);
    digitalWrite(coldOnOff, HIGH);
    digitalWrite(warmOnOff, HIGH);

    // Initialize all global values to their starting values
    if (Time.isDST()) {
      Time.setDSTOffset(Time.getDSTOffset());
    }

    //userTimezone = EST;
    Time.zone(-5);
}

int prevARead = 0;

int prevDRead = HIGH;
int lastDRead = LOW;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

int blueVal = 0;

int state = 0;

//------------------------------------------//
//              Main loop
//------------------------------------------//
void loop()
{
    time_t time = Time.now();
    Serial.printlnf(Time.timeStr());
    Serial.printlnf("Curent hour: %d\n", Time.hour());
    
    int newBlueVal = correctBBrightness(modBValue(), Time.hour());
    
    if (blueVal != newBlueVal)
    {
        Serial.printlnf("Adjusted blue value: %d\n", blueVal);
        blueVal = newBlueVal;
    }

    int aRead = analogRead(brightness);

    if (abs(aRead - prevARead) > 60)
    {
        prevARead = aRead;
        float norm = aRead / 4095.0;
        int brightnessVal = (int)floor(255 * norm);
        int blueBrightnessVal = (int)floor(blueVal * norm);

        analogWrite(otherBrightness, brightnessVal, 200);
        analogWrite(blueBrightness, blueBrightnessVal, 200);

        Serial.printlnf("Setting brigntness to %d\n", brightnessVal);
    }

    int dRead = digitalRead(ambientFocusSwitch);

    if (dRead != lastDRead)
    {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay)
    {
        if (dRead != prevDRead)
        {
            prevDRead = dRead;

            if (prevDRead == 0)
            {
                state++;

                state = state % 3;
                Serial.printlnf("Switching state to %d\n", state);
            }
        }
    }

    lastDRead = dRead;
    
    String phase = modBValue();
    Serial.printlnf("State: %d\n", state);
    Serial.printlnf(phase);

    if (state == 0)
    {
        digitalWrite(coldOnOff, HIGH);
        digitalWrite(warmOnOff, HIGH);

        digitalWrite(rgbOnOff, LOW);
    }
    else if(state == 1 && (phase == "am" || phase == "midday"))
    {
        digitalWrite(warmOnOff, HIGH);
        digitalWrite(rgbOnOff, HIGH);

        digitalWrite(coldOnOff, LOW);
    }
    else if(state == 1 && phase == "pm")
    {
        digitalWrite(coldOnOff, HIGH);
        digitalWrite(rgbOnOff, HIGH);

        digitalWrite(warmOnOff, LOW);
    }
    else if(state == 2 )
    {
        digitalWrite(coldOnOff, HIGH);
        digitalWrite(rgbOnOff, HIGH);
        digitalWrite(warmOnOff, HIGH);
    }
}