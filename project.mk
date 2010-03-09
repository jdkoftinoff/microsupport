#
# defintions
#
PROJECT=microsupport
PROJECT_NAME=microsupport
PROJECT_VERSION=20100216
PROJECT_EMAIL=jeffk@meyersound.com
PROJECT_LICENSE=private
PROJECT_MAINTAINER=jeffk@meyersound.com
PROJECT_COPYRIGHT=Copyright 2010
PROJECT_DESCRIPTION=microsupport
PROJECT_WEBSITE=https://github.com/jeffk-meyersound/microsupport
PROJECT_IDENTIFIER=com.meyersound.microsupport
TOP_LIB_DIRS+=.
CONFIG_TOOLS+=
PKGCONFIG_PACKAGES+=
COMPILE_FLAGS+=-Wall

ENABLE_PRECOMPILED_HEADERS?=0
PRECOMPILED_HEADER=$(TOP)/include/us_world.h

STRIP=true

LIB_SRC_DIR+=
LIB_EXAMPLES_DIR+=

