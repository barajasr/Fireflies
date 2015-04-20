target =  app

INC = inc
OBJ = obj
SRC = src

sources = $(wildcard $(SRC)/*.cpp)
objects = $(sources:$(SRC)/%.cpp=$(OBJ)/%.o)

debug =
CXXFLAGS = -std=c++1y -Wall -Wextra -O2 -pedantic -g
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system
ifdef debug
CXXFLAGS += -DDEBUG
endif

all: $(target)

$(target): $(objects)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(OBJ)/Firefly.o: src/Firefly.cpp inc/Firefly.hpp inc/HSL.hpp
	$(CXX) -c $< $(CXXFLAGS) -I$(INC) -o $@

$(OBJ)/HSL.o: src/HSL.cpp inc/HSL.hpp
	$(CXX) -c $< $(CXXFLAGS) -I$(INC) -o $@

$(OBJ)/Main.o: src/Main.cpp inc/Simulation.hpp inc/Firefly.hpp inc/HSL.hpp
	$(CXX) -c $< $(CXXFLAGS) -I$(INC) -o $@

$(OBJ)/Simulation.o: src/Simulation.cpp inc/Simulation.hpp inc/Firefly.hpp inc/HSL.hpp
	$(CXX) -c $< $(CXXFLAGS) -I$(INC) -o $@

.PHONY: clean
clean:
	rm -f $(target) $(objects)
