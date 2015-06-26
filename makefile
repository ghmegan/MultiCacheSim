
OBJDIR=obj/

#TESTDIR=test/

#TESTS= $(TESTDIR)basic.x
TESTS = CacheTestDriver.x

MY_CFLAGS = -Wall -Wno-unused-function -fPIC -O2 -g

MY_LIBS = -lpthread -ldl

OBJS= 	$(OBJDIR)SMPCache.o $(OBJDIR)MultiCacheSim.o \
	$(OBJDIR)CacheCore.o $(OBJDIR)Snippets.o \
	$(OBJDIR)nanassert.o $(OBJDIR)CacheMaker.o \
	$(OBJDIR)MESI_SMPCache.o $(OBJDIR)MSI_SMPCache.o

all: $(OBJDIR) $(OBJS) $(TESTS)

$(OBJDIR):
	mkdir -p $(OBJDIR)

#$(TESTDIR)%.x: $(TESTDIR)%.cpp $(OBJS)
#	g++ $(MY_CFLAGS) -c $(TESTDIR)$*.cpp -o $(TESTDIR)$*.o
#	g++  -o $(TESTDIR)$*.x $(OBJS) $(TESTDIR)$*.o

%.x: %.cpp $(OBJS)
	g++ $(MY_CFLAGS) -c $*.cpp -o $(OBJDIR)$*.o
	g++  -o $*.x $(OBJS) $(OBJDIR)$*.o $(MY_LIBS)

$(OBJDIR)%.o: %.cpp
	g++ $(MY_CFLAGS) -c $*.cpp -o $(OBJDIR)$*.o

tidy: .FORCE
	rm -f *~

clean: tidy
	rm -f $(OBJDIR)*.o

scrub: clean
	rm -f $(TESTS)

.FORCE:

# print any make var
print-%: ; @echo $*=$($*)

