all: cute3d

cute3d:
	mkdir -p build; cd build; python ../ninja_cute3d.py; ninja

test-%: cute3d
	cd build; ninja $@

.PHONY: clean
clean:
	rm -f build/
