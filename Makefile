.PHONY: build-linux default

default: build-linux

build-linux:
	docker build -t droidforge .
	docker run -v $(PWD)/droidforge:/app -it --rm droidforge

 # "cmake -S . -B build && cmake --build build -j24"
