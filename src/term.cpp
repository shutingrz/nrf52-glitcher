
#include "term.h"
#include "nrf_term.h"

SerialTerminal termSession;
SerialTerminal term_root;

void begin_term()
{
    create_term_root();
    create_term_nrf();
    termSession = term_root;
    showLogo();
    cmd_root_help();
    printConsoleChar();
}

void showLogo()
{
    const char* logo =
        "  ____  _  _  _         _                 \n"
        " / ___|| |(_)| |_  ___ | |__    ___  _ __ \n"
        "| |  _ | || || __|/ __|| '_ \\  / _ \\| '__|\n"
        "| |_| || || || |_| (__ | | | ||  __/| |   \n"
        " \\____||_||_| \\__|\\___||_| |_| \\___||_|    by @shutingrz\n";
    
    Serial.println(logo);
}

void read_serial()
{
    termSession.readSerial();
}

void unknownCommand(const char *command)
{
    // Print unknown command
    Serial.print(F("Unknown command: "));
    Serial.println(command);
}

SerialTerminal getTermSession()
{
    return termSession;
}

void setTermSession(SerialTerminal term)
{
    termSession = term;
}

SerialTerminal create_term()
{
    SerialTerminal term;
    term.setDefaultHandler(unknownCommand);
    term.setSerialEcho(true);
    return term;
}

//---------------------------------------------------

void create_term_root()
{
    SerialTerminal term = create_term();
    term.setPostCommandHandler(printConsoleChar);
    term.addCommand("help", cmd_root_help);
    term.addCommand("nrf", cmd_root_nrf); 
    term_root = term;
}

void switch_root()
{
    setTermSession(term_root);
}

void printConsoleChar()
{
    Serial.print(F("> "));
}

void cmd_root_nrf()
{
    switch_nrf();
}

void cmd_root_help()
{
    Serial.println(F("Serial terminal usage:"));
    Serial.println(F("  help               Print this usage"));
    Serial.println(F("  nrf                Voltage glitch for nrf\n"));
}