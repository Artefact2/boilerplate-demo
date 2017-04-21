build:
	@mkdir -p build
	@cd build && cmake ..

%.pack: %
	./pack $< $@

dist-clean:
	@rm -Rf build

.PHONY: build dist-clean
