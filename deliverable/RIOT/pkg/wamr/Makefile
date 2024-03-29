PKG_NAME=wamr
PKG_URL=https://github.com/bytecodealliance/wasm-micro-runtime.git
ifeq ($(PKG_BLEADING),1)
  PKG_VERSION=main
else
  PKG_VERSION=770ca8c90c8803f934bf681cd29949b7fb5291a3
  #main 2022-01-28
endif
PKG_LICENSE=Apache-2.0

PKG_CUSTOM_PREPARED = CHECK_VERSION

include $(RIOTBASE)/pkg/pkg.mk

#less Wall TODO: get things fixed upstream
CFLAGS :=  $(filter-out -Wcast-align, $(CFLAGS))
CFLAGS :=  $(filter-out -Werror, $(CFLAGS))
CFLAGS += -Wno-format
CFLAGS += -Wno-strict-prototypes
CFLAGS += -Wno-old-style-definition
CFLAGS += -Wno-cast-function-type

#Prepapre includes for cmake
RIOT_INCLUDES = $(filter-out -%,$(subst -I,,$(INCLUDES)))

#translate (CPU_ARCH) to Build Target
#WAMR_BUILD_TARGET is "X86_32" "AARCH64[sub]", "ARM[sub]",
#                     "THUMB[sub]", "MIPS" or "XTENSA"
#no msp430, no AVR support for now

ifeq ($(CPU),native)
ifeq ($(findstring x86,$(OS_ARCH)),x86)
  WAMR_BUILD_TARGET = X86_32
endif
ifeq ($(findstring arm,$(OS_ARCH)),arm)
  WAMR_BUILD_TARGET = ARM
endif
ifeq ($(findstring aarch,$(OS_ARCH)),aarch)
  WAMR_BUILD_TARGET = ARM
endif
else ifeq ($(findstring arm,$(CPU_ARCH)),arm)
  WAMR_BUILD_TARGET = THUMB
else ifeq ($(CPU_ARCH),mips32r2)
  WAMR_BUILD_TARGET = MIPS
else ifeq ($(CPU_ARCH),xtensa)
  WAMR_BUILD_TARGET = XTENSA
else ifeq ($(CPU_ARCH),rv32)
  WAMR_BUILD_TARGET = RISCV32
else
  $(error "WASM is not supported for architecture $(CPU_ARCH)")
endif

ifeq ($(QUIET),0)
  CMAKEMAKEFLAGS += VERBOSE=1
endif

#WAMR_CONFIG will be included into the cmake
ifneq ($(WAMR_CONFIG),)
  WAMR_CMAKE_FLAGS += "-DWAMR_CONFIG=$(WAMR_CONFIG)"
endif

WAMR_CMAKE_FLAGS += "-DRIOT_INCLUDES=$(RIOT_INCLUDES)"\
                     -DWAMR_BUILD_TARGET=$(WAMR_BUILD_TARGET)\
                     -DWAMR_ROOT_DIR=$(PKG_SOURCE_DIR)/ \
                     -DCMAKE_SYSTEM_NAME=Generic \
                     -DCMAKE_SYSTEM_PROCESSOR="$(MCPU)" \
                     -DCMAKE_C_COMPILER=$(CC) \
                     -DCMAKE_C_COMPILER_WORKS=TRUE \
                     #

all: $(BINDIR)/libwamr.a

$(BINDIR)/libwamr.a: $(PKG_BUILD_DIR)/libwamr.a
	cp $< $@

$(PKG_BUILD_DIR)/libwamr.a: $(PKG_BUILD_DIR)/Makefile
	+$(MAKE) -C $(PKG_BUILD_DIR) $(CMAKEMAKEFLAGS)

$(PKG_BUILD_DIR)/Makefile: $(PKG_PREPARED) print_build_target
	ASMFLAGS="${CFLAGS}" cmake -B$(PKG_BUILD_DIR) $(WAMR_CMAKE_FLAGS)

print_build_target:
	@echo PKG_VERSION: $(PKG_VERSION)
	@echo native OS_ARCH: $(OS_ARCH)
	@echo CPU_ARCH: $(CPU_ARCH)
	@echo CPU: $(CPU)
	@echo CFLAGS: $(CFLAGS)
	@echo WAMR_BUILD_TARGET: $(WAMR_BUILD_TARGET)
	@echo WAMR_CONFIG: $(WAMR_CONFIG)
	@echo RIOT_INCLUDES: $(RIOT_INCLUDES)


CHECK_VERSION:
	test -f $(PKG_DOWNLOADED) && (test `cat $(PKG_DOWNLOADED)` = $(PKG_VERSION) || rm $(PKG_DOWNLOADED));true
