SRCS = qdbmp.c generator.c
EXE = gen
CFLAGS = -Wall -g
LD = gcc
OBJS = qdbmp.o generator.o

default: compile

compile: $(EXE)

$(EXE): $(OBJS)
	$(LD) $(OBJS) -o $(EXE)

qdbmp.o: qdbmp.c
	$(LD) -c $(CFLAGS) qdbmp.c

generator.o: generator.c
	$(LD) -c $(CFLAGS) generator.c

clean:
	-rm -f $(EXE)
	-rm -f $(OBJS)

