CC = gcc
CFLAGS = -Wall -pedantic -std=c2x -g -fsanitize=address
LDLIBS= -lm
LDFLAGS = -fsanitize=address
ifeq ($(CITY), NYC)
CFLAGS+=-DCITY_NYC
else ifeq ($(CITY), CHI)
CFLAGS+=-DCITY_CHI
else
$(error Debe definir CITY=NYC o CITY=CHI)
endif
TARGET = cityServices$(CITY)
SRC = ./front/cityServices.c \
      ./front/lector.c \
      ./back/queries.c \
      ./front/fromQueryToFile.c \
      ./back/htmlTable.c

OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS) $(LDLIBS)

%.nyc.o: %.c
	$(CC) $(CFLAGS) -DCITY_NYC -c $< -o $@

%.chi.o: %.c
	$(CC) $(CFLAGS) -DCITY_CHI -c $< -o $@

clean:
	rm -f *.o

distclean: clean
	rm -f $(TARGET)
