/*
 * Copyright (C) 2017 Inria
 *               2017 Inria Chile
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 *
 * @file
 * @brief       Semtech LoRaMAC test application
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Jose Alamos <jose.alamos@inria.cl>
 */

#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "msg.h"
#include "shell.h"
#include "fmt.h"
#include "xtimer.h"

#include "net/loramac.h"
#include "semtech_loramac.h"

int generate_random_temp(void) { //this will generate random number in range l and r
    int l = -50;
    int r = 50;

    int rand_num = (rand() % (r - l + 1)) + l;
    //printf("%dmm/h ", rand_num);
    return rand_num;

}

int generate_random_hum(void) { //this will generate random number in range l and r
    int l = 0;
    int r = 100;

    int rand_num = (rand() % (r - l + 1)) + l;
    //printf("%dmm/h ", rand_num);
    return rand_num;

}

int generate_random_dir(void) { //this will generate random number in range l and r
    int l = 0;
    int r = 360;

    int rand_num = (rand() % (r - l + 1)) + l;
    //printf("%dmm/h ", rand_num);
    return rand_num;

}

int generate_random_int(void) { //this will generate random number in range l and r
    int l = 0;
    int r = 100;

    int rand_num = (rand() % (r - l + 1)) + l;
    //printf("%dmm/h ", rand_num);
    return rand_num;

}

int generate_random_rain(void) { //this will generate random number in range l and r
    int l = 0;
    int r = 50;

    int rand_num = (rand() % (r - l + 1)) + l;
    //printf("%dmm/h ", rand_num);
    return rand_num;
}

int genNextValue(int prevValue, int min, int max){ //generate a new value not too different from the last one.
    int value = prevValue + ((max - min) * (((rand() % 11) - 5))) * 0.003;
    if(value > max) value=max;
    else if (value < min) value=min;
    else value=value;

    return value;
}

semtech_loramac_t loramac;

static const uint8_t deveui[LORAMAC_DEVEUI_LEN] = { 0x00, 0xB8, 0x3D, 0x0E, \
                                                    0x2F, 0xAC, 0x99, 0x80 };
static const uint8_t appeui[LORAMAC_APPEUI_LEN] = { 0x70, 0xB3, 0xD5, 0x7E, \
                                                    0xD0, 0x02, 0xD4, 0xAC };
static const uint8_t appkey[LORAMAC_APPKEY_LEN] = { 0xF9, 0x98, 0x12, 0xF7, \
                                                    0x1B, 0x08, 0x92, 0xBB, \
                                                    0xCD, 0xAB, 0xD5, 0xD5, \
                                                    0x77, 0xAD, 0x63, 0x14 };

/* Application key is 16 bytes long (e.g. 32 hex chars), and thus the longest
   possible size (with application session and network session keys) */
static char print_buf[LORAMAC_APPKEY_LEN * 2 + 1];

static void _loramac_usage(void)
{
    puts("Usage: loramac <get|set|join|tx|link_check"
#ifdef MODULE_PERIPH_EEPROM
         "|save|erase"
#endif
         ">");
}

static void _loramac_join_usage(void)
{
    puts("Usage: loramac join <otaa|abp>");
}

static void _loramac_tx_usage(void)
{
    puts("Usage: loramac tx <payload> [<cnf|uncnf>] [port]");
}

static void _loramac_set_usage(void)
{
    puts("Usage: loramac set <deveui|appeui|appkey|appskey|nwkskey|devaddr|"
         "class|dr|adr|public|netid|tx_power|rx2_freq|rx2_dr> <value>");
}

static void _loramac_get_usage(void)
{
    puts("Usage: loramac get <deveui|appeui|appkey|appskey|nwkskey|devaddr|"
         "class|dr|adr|public|netid|tx_power|rx2_freq|rx2_dr>");
}

static int _cmd_loramac(int argc, char **argv)
{
    if (argc < 2) {
        _loramac_usage();
        return 1;
    }

    if (strcmp(argv[1], "get") == 0) {
        if (argc < 3) {
            _loramac_get_usage();
            return 1;
        }

        if (strcmp("deveui", argv[2]) == 0) {
            uint8_t deveui[LORAMAC_DEVEUI_LEN];
            semtech_loramac_get_deveui(&loramac, deveui);
            fmt_bytes_hex(print_buf, deveui, LORAMAC_DEVEUI_LEN);
            print_buf[LORAMAC_DEVEUI_LEN * 2] = '\0';
            printf("DEVEUI: %s\n", print_buf);
        }
        else if (strcmp("appeui", argv[2]) == 0) {
            uint8_t appeui[LORAMAC_APPEUI_LEN];
            semtech_loramac_get_appeui(&loramac, appeui);
            fmt_bytes_hex(print_buf, appeui, LORAMAC_APPEUI_LEN);
            print_buf[LORAMAC_APPEUI_LEN * 2] = '\0';
            printf("APPEUI: %s\n", print_buf);
        }
        else if (strcmp("appkey", argv[2]) == 0) {
            uint8_t appkey[LORAMAC_APPKEY_LEN];
            semtech_loramac_get_appkey(&loramac, appkey);
            fmt_bytes_hex(print_buf, appkey, LORAMAC_APPKEY_LEN);
            print_buf[LORAMAC_APPKEY_LEN * 2] = '\0';
            printf("APPKEY: %s\n", print_buf);
        }
        else if (strcmp("appskey", argv[2]) == 0) {
            uint8_t appskey[LORAMAC_APPSKEY_LEN];
            semtech_loramac_get_appskey(&loramac, appskey);
            fmt_bytes_hex(print_buf, appskey, LORAMAC_APPSKEY_LEN);
            print_buf[LORAMAC_APPSKEY_LEN * 2] = '\0';
            printf("APPSKEY: %s\n", print_buf);
        }
        else if (strcmp("nwkskey", argv[2]) == 0) {
            uint8_t nwkskey[LORAMAC_NWKSKEY_LEN];
            semtech_loramac_get_nwkskey(&loramac, nwkskey);
            fmt_bytes_hex(print_buf, nwkskey, LORAMAC_NWKSKEY_LEN);
            print_buf[LORAMAC_NWKSKEY_LEN * 2] = '\0';
            printf("NWKSKEY: %s\n", print_buf);
        }
        else if (strcmp("devaddr", argv[2]) == 0) {
            uint8_t devaddr[LORAMAC_DEVADDR_LEN];
            semtech_loramac_get_devaddr(&loramac, devaddr);
            fmt_bytes_hex(print_buf, devaddr, LORAMAC_DEVADDR_LEN);
            print_buf[LORAMAC_DEVADDR_LEN * 2] = '\0';
            printf("DEVADDR: %s\n", print_buf);
        }
        else if (strcmp("class", argv[2]) == 0) {
            printf("Device class: ");
            switch(semtech_loramac_get_class(&loramac)) {
                case LORAMAC_CLASS_A:
                    puts("A");
                    break;
                case LORAMAC_CLASS_B:
                    puts("B");
                    break;
                case LORAMAC_CLASS_C:
                    puts("C");
                    break;
                default:
                    puts("Invalid");
                    break;
            }
        }
        else if (strcmp("dr", argv[2]) == 0) {
            printf("DATARATE: %d\n",
                   semtech_loramac_get_dr(&loramac));
        }
        else if (strcmp("adr", argv[2]) == 0) {
            printf("ADR: %s\n",
                   semtech_loramac_get_adr(&loramac) ? "on" : "off");
        }
        else if (strcmp("public", argv[2]) == 0) {
            printf("Public network: %s\n",
                   semtech_loramac_get_public_network(&loramac) ? "on" : "off");
        }
        else if (strcmp("netid", argv[2]) == 0) {
            printf("NetID: %" PRIu32 "\n", semtech_loramac_get_netid(&loramac));
        }
        else if (strcmp("tx_power", argv[2]) == 0) {
            printf("TX power index: %d\n", semtech_loramac_get_tx_power(&loramac));
        }
        else if (strcmp("rx2_freq", argv[2]) == 0) {
            printf("RX2 freq: %" PRIu32 "\n", semtech_loramac_get_rx2_freq(&loramac));
        }
        else if (strcmp("rx2_dr", argv[2]) == 0) {
            printf("RX2 dr: %d\n", semtech_loramac_get_rx2_dr(&loramac));
        }
        else {
            _loramac_get_usage();
            return 1;
        }
    }
    else if (strcmp(argv[1], "set") == 0) {
        if (argc < 3) {
            _loramac_set_usage();
            return 1;
        }

        if (strcmp("deveui", argv[2]) == 0) {
            if ((argc < 4) || (strlen(argv[3]) != LORAMAC_DEVEUI_LEN * 2)) {
                puts("Usage: loramac set deveui <16 hex chars>");
                return 1;
            }
            uint8_t deveui[LORAMAC_DEVEUI_LEN];
            fmt_hex_bytes(deveui, argv[3]);
            semtech_loramac_set_deveui(&loramac, deveui);
        }
        else if (strcmp("appeui", argv[2]) == 0) {
            if ((argc < 4) || (strlen(argv[3]) != LORAMAC_APPEUI_LEN * 2)) {
                puts("Usage: loramac set appeui <16 hex chars>");
                return 1;
            }
            uint8_t appeui[LORAMAC_APPEUI_LEN];
            fmt_hex_bytes(appeui, argv[3]);
            semtech_loramac_set_appeui(&loramac, appeui);
        }
        else if (strcmp("appkey", argv[2]) == 0) {
            if ((argc < 4) || (strlen(argv[3]) != LORAMAC_APPKEY_LEN * 2)) {
                puts("Usage: loramac set appkey <32 hex chars>");
                return 1;
            }
            uint8_t appkey[LORAMAC_APPKEY_LEN];
            fmt_hex_bytes(appkey, argv[3]);
            semtech_loramac_set_appkey(&loramac, appkey);
        }
        else if (strcmp("appskey", argv[2]) == 0) {
            if ((argc < 4) || (strlen(argv[3]) != LORAMAC_APPSKEY_LEN * 2)) {
                puts("Usage: loramac set appskey <32 hex chars>");
                return 1;
            }
            uint8_t appskey[LORAMAC_APPSKEY_LEN];
            fmt_hex_bytes(appskey, argv[3]);
            semtech_loramac_set_appskey(&loramac, appskey);
        }
        else if (strcmp("nwkskey", argv[2]) == 0) {
            if ((argc < 4) || (strlen(argv[3]) != LORAMAC_NWKSKEY_LEN * 2)) {
                puts("Usage: loramac set nwkskey <32 hex chars>");
                return 1;
            }
            uint8_t nwkskey[LORAMAC_NWKSKEY_LEN];
            fmt_hex_bytes(nwkskey, argv[3]);
            semtech_loramac_set_nwkskey(&loramac, nwkskey);
        }
        else if (strcmp("devaddr", argv[2]) == 0) {
            if ((argc < 4) || (strlen(argv[3]) != LORAMAC_DEVADDR_LEN * 2)) {
                puts("Usage: loramac set devaddr <8 hex chars>");
                return 1;
            }
            uint8_t devaddr[LORAMAC_DEVADDR_LEN];
            fmt_hex_bytes(devaddr, argv[3]);
            semtech_loramac_set_devaddr(&loramac, devaddr);
        }
        else if (strcmp("class", argv[2]) == 0) {
            if (argc < 4) {
                puts("Usage: loramac set class <A,B,C>");
                return 1;
            }
            loramac_class_t cls;
            if (strcmp(argv[3], "A") == 0) {
                cls = LORAMAC_CLASS_A;
            }
            else if (strcmp(argv[3], "B") == 0) {
                cls = LORAMAC_CLASS_B;
            }
            else if (strcmp(argv[3], "C") == 0) {
                cls = LORAMAC_CLASS_C;
            }
            else {
                puts("Usage: loramac set class <A,B,C>");
                return 1;
            }
            semtech_loramac_set_class(&loramac, cls);
        }
        else if (strcmp("dr", argv[2]) == 0) {
            if (argc < 4) {
                puts("Usage: loramac set dr <0..16>");
                return 1;
            }
            uint8_t dr = atoi(argv[3]);
            if (dr > LORAMAC_DR_15) {
                puts("Usage: loramac set dr <0..16>");
                return 1;
            }
            semtech_loramac_set_dr(&loramac, dr);
        }
        else if (strcmp("adr", argv[2]) == 0) {
            if (argc < 4) {
                puts("Usage: loramac set adr <on|off>");
                return 1;
            }
            bool adr;
            if (strcmp("on", argv[3]) == 0) {
                adr = true;
            }
            else if (strcmp("off", argv[3]) == 0) {
                adr = false;
            }
            else {
                puts("Usage: loramac set adr <on|off>");
                return 1;
            }
            semtech_loramac_set_adr(&loramac, adr);
        }
        else if (strcmp("public", argv[2]) == 0) {
            if (argc < 4) {
                puts("Usage: loramac set public <on|off>");
                return 1;
            }
            bool public;
            if (strcmp("on", argv[3]) == 0) {
                public = true;
            }
            else if (strcmp("off", argv[3]) == 0) {
                public = false;
            }
            else {
                puts("Usage: loramac set public <on|off>");
                return 1;
            }
            semtech_loramac_set_public_network(&loramac, public);
        }
        else if (strcmp("netid", argv[2]) == 0) {
            if (argc < 4) {
                puts("Usage: loramac set netid <integer value>");
                return 1;
            }
            semtech_loramac_set_netid(&loramac, strtoul(argv[2], NULL, 0));
        }
        else if (strcmp("tx_power", argv[2]) == 0) {
            if (argc < 4) {
                puts("Usage: loramac set tx_power <0..16>");
                return 1;
            }
            uint8_t power = atoi(argv[3]);
            if (power > LORAMAC_TX_PWR_15) {
                puts("Usage: loramac set tx_power <0..16>");
                return 1;
            }
            semtech_loramac_set_tx_power(&loramac, power);
        }
        else if (strcmp("rx2_freq", argv[2]) == 0) {
            if (argc < 4) {
                puts("Usage: loramac set rx2_freq <frequency>");
                return 1;
            }
            uint32_t freq = atoi(argv[3]);
            semtech_loramac_set_rx2_freq(&loramac, freq);
        }
        else if (strcmp("rx2_dr", argv[2]) == 0) {
            if (argc < 4) {
                puts("Usage: loramac set rx2_dr <0..16>");
                return 1;
            }
            uint8_t dr = atoi(argv[3]);
            if (dr > LORAMAC_DR_15){
                puts("Usage: loramac set rx2_dr <0..16>");
                return 1;
            }
            semtech_loramac_set_rx2_dr(&loramac, dr);
        }
        else {
            _loramac_set_usage();
            return 1;
        }
    }
    else if (strcmp(argv[1], "join") == 0) {
        if (argc < 3) {
            _loramac_join_usage();
            return 1;
        }

        uint8_t join_type;
        if (strcmp(argv[2], "otaa") == 0) {
            join_type = LORAMAC_JOIN_OTAA;
        }
        else if (strcmp(argv[2], "abp") == 0) {
            join_type = LORAMAC_JOIN_ABP;
        }
        else {
            _loramac_join_usage();
            return 1;
        }

        switch (semtech_loramac_join(&loramac, join_type)) {
            case SEMTECH_LORAMAC_DUTYCYCLE_RESTRICTED:
                puts("Cannot join: dutycycle restriction");
                return 1;
            case SEMTECH_LORAMAC_BUSY:
                puts("Cannot join: mac is busy");
                return 1;
            case SEMTECH_LORAMAC_JOIN_FAILED:
                puts("Join procedure failed!");
                return 1;
            case SEMTECH_LORAMAC_ALREADY_JOINED:
                puts("Warning: already joined!");
                return 1;
            case SEMTECH_LORAMAC_JOIN_SUCCEEDED:
                puts("Join procedure succeeded!");
                break;
            default: /* should not happen */
                break;
        }
        return 0;
    }
    else if (strcmp(argv[1], "tx") == 0) {
        if (argc < 3) {
            _loramac_tx_usage();
            return 1;
        }

        uint8_t cnf = LORAMAC_DEFAULT_TX_MODE;  /* Default: confirmable */
        uint8_t port = LORAMAC_DEFAULT_TX_PORT; /* Default: 2 */
        /* handle optional parameters */
        if (argc > 3) {
            if (strcmp(argv[3], "cnf") == 0) {
                cnf = LORAMAC_TX_CNF;
            }
            else if (strcmp(argv[3], "uncnf") == 0) {
                cnf = LORAMAC_TX_UNCNF;
            }
            else {
                _loramac_tx_usage();
                return 1;
            }

            if (argc > 4) {
                port = atoi(argv[4]);
                if (port == 0 || port >= 224) {
                    printf("error: invalid port given '%d', "
                           "port can only be between 1 and 223\n", port);
                    return 1;
                }
            }
        }

        semtech_loramac_set_tx_mode(&loramac, cnf);
        semtech_loramac_set_tx_port(&loramac, port);

        switch (semtech_loramac_send(&loramac,
                                     (uint8_t *)argv[2], strlen(argv[2]))) {
            case SEMTECH_LORAMAC_NOT_JOINED:
                puts("Cannot send: not joined");
                return 1;

            case SEMTECH_LORAMAC_DUTYCYCLE_RESTRICTED:
                puts("Cannot send: dutycycle restriction");
                return 1;

            case SEMTECH_LORAMAC_BUSY:
                puts("Cannot send: MAC is busy");
                return 1;

            case SEMTECH_LORAMAC_TX_ERROR:
                puts("Cannot send: error");
                return 1;
        }

        /* wait for receive windows */
        switch (semtech_loramac_recv(&loramac)) {
            case SEMTECH_LORAMAC_DATA_RECEIVED:
                loramac.rx_data.payload[loramac.rx_data.payload_len] = 0;
                printf("Data received: %s, port: %d\n",
                       (char *)loramac.rx_data.payload, loramac.rx_data.port);
                break;

            case SEMTECH_LORAMAC_DUTYCYCLE_RESTRICTED:
                puts("Cannot send: dutycycle restriction");
                return 1;

            case SEMTECH_LORAMAC_BUSY:
                puts("Cannot send: MAC is busy");
                return 1;

            case SEMTECH_LORAMAC_TX_ERROR:
                puts("Cannot send: error");
                return 1;

            case SEMTECH_LORAMAC_TX_DONE:
                puts("TX complete, no data received");
                break;
        }

        if (loramac.link_chk.available) {
            printf("Link check information:\n"
                   "  - Demodulation margin: %d\n"
                   "  - Number of gateways: %d\n",
                   loramac.link_chk.demod_margin,
                   loramac.link_chk.nb_gateways);
        }

        return 0;
    }
    else if (strcmp(argv[1], "loop") == 0) {

        srand(time(0));

        //initializing the random values
        int temp = generate_random_temp();
        int hum = generate_random_hum();
        int dir = generate_random_dir();
        int inte = generate_random_int();
        int rain = generate_random_rain();
        int device = 1;

        while (true)
        {

            //generating new values
            int new_temp = genNextValue(temp, -50, 50);
            int new_hum = genNextValue(hum, 0, 100);
            int new_dir = genNextValue(dir, 0, 360);
            int new_inte = genNextValue(inte, 0, 100);
            int new_rain = genNextValue(rain, 0, 50);
            /*printf("%d° \t", new_temp);
            printf("%d%% \t", new_hum);
            printf("%d° \t", new_dir);
            printf("%dm/s \t", new_inte);
            printf("%dmm/h \n", new_rain);
            printf("%d° \t", temp);
            printf("%d%% \t", hum);
            printf("%d° \t", dir);
            printf("%dm/s \t", inte);
            printf("%dmm/h \n", rain);*/

            //store the values in a variable so we can pass it in the publish

            char argomento[200];// = "{\"device\": 5.2,\"temperature\": 33, \"humidity\": 22}";
            sprintf(argomento, "{\"device\": \"%d\", \"temperature\": \"%d\", \"humidity\": \"%d\", \"windDirection\": \"%d\", \"windIntensity\": \"%d\", \"rainHeight\": \"%d\"}", device, new_temp, new_hum, new_dir, new_inte, new_rain); //creating the argument to pass it in the tx

            uint8_t cnf = LORAMAC_DEFAULT_TX_MODE;  /* Default: confirmable */
            uint8_t port = LORAMAC_DEFAULT_TX_PORT; /* Default: 2 */
            /* handle optional parameters */
            if (argc > 3) {
                if (strcmp(argv[3], "cnf") == 0) {
                    cnf = LORAMAC_TX_CNF;
                }
                else if (strcmp(argv[3], "uncnf") == 0) {
                    cnf = LORAMAC_TX_UNCNF;
                }
                else {
                    _loramac_tx_usage();
                    return 1;
                }

                if (argc > 4) {
                    port = atoi(argv[4]);
                    if (port == 0 || port >= 224) {
                        printf("error: invalid port given '%d', "
                            "port can only be between 1 and 223\n", port);
                        return 1;
                    }
                }
            }

            semtech_loramac_set_tx_mode(&loramac, cnf);
            semtech_loramac_set_tx_port(&loramac, port);

            switch (semtech_loramac_send(&loramac,
                                        (uint8_t *)argomento, strlen(argomento))) {
                case SEMTECH_LORAMAC_NOT_JOINED:
                    puts("Cannot send: not joined");
                    return 1;

                case SEMTECH_LORAMAC_DUTYCYCLE_RESTRICTED:
                    puts("Cannot send: dutycycle restriction");
                    return 1;

                case SEMTECH_LORAMAC_BUSY:
                    puts("Cannot send: MAC is busy");
                    return 1;

                case SEMTECH_LORAMAC_TX_ERROR:
                    puts("Cannot send: error");
                    return 1;
            }

            /* wait for receive windows */
            switch (semtech_loramac_recv(&loramac)) {
                case SEMTECH_LORAMAC_DATA_RECEIVED:
                    loramac.rx_data.payload[loramac.rx_data.payload_len] = 0;
                    printf("Data received: %s, port: %d\n",
                        (char *)loramac.rx_data.payload, loramac.rx_data.port);
                    break;

                case SEMTECH_LORAMAC_DUTYCYCLE_RESTRICTED:
                    puts("Cannot send: dutycycle restriction");
                    return 1;

                case SEMTECH_LORAMAC_BUSY:
                    puts("Cannot send: MAC is busy");
                    return 1;

                case SEMTECH_LORAMAC_TX_ERROR:
                    puts("Cannot send: error");
                    return 1;

                case SEMTECH_LORAMAC_TX_DONE:
                    puts("TX complete, no data received");
                    break;
            }

            if (loramac.link_chk.available) {
                printf("Link check information:\n"
                    "  - Demodulation margin: %d\n"
                    "  - Number of gateways: %d\n",
                    loramac.link_chk.demod_margin,
                    loramac.link_chk.nb_gateways);
            }

            xtimer_sleep(5);
        }
        return 0;
        
    }
    else if (strcmp(argv[1], "link_check") == 0) {
        if (argc > 2) {
            _loramac_usage();
            return 1;
        }

        semtech_loramac_request_link_check(&loramac);
        puts("Link check request scheduled");
    }
#ifdef MODULE_PERIPH_EEPROM
    else if (strcmp(argv[1], "save") == 0) {
        if (argc > 2) {
            _loramac_usage();
            return 1;
        }

        semtech_loramac_save_config(&loramac);
    }
    else if (strcmp(argv[1], "erase") == 0) {
        if (argc > 2) {
            _loramac_usage();
            return 1;
        }

        semtech_loramac_erase_config();
    }
#endif
    else {
        _loramac_usage();
        return 1;
    }

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "loramac", "control the loramac stack", _cmd_loramac },
    { NULL, NULL, NULL }
};

int main(void)
{
    /* 1. initialize the LoRaMAC MAC layer */
    semtech_loramac_init(&loramac);
    //set the data rate
    semtech_loramac_set_dr(&loramac, 5);
    /* 2. set the keys identifying the device */
    semtech_loramac_set_deveui(&loramac, deveui);
    semtech_loramac_set_appeui(&loramac, appeui);
    semtech_loramac_set_appkey(&loramac, appkey);

    /* 3. join the network */
    if (semtech_loramac_join(&loramac, LORAMAC_JOIN_OTAA) != SEMTECH_LORAMAC_JOIN_SUCCEEDED) {
        puts("Join procedure failed");
        return 1;
    }
    puts("Join procedure succeeded");

    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
}
