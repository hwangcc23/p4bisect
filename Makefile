P4API = /Users/hwangcc23/workdir/p4bisect/p4api-2013.3.784164
SOURCES = p4bisect.cc
INCLUDES = -I$(P4API)/include/p4/
OBJECTS = ${SOURCES:.cc=.o} 
LIBRARIES = $(P4API)/lib/libclient.a $(P4API)/lib/librpc.a $(P4API)/lib/libsupp.a $(P4API)/lib/libp4sslstub.a 
BINARY = p4bisect
RM = /bin/rm -f

C++ = g++
C++FLAGS = -c -g -D_XOPEN_SOURCE -D_DARWIN_C_SOURCE
LINK = g++
LINKFLAGS = -arch x86_64 -framework Cocoa -lpthread -ldl -lobjc

${BINARY} : ${OBJECTS}
	${LINK} -o ${BINARY} ${OBJECTS} ${LIBRARIES} $(LINKFLAGS)

.cc.o :
	${C++} ${C++FLAGS} $< ${INCLUDES}

clean : 
	- ${RM} ${OBJECTS} ${BINARY}
