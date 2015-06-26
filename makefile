
OBJDIR=obj/
SRCDIR=src/
TESTDIR=test/

TESTS = CacheTestDriver.x

MY_CFLAGS = -Iinclude -Wall -Wno-unused-function -fPIC -O2 -g

MY_LIBS = -lpthread -ldl

OBJS= 	$(OBJDIR)SMPCache.o $(OBJDIR)MultiCacheSim.o \
	$(OBJDIR)CacheCore.o $(OBJDIR)Snippets.o \
	$(OBJDIR)nanassert.o $(OBJDIR)CacheMaker.o \
	$(OBJDIR)MESI_SMPCache.o $(OBJDIR)MSI_SMPCache.o

all: $(OBJDIR) $(OBJS) $(TESTS)

$(OBJDIR):
	mkdir -p $(OBJDIR)

%.x: $(TESTDIR)%.cpp $(OBJS)
	g++ $(MY_CFLAGS) -c $(TESTDIR)$*.cpp -o $(OBJDIR)$*.o
	g++  -o $*.x $(OBJS) $(OBJDIR)$*.o $(MY_LIBS)

$(OBJDIR)%.o: $(SRCDIR)%.cpp
	g++ $(MY_CFLAGS) -c $(SRCDIR)$*.cpp -o $(OBJDIR)$*.o

tidy: .FORCE
	rm -f *~

clean: tidy
	rm -f $(OBJDIR)*.o

scrub: clean
	rm -f $(OBJDIR)
	rm -f $(TESTS)

.FORCE:

# print any make var
print-%: ; @echo $*=$($*)

