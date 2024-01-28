CFLAGS = -c -g -Wall -Wextra
LINKFLAGS = -lm

SOURCEDIR = src
BUILDDIR = build
STATICDIR = build/static

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
	-rm $(BUILDDIR)/*.o
	-rm *.o
	-rm $(STATICDIR)/*
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
	$(CC) -S -o $(BUILDDIR)/$(EXEC).s $^

shared: $(SOURCES)
	$(CC) -o $(BUILDDIR)/$(EXEC).o -fPIC $^ 

static: $(SOURCES)
	$(CC) -c -o $(STATICDIR)/lib$(EXEC).o $^
	ar -rcs $(STATICDIR)/lib$(EXEC).a $(STATICDIR)/lib$(EXEC).o
