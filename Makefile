CFLAGS = -c -g -Wall -Wextra

SOURCEDIR = src
BUILDDIR = build

EXEC = gc
SOURCES = $(wildcard $(SOURCEDIR)/*.c)
OBJECTS = $(patsubst $(SOURCEDIR)/%.c,$(BUILDDIR)/%.o,$(SOURCES))

$(EXEC): $(OBJECTS)
	$(CC) $^ -o $@ $(SDL2FLAGS) 

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
