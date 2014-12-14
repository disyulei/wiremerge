# boost
BOOST_I=-I$(BOOST_DIR)
BLIB_I=-I./bLib

CC        =  g++
CXX       =  g++34
CFLAGS    =  -O2  -Wno-unnamed-type-template-args 
#CFLAGS   =  -g -Wno-unnamed-type-template-args

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
	ctags -R *.cpp *.h bLib/*

$(OPATH)%.o:%.cpp
	@$(MKDIR)
	$(CC) $(CFLAGS) -c -o $@ $< $(BOOST_I) $(BLIB_I)

clean:
	rm -f *.o *~ $(EXE) $(OPATH)*
