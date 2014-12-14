LIB_I   =  -I./bLib  -I$(BOOST_DIR)
ifeq ($(shell uname), Darwin)
LIB_L   =  -L./limbo -lgdsparser_mac
else
LIB_L   =  -L./limbo -lgdsparser_linux
endif

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
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIB_L) 
	ctags -R *.cpp *.h bLib/*

$(OPATH)%.o:%.cpp
	@$(MKDIR)
	$(CC) $(CFLAGS) -c -o $@ $< $(LIB_I)

clean:
	rm -f *.o *~ $(EXE) $(OPATH)*
