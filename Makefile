P4API = /Users/hwangcc23/workdir/p4api-2013.3.784164
SOURCES = main.cc p4bisect.cc p4bisectclientuser.cc
INCLUDES = -I$(P4API)/include/p4/
OBJECTS = ${SOURCES:.cc=.o} 
LIBRARIES = $(P4API)/lib/libclient.a $(P4API)/lib/librpc.a $(P4API)/lib/libsupp.a $(P4API)/lib/libp4sslstub.a 
BINARY = p4bisect
RM = /bin/rm -f

C++ = g++
C++FLAGS = -Wall -c -g -D_XOPEN_SOURCE -D_DARWIN_C_SOURCE
LINK = g++
LINKFLAGS = -arch x86_64 -framework Cocoa -lpthread -ldl -lobjc -lncurses

${BINARY} : ${OBJECTS}
	${LINK} -o ${BINARY} ${OBJECTS} ${LIBRARIES} $(LINKFLAGS)

p4bisect.o : p4bisect.cc p4bisect.h p4bisectclientuser.h
	${C++} ${C++FLAGS} ${INCLUDES} -o $@ -c $<

p4bisectclientuser.o : p4bisectclientuser.cc p4bisectclientuser.h
	${C++} ${C++FLAGS} ${INCLUDES} -o $@ -c $<

.cc.o :
	${C++} ${C++FLAGS} $< ${INCLUDES}

clean : 
	- ${RM} ${OBJECTS} ${BINARY}
