#include "term.h"
#include "nrf_term.h"
#include "glitcher.h"
#include "defines.h"
#include "nrf_swd.h"
#include "swd.h"

SerialTerminal term_nrf;

struct Param
{
    const char* name;
    uint32_t* var;
};

Param params[] = {
    {"delay_start", &delay_start},
    {"width_start", &width_start},
    {"delay_end", &delay_max},
    {"width_end", &width_max},
    {"paranoia_mode", &paranoia_mode}
};

void create_term_nrf()
{
    SerialTerminal term = create_term();
    term.setPostCommandHandler(printConsoleChar_nrf);
    term.addCommand("help", cmd_nrf_help);
    term.addCommand("exit", cmd_nrf_exit);
    term.addCommand("show", cmd_nrf_show);
    term.addCommand("set", cmd_nrf_set);
    term.addCommand("power", cmd_nrf_power);
    term.addCommand("run", cmd_nrf_run);
    term.addCommand("swd", cmd_nrf_swd);
    term_nrf = term;
}

void switch_nrf()
{
    setTermSession(term_nrf);
    Serial.println(F("switch nrf mode.\n"));
    glitcher_prepare();
    cmd_nrf_help();
}

void printConsoleChar_nrf()
{
    Serial.print(F("nrf"));
    Serial.print(F("> "));
}

void cmd_nrf_exit()
{
    switch_root();
}

void cmd_nrf_help()
{
    Serial.println(F("Serial terminal usage:"));
    Serial.println(F("  help               Print this usage"));
    Serial.println(F("  set <item> <value> set param"));
    Serial.println(F("  exit               exit nrf mode"));
    Serial.println(F("  show               show glitch options"));
    Serial.println(F("  power <on|off>     Power on or off nRF."));
    Serial.println(F("  swd                SWD IDCODE check"));
    Serial.println(F(""));
    Serial.println(F("You can set glitch option value with \"set\" command."));
    Serial.println(F(" Params: delay_start, delay_end, width_start, width_end, paranoia_mode"));
    Serial.println(F("and can get current value with \"show\" command. \n"));
}

void cmd_nrf_show()
{
    Serial.println(F("----------------"));
    Serial.println(F("Glitch options:"));
    for(Param x: params)
    {
        Serial.printf("  %s\t: %d\n", x.name, *x.var);
    }
    Serial.println(F("----------------"));
}

void cmd_nrf_run()
{
    char buf;
    uint32_t idcode;
    
    set_power(true);
    delay(10);
    swd_begin();
    idcode = swd_init();
    swd_stop();

    if(idcode != NRF52_IDCODE)
    {
        Serial.printf("[!]SWD IDCODE is not 0x%08x (target: 0x%08x).\n", NRF52_IDCODE, idcode);
        Serial.printf("The target may not be nRF, or the pin connections may be incorrect.\n\ncontinue? (y/n): ");
        while(1){
            if(Serial.available() > 0){
                buf = Serial.read();
                Serial.printf("%c\n", buf);
                if(buf == 'y'){
                    break;
                }else{
                    Serial.println("abort.");
                    return;
                }
            }
        }
    }

    cmd_nrf_show();
    Serial.println("Start glitch! Press any key to stop.");
    glitcher_begin();
    set_glitcher(true);
    
    attachInterrupt(SERIAL_RX, serial_rx_interrupt_nrf, RISING);
    while (get_glitcher())
    {
        do_glitcher();
    }
    glitcher_stop();
    detachInterrupt(SERIAL_RX);
}

void cmd_nrf_power()
{
    char *arg = NULL;

    SerialTerminal term = getTermSession();
    arg = term.getNext();
    if(arg == NULL){
        Serial.println(F("power cmd error. type \"on\" or \"off\"."));
        return;
    }

    if(strncmp(arg, "on", 2) == 0){
        set_power(HIGH);
        Serial.println(F("Power on."));
    }else if(strncmp(arg, "off", 3) == 0){
        set_power(LOW);
        Serial.println(F("Power off."));
    }else{
        Serial.println(F("power cmd error. type \"on\" or \"off\"."));
    }
}

void serial_rx_interrupt_nrf()
{
    Serial.println("abort!");
    set_glitcher(false);
    detachInterrupt(SERIAL_RX);
}

void cmd_nrf_set()
{
    char *item = NULL;
    char *value_str = NULL;
    uint32_t value = 0;

    SerialTerminal term = getTermSession();
    item = term.getNext();
    value_str = term.getNext();
    if (item == NULL || value_str == NULL){
        Serial.println(F("set value error. Please type: set <item> <value>"));
        return;
    }

    value = atoi(value_str);
    if (value < 0){
        Serial.println(F("set value must be integer(>=0)."));
        return;
    }

    bool setFlg = false;
    for(Param x: params)
    {
        if(strncmp(item, x.name, 20) == 0){
            *x.var = value;
            Serial.printf("set: %s=%s\n", item, value_str);
            setFlg = true;
            break;
        }
    }
    if(!setFlg){
        Serial.println(F("set item name is invalid. show help."));
    }
}

void cmd_nrf_swd()
{
    bool state;
    uint32_t idcode;
    uint32_t dev_type;

    swd_begin();
    idcode = swd_init();
    dev_type = read_register(0x10000100, 1);   
    state = nrf_read_lock_state();    
    swd_stop();

    Serial.printf("SWD IDCODE: 0x%08x\n", idcode);
    Serial.printf("UICR device type(0x10000100): 0x%08x\n", dev_type);

    if(state == 0){
        Serial.println(F("APPROTECT enabled."));
    }else{
        Serial.println(F("APPEOTECT disabled."));
    }

    if(dev_type != 0){
        Serial.println(F("SWD port unlocked!"));
    }else{
        Serial.println(F("SWD port locked."));
    }
}