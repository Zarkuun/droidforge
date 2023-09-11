.PHONY: build-linux default

default: build-linux

build-linux:
	docker build -t droidforge .
	docker run -it --rm droidforge
