HDRS = dmcbin.h dmcdma.h dmcdrc.h dmceth.h dmcfmw.h dmcint.h dmcisa.h dmclnx.h dmcpci.h dmcser.h dmcsup.h
OBJS = dmcbin.o dmccvs.o dmcdma.o dmceth.o dmcfmw.o dmcint.o dmcisa.o dmclnx.o dmcpci.o dmcser.o dmcsup.o
CFLAGS = -g

dmclnx.a : $(OBJS)
	ar r dmclnx.a $(OBJS)

dmcbin.o : dmcbin.c $(HDRS)
	gcc -c $(CFLAGS) dmcbin.c -o dmcbin.o
	
dmccvs.o : dmccvs.c $(HDRS)
	gcc -c $(CFLAGS) dmccvs.c -o dmccvs.o

dmcdma.o : dmcdma.c $(HDRS)
	gcc -c $(CFLAGS) dmcdma.c -o dmcdma.o
	
dmceth.o : dmceth.c $(HDRS)
	gcc -c $(CFLAGS) dmceth.c -o dmceth.o

dmcfmw.o : dmcfmw.c $(HDRS)
	gcc -c $(CFLAGS) dmcfmw.c -o dmcfmw.o
	
dmcint.o : dmcint.c $(HDRS)
	gcc -c $(CFLAGS) dmcint.c -o dmcint.o

dmcisa.o : dmcisa.c $(HDRS)
	gcc -c $(CFLAGS) dmcisa.c -o dmcisa.o
	
dmclnx.o : dmclnx.c $(HDRS)
	gcc -c $(CFLAGS) dmclnx.c -o dmclnx.o

dmcpci.o : dmcpci.c $(HDRS)
	gcc -c $(CFLAGS) dmcpci.c -o dmcpci.o

dmcser.o : dmcser.c $(HDRS)
	gcc -c $(CFLAGS) dmcser.c -o dmcser.o

dmcsup.o : dmcsup.c $(HDRS)
	gcc -c $(CFLAGS) dmcsup.c -o dmcsup.o

clean:
	rm $(OBJS) dmclnx.a *~












