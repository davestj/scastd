PACKAGE=scastd
VERSION=0.1
AUTOMAKE=automake
OBJS=Socket.o DB.o

EXEC=scastd
CFLAGS=-g -I /usr/local/include -I /usr/include/libxml2 -I /usr/include/mysql
LFLAGS=-L/usr/lib -L/usr/lib/mysql -lxml2 -lmysqlclient





SUFFIXES=.cpp .o

.cpp.o:
	g++ $(CFLAGS) -o $@ -c $<

$(EXEC): $(OBJS) $(EXEC).cpp
	g++ $(CFLAGS) $(LFLAGS) -o $(EXEC) $(EXEC).cpp $(OBJS)


finished:
	@echo "----------------------------------------------------------------------"
	@echo "              Command Line Scast Daemon built!"
	@echo ""
	@echo "             command line = scastd"
	@echo "----------------------------------------------------------------------"

clean: 
	rm $(OBJS) $(EXEC)