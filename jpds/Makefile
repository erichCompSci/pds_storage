LIBRARIES = -lwp -lecho -lecl -licode -lvcode
#-lgenthreads -lcthreads -lsocket

prefix =		/usr/local
exec_prefix =		.
program_prefix = 	
bindir =		${exec_prefix}/bin
libdir =		${exec_prefix}/lib
incdir =		${prefix}/include

JAVA_HOME = /usr/local/jdk1.2
JAVADIR = $(JAVA_HOME)
CXX = g++

CFLAGS = -g -I. -I../../wp -I/users/c/chaos/include -I/users/c/chaos/include/sparc -I$(JAVADIR)/include -I$(JAVADIR)/include/solaris  -Wall -fvolatile
LDFLAGS = -g -L. -L../../lib -L../.. -L/users/c/chaos/lib/sun7

SOFLAGS = $(CFLAGS) -shared $(LDFLAGS)
SOLIBS = $(LIBRARIES) -lstdc++ -lgcc

SRCS =	jwpClient.cc attrList.cc
HEADERS = jwpClient.h attrList.h
LIB_OBJS = jwpClient.o attrList.o

CLASSPATH=$(JAVADIR)/lib/tools.jar:$(JAVADIR)/jre/lib/rt.jar:./visad.jar:.

JAVA   = $(JAVADIR)/bin/java -classpath $(CLASSPATH) 
JAVAC  = $(JAVADIR)/bin/javac -deprecation -classpath $(CLASSPATH)

TARGET = jwpClient
SOTARGET = libjwpClient.so

JAVA_SRCS = jwpDomainId.java jwpContextId.java jwpEntityId.java jwpClient.java attrList.java
JAVA_OBJS = $(JAVA_SRCS:java=class)

.SUFFIXES: .C .c .cpp .java .class

.C.o:
	$(CXX) $(CFLAGS) -c $*.C

.cc.o:
	$(CXX) $(CFLAGS) -c $*.cc

.c.o:
	$(CC) $(CFLAGS) -c $*.c

.java.class:
	$(JAVAC) $*.java

all install clean distclean::
	@echo "*****\n***  jpds: Makefile disabled\n*****"

all1::	libjwpClient.so $(JAVA_OBJS)

libjwpClient.so: $(LIB_OBJS) $(HEADERS)
	$(CXX) $(SOFLAGS) -o $@ $(LIB_OBJS) $(SOLIBS)


run:	$(TARGET).class $(SOTARGET)
	$(JAVA) $(TARGET)


check:

