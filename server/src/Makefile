SRCS:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,$(SRCS))
ELF:=ftp_server
CC:=gcc
CFLAGS:=-g -Wall -I /usr/include/mysql/
$(ELF):$(OBJS)
	gcc -o $@ $^ -lpthread  -lmysqlclient -lcrypt
clean:
	rm -rf $(OBJS) $(ELF)
