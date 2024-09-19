CXX=g++
TARGER=hello
OBJ=main.o Cilent.o Server.o 
CXXFALSE=-c -wall
$(TARGER):$(OBJ)
	$(CXX) -o $(TARGER) $(OBJ)

main.o:main.cpp
	$(CXX) -c main.cpp

Cilent.o:Cilent.cpp
	$(CXX) -c Cilent.cpp

Server.o:Server.cpp
	$(CXX) -c Server.cpp