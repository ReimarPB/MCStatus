OBJS=                             \
	build/mcstatus.o              \
	build/errors.o                \
	build/protocols/java.o        \
	build/protocols/legacy_java.o \
	build/utils/tcp.o             \
	build/utils/chat_parser.o     \
	build/utils/ms.o              \
	build/utils/text_converter.o

LIBS=-lcjson -lcjson_utils

prefix ?= /usr/local
bindir ?= $(prefix)/bin

build/%.o: %.c $(wildcard *.h)
	mkdir -p `dirname $@`
	$(CC) -c -o $@ $< $(LIBS)

all: $(OBJS)
	$(CC) -o mcstatus $(OBJS) $(LIBS)

install:
	cp mcstatus $(bindir)/mcstatus

uninstall:
	rm $(bindir)/mcstatus

clean:
	rm -r build

