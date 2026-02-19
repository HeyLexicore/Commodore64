# Compiler
CXX := g++

# Executable name
TARGET :=out 
# Source files
SRC := Main.cpp Bus.cpp Cpu.cpp Opcodes.cpp

# Compiler flags
CXXFLAGS := 
# Linker flags
LDFLAGS := 
# Build target
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Clean build artifacts
edit:
	nvim Main.cpp Bus.cpp Cpu.cpp Opcodes.cpp

clean:
	rm -f $(TARGET)

.PHONY: all clean

