CXX := g++
CXXFLAGS := -std=c++14 -Wall
SRCS := main.cpp cell.cpp
OBJS := $(SRCS:.cpp=.o)
LIBS := -lncurses
TARGET := myprogram

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET)
