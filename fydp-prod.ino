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

// These control the R and G values as digital I/O
int redOnOff = D4;
int greenOnOff = D3;

// These control values as PWM
int blueBrightness = D2; 
int coldBrightness = D1;
int warmBrightness = D0;

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
    pinMode(ambientFocusSwitch, INPUT);

    // Digital outputs
    pinMode(rgbOnOff, OUTPUT);
    pinMode(coldOnOff, OUTPUT);
    pinMode(warmOnOff, OUTPUT);
    pinMode(redOnOff, OUTPUT);
    pinMode(greenOnOff, OUTPUT);
    
    // PWM outputs
    pinMode(blueBrightness, OUTPUT);
    pinMode(coldBrightness, OUTPUT);
    pinMode(warmBrightness, OUTPUT);
    
    // FOR DEBUGGING PURPOSES
    pinMode(D7, OUTPUT);
}

int cloudSetTimezone(String timezone)
{
    // Expand this to support more timezones
    if (timezone == "EST")
    {
        userTimezone = EST;
        
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
    setupPins();
    setupCloudFunctions();
    
    // Initialize all global values to their starting values
}

//------------------------------------------//
//              Main loop
//------------------------------------------//
void loop() 
{

}