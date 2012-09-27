CXXFLAGS = -std=c++0x -ggdb3 -Wall #-O3 -fno-inline
CPPFLAGS = -MD -I../mlsmoke

SMOKES += -lsmokephonon
SMOKES += -lsmokeqimageblitz
SMOKES += -lsmokeqsci
SMOKES += -lsmokeqt3support
SMOKES += -lsmokeqtcore
SMOKES += -lsmokeqtdbus
SMOKES += -lsmokeqtdeclarative
SMOKES += -lsmokeqtgui
SMOKES += -lsmokeqthelp
SMOKES += -lsmokeqtnetwork
SMOKES += -lsmokeqtopengl
SMOKES += -lsmokeqtscript
SMOKES += -lsmokeqtsql
SMOKES += -lsmokeqtsvg
SMOKES += -lsmokeqttest
SMOKES += -lsmokeqtuitools
SMOKES += -lsmokeqtwebkit
SMOKES += -lsmokeqtxmlpatterns
SMOKES += -lsmokeqtxml


build: mlsmokegen
	./mlsmokegen

mlsmokegen: $(patsubst %.cpp,%.o,$(wildcard *.cpp))
	$(LINK.cpp) $^ -o $@ -lsmokebase $(SMOKES)

clean:
	$(RM) *.o *.d mlsmokegen

-include *.d
