CXX = g++
CXX_FLAGS = -std=c++17 -I src/
SRC_FILES = src/main.cpp
OUTPUT = nova

all: $(OUTPUT)

$(OUTPUT): $(SRC_FILES)
	$(CXX) $(CXX_FLAGS) $< -o $@

run: $(OUTPUT)
	./$(OUTPUT) && python3 screen.py