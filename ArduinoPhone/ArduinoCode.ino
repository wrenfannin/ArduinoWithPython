#include "Adafruit_FONA.h"  // This is the library which runs the SIM module
// This is in brackets because it is using an outside library which does not need to be loaded onto the Arduino's CPU
#define FONA_RST 4 // Defines the reset pin that is used later on

char replybuffer[255]; // Reply buffer that is needed for the project to work

#if (defined(__AVR__)) // Selects the right programmer for my arduino, for this code to work with other controllers more options must be added here, so this is left as an if statement

#include <SoftwareSerial.h> // Used to communicate over the serial port

#define FONA_RX 2
#define FONA_TX 3

SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
SoftwareSerial *fonaSerial = &fonaSS;

#endif

Adafruit_FONA fona = Adafruit_FONA(FONA_RST); // Defines SIM module to the library

uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout = 0);

uint8_t type;

void setup() {
  while (!Serial);

  Serial.begin(115200); // Sets the baud rate of serial monitor
  Serial.println(F("FONA basic test")); // Prints message to show user tests are being run, can be removed
  Serial.println(F("Initializing....(May take 3 seconds)"));

  fonaSerial->begin(4800);
  if (! fona.begin(*fonaSerial)) { // If there is no FONA (SIM) module wired to the arduino print this to the user
    Serial.println(F("Couldn't find FONA"));
    while (1);
  }
  type = fona.type(); // Holds the model of the module
  Serial.println(F("FONA is OK")); // Shows user tests have run
  Serial.print(F("Found "));
  switch (type) { // This if prints the model of the FONA module and tells the user if it is not recognised
    case FONA800H:
      Serial.println(F("FONA 800H")); break; // Prints this in the statement above and ends the statement
    else:
      Serial.println(F("An unrecognised model")); break;
  }


// This is the end of the setup process, now I have written a list of options the user can choose from
// They include :Making calls, getting the status of the current call, play audio to test speakers/ headphones, getting the strength/status of the mobile network
// setting and changing volume, read SMS, send SMS, printing the options menu, turn on PWM pins (can be connected to something like a buzzer or a light)
// This list will be controlled via python
