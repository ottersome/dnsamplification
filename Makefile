TARGET ?= dns_attack

BUILD_DIR ?= ./build
SRC_DIR ?= ./src
OBJ_DIR ?= ./build

SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))


INC_DIRS := ./include

INC_FLAGS = $(addprefix -I,$(INC_DIRS))
CPPFLAGS ?= $(INC_FLAGS) -MMD -MP -std=c++11


#Objects to executable
$(BUILD_DIR)/$(TARGET): $(OBJ_FILES)
	$(CXX) $(OBJ_FILES) -o $@ $(LDFLAGS)

# Source files

$(BUILD_DIR)/%.o:$(SRC_FILES)
	echo "Th his is the obj file ${SRC}" 
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@
		

.PHONY: clean 
clean:
	rm -r $(BUILD_DIR)/*
