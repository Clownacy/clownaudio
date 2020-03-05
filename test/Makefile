ALL_CFLAGS = $(CFLAGS)
ALL_LIBS = $(LIBS)

ifeq ($(STATIC), 1)
  ALL_CFLAGS += $(shell pkg-config clownaudio --cflags --static) -static -DCLOWNAUDIO_STATIC_DEFINE
  ALL_LIBS += $(shell pkg-config clownaudio --libs --static)
else
  ALL_CFLAGS += $(shell pkg-config clownaudio --cflags)
  ALL_LIBS += $(shell pkg-config clownaudio --libs)
endif

test: test.c
	$(CC) $(ALL_CFLAGS) -o $@ $^ $(LDFLAGS) $(ALL_LIBS)
