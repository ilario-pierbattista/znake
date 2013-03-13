# main Makefile

all:
	make -C src all

clean:
	rm -f check.log
	make -C src clean
	
check:
	./check-dep

install:
	make -C src install

uninstall:
	make -C src uninstall

