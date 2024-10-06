.PHONY: target

Headers := stack_hcpp/stack.h
Objects := obj/main.o obj/stack.o

a.out: $(Objects) $(Headers)
	g++ $(Objects)

obj/main.o: main.cpp $(Headers)
	g++ -c main.cpp -o obj/main.o

obj/stack.o: stack_hcpp/stack.cpp $(Headers)
	g++ -c stack_hcpp/stack.cpp -o obj/stack.o

target:
	mkdir obj
