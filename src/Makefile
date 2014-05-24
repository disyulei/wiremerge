#**************************************************************************#
#                         Special Library
#**************************************************************************#
# openaccess
OA_I = -I/home/polaris/bei/app/openaccessv2/include/oa
#OA_L = -L../build/oalib -loaCommon -loaBase -loaDM -loaTech -loaDesign -loaWafer -loaPlugIn -ldl
OA_L = -L../build/libOA -loaCommon -loaBase -loaDM -loaTech -loaDesign -loaWafer -loaPlugIn -ldl
#OA_I = 
#OA_L = 

# boost
BOOST_I = -I/home/usr1/bei/boost_1_49_0
#BOOST_I = 


#**************************************************************************#
#                         Standard Setting
#**************************************************************************#
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
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(OA_L) $(CCLNFLAGS) 

$(OPATH)%.o:%.cpp
	@$(MKDIR)
	$(CC) $(CFLAGS) -c -o $@ $< $(OA_I) $(BOOST_I)

clean:
	rm -f *.o *~ $(EXE) $(OPATH)*
