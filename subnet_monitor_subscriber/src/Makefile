SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c, %.o, $(SRCS))

CFLAGS := -Wall -pedantic

LD_FLAGS := -lubox -lubus -lblobmsg_json

.PHONY: all clean

all: subnet_monitor_sub

clean:
	rm -f subnet_monitor_sub
	rm -f $(OBJS)

subnet_monitor: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LD_FLAGS) 

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

