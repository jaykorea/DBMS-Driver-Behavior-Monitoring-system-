// SafeString_ReadCmds.ino
//
// This example takes commmands from the Arduino Monitor input and acts on them
// the available commands are start stop and reset
// Commands are delimited by space dot comma NL or CR
// or if no input received for 0.1sec
//
// These commands can be picked out of a line of user input
//
// download and install the SafeString library from
// www.forward.com.au/pfod/ArduinoProgramming/SafeString/index.html
// download and install the millisDelay library from
// https://www.forward.com.au/pfod/ArduinoProgramming/TimingDelaysInArduino.html
#include "SafeString.h"
#include "millisDelay.h"

millisDelay timeout;
unsigned long TIMEOUT_MS = 100; // 0.1sec

const size_t maxCmdLength = 5; // make SafeStrings at least large enough to hold longest cmd
// Use SafeStrings for the commands as comparing two SafeStrings is generally faster as the lengths can be compared first.
createSafeString(startCmdStr, maxCmdLength, "start");
createSafeString(stopCmdStr, maxCmdLength, "stop");
createSafeString(resetCmdStr, maxCmdLength, "reset");

// input must be large enough to hold longest cmd + 1 delimiter
createSafeString(input, maxCmdLength + 1); //  to read input cmd + 1 delimiter
createSafeString(token, maxCmdLength + 1); // for parsing capacity >= input.capacity()

char delimiters[] = " .,\r\n"; // space dot comma CR NL are cmd delimiters

bool running = true;
unsigned long loopCounter = 0;

void setup() {
  Serial.begin(9600);    // Open serial communications and wait a few seconds
  for (int i = 10; i > 0; i--) {
    Serial.print(' '); Serial.print(i);
    delay(500);
  }
  Serial.println();
  // first run without any outputting any error msgs or debugging
  //SafeString::setOutput(Serial); // enable error messages and debug() output to be sent to Serial

  // show msg to user if maxCmdLength too small one of these cmdStr will be obviously empty
  Serial.print(F("Enter one, or more, of the commands : ")); Serial.print(startCmdStr); Serial.print(" , "); Serial.print(stopCmdStr);
  Serial.print(" or "); Serial.print(resetCmdStr);  Serial.println(F("  separated by space , . CR or NL"));
  if (running) {
    Serial.println(F(" Counter Started"));
  }
}

void loop() {
  if (input.read(Serial)) {  // read from Serial, returns true if at least one character was added to SafeString input
    input.debug("after read => ");
    timeout.start(TIMEOUT_MS); // restart a 0.1sec timer every time something is read
  }

  if (input.nextToken(token, delimiters)) { // process at most one token per loop does not return tokens longer than input.capacity()
    token.debug("after nextToken => ");

    if (token == startCmdStr) {
      running = true;  Serial.print(F("start at ")); Serial.println(loopCounter);

    } else if (token == stopCmdStr) {
      running = false; Serial.print(F("stop at ")); Serial.println(loopCounter);

    } else if (token == resetCmdStr) {
      loopCounter = 0; Serial.print(F("reset Counter:")); Serial.println(loopCounter);

    }// else  // not a valid cmd ignore
  }
  if (timeout.justFinished()) { // nothing received for 0.1secs, terminated last chars so token will be processed.
    input += delimiters[0]; // any delimiter will do
    SafeString::Output.print(F("Input timed out"));
  }
  // rest of code here is executed while the user typing in commands
  if (running) {
    loopCounter++;
    if ((loopCounter % 100000) == 0) {
      Serial.print(F("Counter:")); Serial.println(loopCounter);
    }
  }
}
