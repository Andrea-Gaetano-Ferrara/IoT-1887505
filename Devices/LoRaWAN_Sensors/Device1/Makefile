BOARD ?= b-l072z-lrwan1

APPLICATION=sensors

include Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-nano arduino-uno \
                             nucleo-f031k6 nucleo-f042k6 nucleo-l031k6

BOARD_BLACKLIST := msb-430 msb-430h pic32-clicker pic32-wifire \
                   telosb wsn430-v1_3b wsn430-v1_4 z1

LORA_DRIVER ?= sx1276
LORA_REGION ?= EU868

USEPKG += semtech-loramac
USEMODULE += $(LORA_DRIVER)
USEMODULE += hts221
USEMODULE += xtimer

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += fmt

FEATURES_OPTIONAL += periph_eeprom

RIOTBASE ?= $(CURDIR)/../../RIOT_v2

CFLAGS += -DREGION_$(LORA_REGION)
CFLAGS += -DLORAMAC_ACTIVE_REGION=LORAMAC_REGION_$(LORA_REGION)
CFLAGS += -DDISABLE_LORAMAC_DUTYCYCLE

include $(RIOTBASE)/Makefile.include
