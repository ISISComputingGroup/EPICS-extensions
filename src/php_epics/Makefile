TOP = ../..
include $(TOP)/configure/CONFIG

#USR_SYS_LIBS_WIN32 += user32

LIBRARY_HOST = php_epics

USR_CFLAGS  +=  -I/usr/include/php5 -I/usr/include/php5/Zend -I/usr/include/php5/TSRM -I/usr/include/php5/main \
                -I/usr/include/php -I/usr/include/php/Zend -I/usr/include/php/TSRM -I/usr/include/php/main \
   -DUNAME="\"UNAME=$(UNAME)\"" \
   -DLOGINDATE="\"LOGINDATE=$(LOGINDATE)\"" -DUNIX  \
   $(SERVER_CFLAGS) -DCOMPILE_DL=1 -DPNG_SETJMP_NOT_SUPPORTED -DHAVE_PHP_STREAM

php_epics_SRCS += php_epics.c tree.c
php_epics_LIBS += $(EPICS_BASE_HOST_LIBS)

include $(TOP)/configure/RULES

