APP_NAME = complete

APP_OBJS = obj/${APP_NAME}.o
INC_DIR = ../../include

# Warnings to be raised by the C compiler
WARNS = -Wall

# Names of tools to use when building
CC = g++

# Compiler flags
CFLAGS = -O3 ${WARNS} -fmessage-length=0 -I${INC_DIR}

# Linker flags
# LDFLAGS = -L../../lib -lqrnglib 
ifeq ($(OS),Windows_NT)
	LDFLAGS = -L../../lib/mingw -lqrnglib 
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
		LDFLAGS = -L../../lib/linux/static -lqrng_vertex 
    endif
endif



.PHONY: all clean

# Build executable by default
all: bin/${APP_NAME}

# Delete all build output
clean:
	if [ -e "bin" ] ; then rm -r "bin"; fi
	if [ -e "obj" ] ; then rm -r "obj"; fi

# Create build output directories if they don't exist
bin obj:
	if [ ! -e "$@" ] ; then mkdir "$@"; fi

# Compile object files for executable
${APP_OBJS}: ${APP_NAME}.cpp ${INC_DIR}/qrng_api.h | obj
	${CC} ${CFLAGS} -c "$<" -o "$@"

# Buld the executable
bin/${APP_NAME}: ${APP_OBJS} | bin
	${CC} -o "$@" ${APP_OBJS} ${LDFLAGS}
	
	
	
	
