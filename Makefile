CFLAGS = -c -g -Wall -Wextra
LINKFLAGS = -lm

SOURCEDIR = src
BUILDDIR = build

EXEC = gc
SOURCES = $(wildcard $(SOURCEDIR)/*.c)
OBJECTS = $(patsubst $(SOURCEDIR)/%.c,$(BUILDDIR)/%.o,$(SOURCES))

$(EXEC): $(OBJECTS)
	$(CC) $^ -o $@ $(LINKFLAGS) 

$(OBJECTS): $(BUILDDIR)/%.o : $(SOURCEDIR)/%.c
	$(CC) $(CFLAGS) -o $@ $<

all: 
	dir $(BUILDDIR)

dir: 
	mkdir -p $(BUILDDIR)

clean: 
	-rm build/*
	-rm $(EXEC)

rebuild:
	make clean
	make

run: 
	make clean
	make
	clear
	./$(EXEC)

asm: $(SOURCES)
	$(CC) -S -o $(EXEC).s $^

shared: $(SOURCES)
	$(CC) -o $(EXEC).o -fPIC $^ 

