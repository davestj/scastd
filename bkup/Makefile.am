OBJS=DB.o

EXEC=scastd
CFLAGS=-g -I /usr/local/include -I /usr/include/libxml2 -I /usr/include/mariadb
LFLAGS=-L/usr/lib -lxml2 -lmariadb

.SUFFIXES: .cpp .o

.cpp.o:
	g++ $(CFLAGS) -o $@ -c $<

$(EXEC): $(OBJS) $(EXEC).cpp
	g++ $(CFLAGS) $(LFLAGS) -o $(EXEC) $(EXEC).cpp $(OBJS)

clean: 
	rm $(OBJS) $(EXEC)
