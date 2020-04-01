
# ------------------- By: TSURA @ -------------------
# ███╗   ██╗███████╗ ██████╗ ██╗  ██╗███████╗██╗  ██╗
# ████╗  ██║██╔════╝██╔═══██╗██║  ██║██╔════╝╚██╗██╔╝
# ██╔██╗ ██║█████╗  ██║   ██║███████║█████╗   ╚███╔╝ 
# ██║╚██╗██║██╔══╝  ██║   ██║██╔══██║██╔══╝   ██╔██╗ 
# ██║ ╚████║███████╗╚██████╔╝██║  ██║███████╗██╔╝ ██╗
# ╚═╝  ╚═══╝╚══════╝ ╚═════╝ ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝
# ---------------------- [.xyz] ---------------------


CC=clang

LDFLAGS= -g -Wall -fPIC -m64 -lm -lstdc++ -std=c++14 $(shell pkg-config --cflags --libs sdl2)  $(shell pkg-config --cflags --libs freetype2) $(shell pkg-config glew --cflags --libs) -pthread -lglut

# SkeletonGL core files
SOURCES= src/main.cpp \
		 src/window.cpp \
		 src/map.cpp \
	 	 src/skeletonGL/window/SGL_Window.cpp \
		 src/skeletonGL/utility/SGL_AssetManager.cpp \
		 src/skeletonGL/utility/SGL_Utility.cpp \
		 src/skeletonGL/renderer/SGL_OpenGLManager.cpp \
		 src/skeletonGL/renderer/SGL_Shader.cpp \
		 src/skeletonGL/renderer/SGL_Texture.cpp \
		 src/skeletonGL/renderer/SGL_PostProcessor.cpp \
		 src/skeletonGL/renderer/SGL_Camera.cpp \
		 src/skeletonGL/renderer/SGL_Renderer.cpp


OBJECTS=$(SOURCES:.cpp=.o)

EXECUTABLE=snake_sgl

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC)  $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $< $(LDFLAGS) -c -o $@


clean:
	rm -rf src/*.o
	rm -rf src/skeletonGL/*.o
	rm -rf src/skeletonGL/renderer/*.o
	rm -rf src/skeletonGL/window/*.o
	rm -rf src/skeletonGL/utility/*.o
