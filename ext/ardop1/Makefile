#	ARDOPC Makefile

OBJS += LinSerial.o KISSModule.o pktARDOP.o pktSession.o BusyDetect.o
OBJS += ALSASound.o ARDOPC.o ardopSampleArrays.o ARQ.o 
OBJS += FFT.o FEC.o HostInterface.o Modulate.o rs.o berlekamp.o
OBJS += galois.o SoundInput.o TCPHostInterface.o SCSHostInterface.o
OBJS += UZ7HOFEC.o

# Configuration:
CFLAGS = -DLINBPQ -MMD -g 
CC = gcc

vpath %.c /mnt/ardopc
vpath %.h /mnt/ardopc
vpath %.o ./
			                       
all: ardopc
			
ardopc: $(OBJS)
	gcc $(OBJS) -Xlinker -Map=output.map -lrt -lm -lpthread -lasound -o ardopc


-include *.d

clean :
	rm -f ardopc $(OBJS) output.map
	rm -f *.d
