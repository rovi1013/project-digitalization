# Wrapper Makefile

all:
	$(MAKE) -C src all

clean:
	$(MAKE) -C src clean

flash:
	$(MAKE) -C src flash

term:
	$(MAKE) -C src term
