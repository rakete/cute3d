all: cute3d

cute3d:
	mkdir build; cd build; python ../ninja_build.py; ninja

test-%: cute3d
	cd build; ninja $@

.PHONY: clean
clean:
	rm -f build/
