CCX=g++

CXXFLAGS= -g -std=c++0x


OPENCV= `pkg-config --cflags opencv` `pkg-config --libs opencv`

OBJS = *.cpp
all: ${OBJS}
	$(CCX) $(CXXFLAGS)   -o prueba ${OBJS} $(OPENCV)
