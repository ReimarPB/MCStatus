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

build/%.o: %.c $(wildcard *.h)
	echo dirname "$@" | xargs -I'{}' mkdir -p '{}'
	$(CC) -c -o $@ $< $(LIBS)

all: $(OBJS)
	$(CC) -o mcstatus $(OBJS) $(LIBS)

install:
	cp build/mcstatus /usr/bin/mcstatus

clean:
	rm -r build

