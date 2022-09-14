PKG_NAME=micropython
PKG_URL=https://github.com/bergzand/micropython
PKG_VERSION=cd7f46297cb5d360625fcdfc9617f12a9b21164e
PKG_LICENSE=MIT

CFLAGS += -Wno-implicit-fallthrough -Wno-unused-parameter -Wno-error

# Micropython build system doesn't support (yet) out-of-source builds
# so clone and build micropython within the application build directory
PKG_BUILD_OUT_OF_SOURCE = 0

include $(RIOTBASE)/pkg/pkg.mk

all:
	@mkdir -p $(PKG_BUILD_DIR)/tmp
	BUILD=$(PKG_BUILD_DIR) "$(MAKE)" -C $(PKG_SOURCE_DIR)/ports/riot
