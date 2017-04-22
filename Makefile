build: build-debug build-prod

build-debug:
	mkdir $@
	cd $@ && CC=gcc cmake -DDEMOS_DEBUG=ON ..

build-prod:
	mkdir $@
	cd $@ && CC=gcc cmake -DDEMOS_DEBUG=OFF ..

%.pack: %
	./pack $< $@

dist-clean:
	rm -Rf build-debug build-prod

.PHONY: build dist-clean
