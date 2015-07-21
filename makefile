
OBJDIR=obj/
SRCDIR=src/
TESTDIR=test/
LIBDIR=lib/

TESTS = CacheTestDriver.x

MY_CFLAGS = -Iinclude -Wall -Wno-unused-function -fPIC -O2 -g

LIBMCS = libmcaches.a 

MY_LIBS = -lpthread -ldl -L$(LIBDIR) -lmcaches

OBJS= 	$(OBJDIR)SMPCache.o $(OBJDIR)MultiCacheSim.o \
	$(OBJDIR)CacheCore.o $(OBJDIR)Snippets.o \
	$(OBJDIR)nanassert.o $(OBJDIR)CacheMaker.o \
	$(OBJDIR)MESI_SMPCache.o $(OBJDIR)MSI_SMPCache.o

all: $(OBJDIR) $(OBJS) $(TESTS)

$(LIBDIR):
	mkdir -p $@

$(OBJDIR):
	mkdir -p $@

$(LIBMCS): $(LIBDIR) $(OBJS)
	ar rcs $(LIBDIR)$@ $(OBJS)

%.x: $(TESTDIR)%.cpp $(LIBMCS)
	g++ $(MY_CFLAGS) -c $(TESTDIR)$*.cpp -o $(OBJDIR)$*.o
	g++  -o $*.x $(OBJDIR)$*.o $(MY_LIBS)

$(OBJDIR)%.o: $(SRCDIR)%.cpp
	g++ $(MY_CFLAGS) -c $(SRCDIR)$*.cpp -o $(OBJDIR)$*.o

tidy: .FORCE
	rm -f *~

clean: tidy
	rm -f $(OBJDIR)*.o

scrub: clean
	rm -rf $(OBJDIR)
	rm -rf $(LIBDIR)
	rm -f $(TESTS)

.FORCE:

# print any make var
print-%: ; @echo $*=$($*)

