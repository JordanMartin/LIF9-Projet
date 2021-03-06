CC = g++ 
CFLAGS = -g -Wall -ansi -std=c++11
EXEC_NAME = astar

default: $(EXEC_NAME)

$(EXEC_NAME): main.o maze.o astar.o union_find.o maze_path.o maze_grid.o
	$(CC) $(CFLAGS) -o $(EXEC_NAME) main.o maze.o astar.o union_find.o maze_path.o maze_grid.o

main.o: main.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

maze.o: maze.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

astar.o: astar.cpp
	$(CC) $(CFLAGS) -o $@ -c $<
	
union_find.o: union_find.cpp
	$(CC) $(CFLAGS) -o $@ -c $<
	
maze_grid.o: maze_grid.cpp
	$(CC) $(CFLAGS) -o $@ -c $<
	
maze_path.o: maze_path.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

simple: CFLAGS += -DMAZE_SIMPLE_DISPLAY

simple: clean $(EXEC_NAME)

clean:
	rm -rf test_maze.o main.o maze.o astar.o union_find.o maze_path.o maze_grid.o $(EXEC_NAME)
