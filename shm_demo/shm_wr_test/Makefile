CC = gcc
CXX = g++


COMM_DIR = ../src
MAIN_DIR = .
CFLAGS = -c -Wall
CFLAGS += -g -O0
CFLAGS += -std=c++11


CFLAGS += -fpic -lpthread

CFLAGS += -I$(COMM_DIR)
CFLAGS += -I$(MAIN_DIR)
CXXFLAGS = $(CFLAGS)
HEADS += $(wildcard $(MAIN_DIR)/*.h)
SOURCES += $(wildcard $(MAIN_DIR)/*.cpp)
SOURCES += $(wildcard $(COMM_DIR)/*.cpp)


OBJS = $(SOURCES:.cpp=.o)
TARGET = shm_wr



LDFLAGS = -Wl,-R /usr/local/lib -lrt


all:$(TARGET)
$(TARGET):$(OBJS) $(SOURCES) $(HEADS)
	$(CXX) -o $@ $(OBJS) $(LFLAGS) $(LDFLAGS)


$(OBJS):%.o:%.cpp $(HEADS)
	$(CXX) $(CXXFLAGS) $< -o $@
        
clean:
	@rm -f $(OBJS) $(TARGET)

