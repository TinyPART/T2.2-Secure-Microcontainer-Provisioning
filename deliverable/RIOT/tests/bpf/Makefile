include ../Makefile.tests_common

USEMODULE += embunit
USEMODULE += bpf

USEMODULE += xtimer
USEMODULE += saul
USEMODULE += saul_reg
USEMODULE += saul_default

CFLAGS += -I$(CURDIR)


BLOBS += $(wildcard bpf/*.bin)

include $(RIOTBASE)/Makefile.include
