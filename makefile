CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -fsanitize=address -g

INCLUDES = -Ifrontend -Ibackend -Ifrontend/CTable

SRC = \
	frontend/main.c \
	frontend/lector.c \
	frontend/writer.c \
	frontend/CTable/ctable.c \
	backend/cityData.c

all: cityServicesNYC cityServicesCHI

cityServicesNYC:
	$(CC) $(CFLAGS) $(INCLUDES) -DCITY_NYC $(SRC) -o cityServicesNYC

cityServicesCHI:
	$(CC) $(CFLAGS) $(INCLUDES) -DCITY_CHICAGO $(SRC) -o cityServicesCHI

clean:
	rm -f cityServicesNYC cityServicesCHI