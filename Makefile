#Remember to retab!

ifndef VERB
    .SILENT:
endif

linuxLibs:
	cd libs/out/linux && \
		c++ \
		-c ../../rayImGui/*.cpp &&\
		ar rcs librlimgui.a *.o \
	&& cd ../../..

linux: # This target assumes you have raylib installed onto /usr/local/(bin/include).
	echo "---linux---"
	c++ \
		src/*.cpp \
	-Llibs/out/linux \
	-lrlimgui \
	-lraylib \
	-Ilibs \
	-o bin/linux/AsteroidMiner \
	-DPLATFORM_DESKTOP \
	$(ADDITIONAL_FLAGS)

webLibs:
	cd libs/out/web && \
		emcc -c ../../rayImGui/*.cpp \
		-I. -I../../../raylib/raylib-5.0/src -Ilibs \
		&& emar rcs librlimgui.a *.o \
		&& cd ../../.. \

web:
	echo "---web---"
	emcc src/*.cpp \
	-o bin/web/AsteroidMiner.html \
	-Os -Wall -std=c++20 \
	./raylib/raylib-5.0/src/libraylib.a \
	./libs/out/web/librlimgui.a \
	-I. -I./raylib/raylib-5.0/src -Ilibs \
	-L. -L./raylib/raylib-5.0/src -Llibs/out/web \
	-s USE_GLFW=3 -s ASYNCIFY \
	--shell-file ./raylib/raylib-5.0/src/shell.html \
	--preload-file resources/web/ \
	-s EXPORTED_RUNTIME_METHODS=ccall \
	-DPLATFORM_WEB $(ADDITIONAL_FLAGS)

compileRaylibWeb:
	echo "---raylib-web---"
	cd raylib/raylib-5.0/src && \
	make PLATFORM=PLATFORM_WEB && \
	cd ../../..

