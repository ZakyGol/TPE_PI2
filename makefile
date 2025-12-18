CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -g -fsanitize=address
LDFLAGS = -fsanitize=address
LDLIBS = -lm

NYC = cityServicesNYC
CHI = cityServicesCHI

SRC = main.c \
      lector.c \
      fromQueryToFile.c \
      queries.c \
      htmlTable.c

OBJ_NYC = $(SRC:.c=.nyc.o)
OBJ_CHI = $(SRC:.c=.chi.o)

all: nyc chicago

nyc: $(NYC)

chicago: $(CHI)

$(NYC): $(OBJ_NYC)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(CHI): $(OBJ_CHI)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

# ✅ REGLAS EXPLÍCITAS (la clave)
frontend/%.nyc.o: frontend/%.c
	$(CC) $(CFLAGS) -DCITY_NYC -c $< -o $@

backend/%.nyc.o: backend/%.c
	$(CC) $(CFLAGS) -DCITY_NYC -c $< -o $@

%.nyc.o: %.c
	$(CC) $(CFLAGS) -DCITY_NYC -c $< -o $@

frontend/%.chi.o: frontend/%.c
	$(CC) $(CFLAGS) -DCITY_CHICAGO -c $< -o $@

backend/%.chi.o: backend/%.c
	$(CC) $(CFLAGS) -DCITY_CHICAGO -c $< -o $@

%.chi.o: %.c
	$(CC) $(CFLAGS) -DCITY_CHICAGO -c $< -o $@

clean:
	rm -f $(NYC) $(CHI) $(OBJ_NYC) $(OBJ_CHI)