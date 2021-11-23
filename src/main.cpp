#include "Arduino.h"
#include "defines.h"
#include "term.h"
#include "glitcher.h"
#include "nrf_swd.h"
#include "swd.h"

SerialTerminal term;

void setup()
{
    // Startup delay to initialize serial port
    delay(500);

    // Initialize serial port
    Serial.begin(115200);

    begin_term();
}

void loop()
{
    read_serial();
}
