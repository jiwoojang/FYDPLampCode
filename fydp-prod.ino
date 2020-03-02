#include <math.h>
//------------------------------------------//
//                Pin setup
//------------------------------------------//

// Digital input pin
int ambientFocusSwitch = A4;

//Analog input pin
int brightness = A3;

// Pull these pin high for on, low for off
int rgbOnOff = A2;
int coldOnOff = A1;
int warmOnOff = A0;

// These control values as PWM
int blueBrightness = D1;
int otherBrightness = D0;

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
        Time.zone(-5 + addDst);
        serial.print(Time.now());
        serial.print(Time.zone(-5 + addDst));


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

string modBValue() {//(int sunrise, int sunset)

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

correctBBrightness(string phase, string hour) {
  int b = 0;
  //446-477 nm bounds for optimization are both 255

  if (phase === "am") {
    b = 255;
  }
  else if (phase === "midday") {
    int delta = sin((hour % 2) * M_PI / 2) * 5;
    b = 255 - delta;
    //midday adjustment function
  }
  else if (phase === "pm") {
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
}

int blueVal = 0;

int prevARead = 0;

int prevDRead = HIGH;
int lastDRead = LOW;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

int brightnessVal = 0;

int state = 0;

//------------------------------------------//
//              Main loop
//------------------------------------------//
void loop()
{
    int aRead = analogRead(brightness);
    //Serial.printlnf("aRead: %d prevARead %d\n delta: %d", aRead, prevARead, abs(aRead - prevARead));

    if (abs(aRead - prevARead) > 60)
    {
        prevARead = aRead;
        float norm = aRead / 4095.0;
        brightnessVal = (int)(255 * norm);

        analogWrite(otherBrightness, brightnessVal, 200);
        analogWrite(blueBrightness, brightnessVal, 200);

        Serial.printlnf("Setting brigntness to %d\n", brightnessVal);
    }

    correctBBrightness(modBValue(), Time.hour());

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
                Serial.printlnf("Switching state to %d\n", brightnessVal);
            }
        }
    }

    lastDRead = dRead;

    if (state == 0)
    {
        digitalWrite(coldOnOff, HIGH);
        digitalWrite(warmOnOff, HIGH);

        digitalWrite(rgbOnOff, LOW);
    }
    else if(state == 1)
    {
        digitalWrite(warmOnOff, HIGH);
        digitalWrite(rgbOnOff, HIGH);

        digitalWrite(coldOnOff, LOW);
    }
    else if(state == 2)
    {
        digitalWrite(coldOnOff, HIGH);
        digitalWrite(rgbOnOff, HIGH);

        digitalWrite(warmOnOff, LOW);
    }

    /*if(Time.hour() > )
    {

    }
    else if (Time.hour() )
    {

    }
    else
    {

    }*/
}
