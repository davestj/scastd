PACKAGE=scastd
VERSION=0.1
AUTOMAKE=automake
OBJS=Socket.o Config.o db/MySQLDatabase.o db/MariaDBDatabase.o db/PostgresDatabase.o

EXEC=scastd
CXXFLAGS=@CXXFLAGS@
LIBS=@LIBS@

SUFFIXES=.cpp .o

.cpp.o:
	g++ $(CXXFLAGS) -o $@ -c $<

$(EXEC): $(OBJS) $(EXEC).cpp
	g++ $(CXXFLAGS) -o $(EXEC) $(EXEC).cpp $(OBJS) $(LIBS)

finished:
	@echo "----------------------------------------------------------------------"
	@echo "              Command Line Scast Daemon built!"
	@echo ""
	@echo "             command line = scastd"
	@echo "----------------------------------------------------------------------"

clean:
	rm -f $(OBJS) $(EXEC)
