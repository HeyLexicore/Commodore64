# Compiler
CXX := g++

# Executable name
TARGET :=out 
# Source files
SRC := Main.cpp Bus.cpp Vic.cpp Cpu.cpp Opcodes.cpp

# Compiler flags
CXXFLAGS := 
# Linker flags
LDFLAGS := 
# Build target
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Clean build artifacts
run:
	make
	./out kernal.901227-02.bin basic.901226-01.bin -o 
edit:
	nvim Main.cpp *.cpp *.h 
clean:
	rm -f $(TARGET)

.PHONY: all clean

