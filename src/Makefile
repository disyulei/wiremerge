# boost
BOOST_I=-I$(BOOST_HOME)

CC        =  g++
CXX       =  g++34
CFLAGS    =  -funroll-all-loops -O2 -m64 -fPIC -Wno-deprecated 
#CFLAGS   =  -g -Wno-deprecated 
CCLNFLAGS = 

MKDIR = if [ ! -d $(@D) ]; then mkdir $(@D); fi
BPATH = ../build/
OPATH = ../obj/
SRCS	= $(wildcard *.cpp)
OBJS = $(SRCS:%.cpp=$(OPATH)%.o)

EXE	= $(BPATH)wireMerge

.SUFFIXES: .cc .cpp .o .x

$(EXE): $(OBJS)
	@$(MKDIR)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(CCLNFLAGS) 

$(OPATH)%.o:%.cpp
	@$(MKDIR)
	$(CC) $(CFLAGS) -c -o $@ $< $(BOOST_I)

clean:
	rm -f *.o *~ $(EXE) $(OPATH)*
