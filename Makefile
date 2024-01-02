#Remember to retab!

ifndef VERB
    .SILENT:
endif

linuxLibs:
	echo "---rlImGui---"
	cd libs/out/linux && \
		c++ \
		-c ../../rayImGui/*.cpp &&\
		ar rcs librlimgui.a *.o \
	&& cd ../../..
	echo "---frustum---"
	cd libs/out/linux && \
		cc \
		-c ../../frustum/*.c &&\
		ar rcs libfrustum.a *.o \
	&& cd ../../..

linux: # This target assumes you have raylib installed onto /usr/local/(bin/include).
	echo "---linux---"
	c++ \
		src/*.cpp \
	-Llibs/out/linux \
	-lrlimgui \
	-lraylib \
	-lfrustum \
	-Ilibs \
	-o bin/linux/AsteroidMiner \
	-DPLATFORM_DESKTOP \
	$(ADDITIONAL_FLAGS)

webLibs:
	echo "---rlImGui---"
	cd libs/out/web && \
		emcc -c ../../rayImGui/*.cpp \
		-I. -I../../../raylib/raylib-5.0/src -Ilibs \
		&& emar rcs librlimgui.a *.o \
	&& cd ../../.. 
	echo "---frustum---"
	cd libs/out/web && \
		emcc -c ../../frustum/*.c\
		-I. -I../../../raylib/raylib-5.0/src -Ilibs \
		&& emar rcs libfrustum.a *.o \
	&& cd ../../.. 

web:
	echo "---web---"
	emcc src/*.cpp \
	-o bin/web/AsteroidMiner.html \
	-O3 -Wall -std=c++20 \
	./raylib/raylib-5.0/src/libraylib.a \
	./libs/out/web/librlimgui.a \
	./libs/out/web/libfrustum.a \
	-I. -I./raylib/raylib-5.0/src -Ilibs \
	-L. -L./raylib/raylib-5.0/src -Llibs/out/web \
	-s USE_GLFW=3 -s ASYNCIFY \
	-s TOTAL_MEMORY=256MB \
	--preload-file resources/web/ \
	-s EXPORTED_RUNTIME_METHODS=ccall \
	--shell-file=./raylib/raylib-5.0/src/minshell.html \
	-DPLATFORM_WEB $(ADDITIONAL_FLAGS)

compileRaylibWeb:
	echo "---raylib-web---"
	cd raylib/raylib-5.0/src && \
	make PLATFORM=PLATFORM_WEB && \
	cd ../../..

