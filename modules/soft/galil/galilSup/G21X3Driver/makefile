all: windows simppci simpeth term

windows: windows.c
	gcc -g -Wall -DLINUX windows.c ../../lib/linux-x86/libdmclnx.a -Idmclnxlib -owindows

simppci: simppci.c
	gcc -g -Wall simppci.c ../../lib/linux-x86/libdmclnx.a -Idmclnxlib -osimppci

simpeth: simpeth.c
	gcc -g -Wall simpeth.c ../../lib/linux-x86/libdmclnx.a -Idmclnxlib -osimpeth

term: term.c
	gcc -g -Wall term.c ../../lib/linux-x86/libdmclnx.a -Idmclnxlib -oterm

clean:
	rm windows simppci simpeth term *~
