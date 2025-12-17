COMPILER=gcc
OUTPUT_FILE=
FRONT=

all: programa clean

programa: $(FRONT) htmlTable.o
	$(COMPILER) -o $(OUTPUT_FILE) $(FRONT) 


htmlTable.o: htmlTable.c
	$(COMPILER) -c htmlTable.c

clean:
	rm -f *.o