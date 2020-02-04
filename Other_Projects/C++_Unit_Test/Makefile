CXX = g++
CXXFLAGS = -std=c++11 -Wall

all: unittest shop


clean:
	rm shop Store.o Item.o TextUI.o

# update to include a test target when you have a test.cpp

shop: main.cpp Store.o Item.o TextUI.o
	$(CXX) $(CXXFLAGS) main.cpp Store.o Item.o TextUI.o -o shop

unittest: test.o Store.o Item.o
	$(CXX) $(CXXFLAGS) Store.o Item.o test.o -o test	

TextUI.o: TextUI.cpp
	$(CXX) $(CXXFLAGS) -c TextUI.cpp

Store.o: Store.cpp
	$(CXX) $(CXXFLAGS) -c Store.cpp

Item.o: Item.cpp
	$(CXX) $(CXXFLAGS) -c Item.cpp

test.o: test.cpp
	$(CXX) $(CXXFLAGS) -c test.cpp
