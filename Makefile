CXX = g++
CVS = -std=c++20
CXXFLAGS = -I$(shell brew --prefix)/include -I$(INCLUDE_DIR) -L$(shell brew --prefix)/lib
LIBS = -framework OpenGL -framework GLUT

SRC_DIR = src
INCLUDE_DIR = include

TARGET = main
SRCS = $(SRC_DIR)/main.cpp

$(TARGET): $(SRCS)
	$(CXX) $(CVS) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET) $(OBJS)

run: $(TARGET)
	./$(TARGET)
