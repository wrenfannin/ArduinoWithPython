// Pinout for this code



   /* Vio connects to 5V (or, with a 3V logic Arduino, 3V)
    GND connects to GND
    Key connects to GND (always on)
    RX connects to digital 2 (NEED TO CHANGE UNLESS CAN FIX 2 PIN)
    TX connects to digital 3 (9 on Leo/Micro, 10 on Mega)
    RST connects to digital 4*/

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
  Serial.println(F("Testing Module")); // Prints message to show user tests are being run, can be removed
  Serial.println(F("Initializing...."));

  fonaSerial->begin(4800);
  if (! fona.begin(*fonaSerial)) { // If there is no FONA (SIM) module wired to the arduino print this to the user
    Serial.println(F("Couldn't find FONA module, check connections"));
    while (1);
  }
  type = fona.type(); // Holds the model of the module
  Serial.println(F("FONA is OK")); // Shows user tests have run
  Serial.print(F("Found "));
  switch (type) { // This if prints the model of the FONA module and tells the user if it is not recognised
    case FONA800H:
      Serial.println(F("FONA 800H")); break; // Prints this in the statement above and ends the statement
    default:
      Serial.println(F("An unrecognised model")); break;
  }


// This is the end of the setup process, now I have written a list of options the user can choose from
// They include :Making calls, getting the status of the current call, play audio to test speakers/ headphones, getting the strength/status of the mobile network
// setting and changing volume, read SMS, send SMS, printing the options menu, turn on PWM pins (can be connected to something like a buzzer or a light)
// This list will be controlled via python
    printMenu();
}

void printMenu(void) {
  Serial.println(F("-------------------------------------")); // This menu is mostly for testing purposes but clearly shows how to access each feature
  Serial.println(F("[?] Print this menu"));  // All of these options can be accessed via python with the Pyserial library
  Serial.println(F("[n] get Network status"));
  Serial.println(F("[v] set audio Volume"));
  Serial.println(F("[V] get Volume"));
  Serial.println(F("[T] play audio Tone"));
  Serial.println(F("[P] PWM/Buzzer out"));

  // Phone
  Serial.println(F("[c] make phone Call"));
  Serial.println(F("[A] get call status"));
  Serial.println(F("[h] Hang up phone"));
  Serial.println(F("[p] Pick up phone"));

  // SMS
  Serial.println(F("[N] Number of SMSs"));
  Serial.println(F("[r] Read SMS #"));
  Serial.println(F("[R] Read All SMS"));
  Serial.println(F("[d] Delete SMS #"));
  Serial.println(F("[s] Send SMS"));
}
void loop() {
  Serial.print(F("FONA> "));
  while (! Serial.available() ) {
    if (fona.available()) {
      Serial.write(fona.read());
    }
  }

  char command = Serial.read(); // sets command as a vairable where it will print the value into the serial monitor so it is run
  Serial.println(command);


  switch (command) {
    case '?': {
        printMenu();// The printMenu function prints out the initial menu so the user can see it once again, can be changed out later down the line
        break;
      }



    case 'n': {
        // read the network/cellular status
        uint8_t n = fona.getNetworkStatus(); // This function returns a number based on the status of the mobile network which the arduino prints into the serial monitor
        Serial.print(F("Network status "));
        Serial.print(n);
        Serial.print(F(": "));
        if (n == 0) Serial.println(F("Not registered")); // number 1-5 is returned based on the result
        if (n == 1) Serial.println(F("Registered (home)"));
        if (n == 2) Serial.println(F("Not registered (searching)"));
        if (n == 3) Serial.println(F("Denied"));
        if (n == 4) Serial.println(F("Unknown"));
        if (n == 5) Serial.println(F("Registered roaming"));
        break;
      }

    /*** Audio ***/
    case 'v': {
        // set volume
        flushSerial();
        if ( (type == FONA3G_A) || (type == FONA3G_E) ) { // checks the type of FONA module as some modules display their volume percentage in a different format
          Serial.print(F("Set Vol [0-8] "));
        } else {
          Serial.print(F("Set Vol % [0-100] ")); // This option will be used for my circuit as I am using something other than the two modules above, they are kept in the if statement in case they are used to replicate my project
        }
        uint8_t vol = readnumber(); // a unint8_t is an unsigned integer which allows us to save memory on the arduino
        Serial.println();
        if (! fona.setVolume(vol)) {
          Serial.println(F("Failed"));
        } else {   // Prints the final result of the operation
          Serial.println(F("OK!"));
        }
        break;
      }

    case 'V': {
        uint8_t v = fona.getVolume();
        Serial.print(v);
        if ( (type == FONA3G_A) || (type == FONA3G_E) ) { // Different models use different methods of storing volume level
          Serial.println(" / 8");
        } else {
          Serial.println("%");
        }
        break;
      }

    case 'T': {
        // play tone
        flushSerial();
        Serial.print(F("Play tone #"));
        uint8_t kittone = readnumber(); // Plays a sound which is useful for checking audio connection
        Serial.println();
        // play for 1 second (1000 ms)
        if (! fona.playToolkitTone(kittone, 1000)) {
          Serial.println(F("Failed"));
        } else {
          Serial.println(F("OK!"));
        }
        break;
      }

    /*** PWM ***/
// My model of FONA module has an empty PWM pin, PWM stands for pulsewidth modulation which allows the headers to control variable current components such as motors, not in use in the project but left in in case it became useful in the future
    case 'P': {
        // PWM Buzzer output @ 2KHz max
        flushSerial();
        Serial.print(F("PWM Freq, 0 = Off, (1-2000): ")); // Selects the desired frequency to run the PWM headers at from 1 to 2000 KHz
        uint16_t freq = readnumber();
        Serial.println();
        if (! fona.setPWM(freq)) {
          Serial.println(F("Failed"));
        } else {
          Serial.println(F("OK!")); // Prints final result
        }
        break;
      }

    /*** Call ***/
    case 'c': {
        // call a phone!
        char number[30]; // Creates the array number with the size of 30 so it can be acsessed from the rest of the function 
        flushSerial();
        Serial.print(F("Call #"));
        readline(number, 30);
        Serial.println();
        Serial.print(F("Calling ")); Serial.println(number);
        if (!fona.callPhone(number)) {
          Serial.println(F("Failed"));
        } else {
          Serial.println(F("Sent!")); // Shows result of function
        }

        break;
      }
    case 'A': {
        // get call status
        int8_t callstat = fona.getCallStatus(); // function to check the status of the current call whether it is ready, ringing, ongoing or has some sort of error
        switch (callstat) {
          case 0: Serial.println(F("Ready")); break;
          case 1: Serial.println(F("Could not get status")); break;
          case 3: Serial.println(F("Ringing (incoming)")); break;
          case 4: Serial.println(F("Ringing/in progress (outgoing)")); break;
          default: Serial.println(F("Unknown")); break;
        }
        break;
      }
      
    case 'h': {
        // hang up!
        if (! fona.hangUp()) { // this function ends the current call
          Serial.println(F("Failed"));
        } else {
          Serial.println(F("OK!"));
        }
        break;
      }

    case 'p': {
        // pick up!
        if (! fona.pickUp()) { // this function answers the current caller
          Serial.println(F("Failed"));
        } else {
          Serial.println(F("OK!"));
        }
        break;
      }

    /*** SMS ***/

    case 'N': {
        // read the number of SMS's!
        int8_t smsnum = fona.getNumSMS(); // checks the number of SMS messages that are stored 
        if (smsnum < 0) {
          Serial.println(F("Could not read # SMS"));
        } else {
          Serial.print(smsnum);
          Serial.println(F(" SMS's on SIM card!"));
        }
        break;
      }
    case 'r': {
        // read an SMS
        flushSerial(); // gives a list of numbers that messages have been recieved from and lets the user choose which one is to be read and printed
        Serial.print(F("Read #"));
        uint8_t smsn = readnumber();
        Serial.print(F("\n\rReading SMS #")); Serial.println(smsn);

        // Retrieve SMS sender address/phone number.
        if (! fona.getSMSSender(smsn, replybuffer, 250)) {
          Serial.println("Failed!");
          break;
        }
        Serial.print(F("FROM: ")); Serial.println(replybuffer);

        // Retrieve SMS value.
        uint16_t smslen;
        if (! fona.readSMS(smsn, replybuffer, 250, &smslen)) { // pass in buffer and max len!
          Serial.println("Failed!");
          break;
        }
        Serial.print(F("***** SMS #")); Serial.print(smsn);
        Serial.print(" ("); Serial.print(smslen); Serial.println(F(") bytes *****"));
        Serial.println(replybuffer);
        Serial.println(F("*****"));

        break;
      }
    case 'R': {
        // read all SMS
        int8_t smsnum = fona.getNumSMS(); // prints all SMS at once
        uint16_t smslen; // creates unsinged value which represents the length of a message
        int8_t smsn; // signed integer to store the number of messages

        if ( (type == FONA3G_A) || (type == FONA3G_E) ) { // options used for other FONA models which have different ways of indexing data
          smsn = 0; // zero indexed
          smsnum--;
        } else {
          smsn = 1;  // 1 indexed
        }

        for ( ; smsn <= smsnum; smsn++) {
          Serial.print(F("\n\rReading SMS #")); Serial.println(smsn);
          if (!fona.readSMS(smsn, replybuffer, 250, &smslen)) {  // pass in buffer and max len!
            Serial.println(F("Failed!"));
            break;
          }
          // if the length is zero, its a special case where the index number is higher
          // this if statement fixes an error that arises if a message with no data is recieved
          if (smslen == 0) {
            Serial.println(F("[empty slot]"));
            smsnum++;
            continue;
          }

          Serial.print(F("***** SMS #")); Serial.print(smsn);
          Serial.print(" ("); Serial.print(smslen); Serial.println(F(") bytes *****"));
          Serial.println(replybuffer);
          Serial.println(F("*****"));
        }
        break;
      }

    case 'd': {
        // delete an SMS
        flushSerial();
        Serial.print(F("Delete #"));
        uint8_t smsn = readnumber();

        Serial.print(F("\n\rDeleting SMS #")); Serial.println(smsn);
        if (fona.deleteSMS(smsn)) {
          Serial.println(F("OK!")); // uses functions and values of smsn to select and delete the correct method
        } else {
          Serial.println(F("Couldn't delete")); // returns an error message if the message cannot be deleted
        }
        break;
      }

    case 's': {
        // send an SMS!
        char sendto[21], message[141]; // creates variables for the number to be sent to and the message with a character limit of 140, both as arrays
        flushSerial();
        Serial.print(F("Send to #"));
        readline(sendto, 20);
        Serial.println(sendto);
        Serial.print(F("Type out one-line message (140 char): "));
        readline(message, 140);
        Serial.println(message);
        if (!fona.sendSMS(sendto, message)) {
          Serial.println(F("Failed"));
        } else {
          Serial.println(F("Sent!"));
        }

        break;
      }

    case 'u': {
      // send a USSD! ONLY USED IN IOT, this sends a very short message which can be used for triggers for smart devices etc etc
      char message[141];
      flushSerial();
      Serial.print(F("Type out one-line message (140 char): "));
      readline(message, 140);
      Serial.println(message);

      uint16_t ussdlen;
      if (!fona.sendUSSD(message, replybuffer, 250, &ussdlen)) { // pass in buffer and max len!
        Serial.println(F("Failed"));
      } else {
        Serial.println(F("Sent!"));
        Serial.print(F("***** USSD Reply"));
        Serial.print(" ("); Serial.print(ussdlen); Serial.println(F(") bytes *****"));
        Serial.println(replybuffer);
        Serial.println(F("*****"));
      }
    }

    default: {
        Serial.println(F("Unknown command")); // if an input that is not in the menu is recieved the program will return an error and reprint the starting screen
        printMenu();
        break;
      }
  }
  // flush input
  flushSerial(); // clears current data
  while (fona.available()) {
    Serial.write(fona.read());
  }

}

void flushSerial() {
  while (Serial.available())
    Serial.read();
}

char readBlocking() {
  while (!Serial.available());
  return Serial.read();
}
uint16_t readnumber() {
  uint16_t x = 0;
  char c;
  while (! isdigit(c = readBlocking())) {
    //Serial.print(c);
  }
  Serial.print(c);
  x = c - '0';
  while (isdigit(c = readBlocking())) {
    Serial.print(c);
    x *= 10;
    x += c - '0';
  }
  return x;
}

uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout) {
  uint16_t buffidx = 0;
  boolean timeoutvalid = true;
  if (timeout == 0) timeoutvalid = false;

  while (true) {
    if (buffidx > maxbuff) {
      //Serial.println(F("SPACE"));
      break;
    }

    while (Serial.available()) {
      char c =  Serial.read();

      //Serial.print(c, HEX); Serial.print("#"); Serial.println(c);

      if (c == '\r') continue;
      if (c == 0xA) {
        if (buffidx == 0)   // the first 0x0A is ignored
          continue;

        timeout = 0;         // the second 0x0A is the end of the line
        timeoutvalid = true;
        break;
      }
      buff[buffidx] = c;
      buffidx++;
    }

    if (timeoutvalid && timeout == 0) {
      //Serial.println(F("TIMEOUT"));
      break;
    }
    delay(1);
  }
  buff[buffidx] = 0;  // null term
  return buffidx;
}

 
