all: build

build:
	@echo "Building"
	gcc LinuxShell.c -o shell
	@echo "\n\n"
	./shell


clean:
	@echo "Cleaning"
	rm shell
