#pragma once
#include <ErriezSerialTerminal.h>

void begin_term();
void read_serial();

SerialTerminal getTermSession();
void setTermSession(SerialTerminal);
void unknownCommand(const char *command);
SerialTerminal create_term();
void showLogo();
void switch_root();
void cmd_root_help();
void cmd_root_nrf();
void cmd_nrf_help();
void cmd_nrf_exit();
void printConsoleChar();
void create_term_root();