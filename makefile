###############################################
# The makefile is desinged for the testbe of functions
# Contact us: SSurui, ssurui@gmail.com; Jerod.yan, jerod.yan@gmail.com
# #############################################

PROJECT=hybrid_video_mixer_server

#PLATFORM
#ifdef PLATFORM_A
#	CC=$(XXXX)/bin/xxxx-cc
#	RESULT_DIR=../lib_dir_xxxxcc/
#else (X86, GCC)
	CC=gcc
	RESULT_DIR=./lib_dir_gcc/
#endif

#RELEASE VERSION CONTROL
ifdef REL
	CFLAGS=-O2 -std=gnu99 
else
	CFLAGS=-g -Wall -std=gnu99 -DDEBUG -gdwarf-2 -g3
endif

#PACKETAGE TOOL
AR=ar

INC_DIR_FLAGS=-I. \
			-I./libavscheme \
			-I./liblogerrors \
			-I./libnetwork 

LIB_OPTIONS=-L./ -lpthread -lm \
			-L$(RESULT_DIR) \
			-lavscheme \
			-lvcc \
			-lnetwork \
			-llogerrors
TARGET=hvs
OBJS=hvs_main.o 
$(TARGET):$(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LIB_OPTIONS)

$(OBJS):%.o:%.c
	$(CC) -c $(CFLAGS) $(INC_DIR_FLAGS) $< -o $@

.PHONY:clean
clean:
	-rm -frv $(TARGET) $(OBJS)

.PHONY:pp
pp:
	-find ./ -name "*" -print |egrep '\.cpp$$|\.c$$|\.h$$' |xargs astyle --style=linux -s -n; touch *.h *.c
