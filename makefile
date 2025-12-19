CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -g -fsanitize=address
LDFLAGS = -fsanitize=address
LDLIBS = -lm

NYC = cityServicesNYC
CHI = cityServicesCHI

SRC = frontend/cityServices.c \
      frontend/lector.c \
      frontend/fromQueryToFile.c \
      htmlTable.c \
      backend/queries.c

OBJ_NYC = $(SRC:.c=.nyc.o)
OBJ_CHI = $(SRC:.c=.chi.o)

all: nyc chi

nyc: $(NYC)

chi: $(CHI)

$(NYC): $(OBJ_NYC)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(CHI): $(OBJ_CHI)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

%.nyc.o: %.c
	$(CC) $(CFLAGS) -DCITY_NYC -c $< -o $@

%.chi.o: %.c
	$(CC) $(CFLAGS) -DCITY_CHI -c $< -o $@

clean:
	rm -f $(NYC) $(CHI) $(OBJ_NYC) $(OBJ_CHI)