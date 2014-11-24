#vpath %.c ./
CC=gcc

#to compile with epics3.13 export EPICS:
#export EPICS=/work/epics
#to compile with epics3.14 export EPICS:
#export EPICS=/work/epics3.14.1

export EPICS=/usr/local/epics3.14.6
#export EPICS=/usr/local/epics3.13.2

CFLAGS  =  -DUNAME="\"UNAME=$(UNAME)\"" \
   -DLOGINDATE="\"LOGINDATE=$(LOGINDATE)\"" -DUNIX  -I $(EPICS)/base/include \
   -I $(EPICS)/base/include/os/Linux $(SERVER_CFLAGS) -I/usr/local/include -I. -I/usr/src/webserver/php-4.3.8 -I/usr/src/webserver/php-4.3.8/Zend -fpic -DCOMPILE_DL=1 -DPNG_SETJMP_NOT_SUPPORTED -DHAVE_PHP_STREAM

#LDFLAGS =  -shared -L/usr/local/lib -rdynamic -L $(EPICS)/base/lib/Linux -lCom -lca  -lm -lc -lpthread

LDFLAGS =  -shared -L/usr/local/lib -rdynamic -L $(EPICS)/base/lib/linux-x86 -lCom -lca  -lm -lc

PROGRAMS = php_epics.so 

all :  $(PROGRAMS)
	cc $(LDFLAGS) -o $(PROGRAMS) php_epics.o tree.o

php_epics.so: php_epics.o tree.o
php_epics.o: php_epics.c tree.h
tree.o: tree.c tree.h

install :
	cp $(PROGRAMS) /usr/local/lib/php/extensions

clean :
	rm -f *.o
	rm -f $(PROGRAMS)
