# Makefile pour les programmes OpenGL du livre D-BookeR
# note: sudo apt-get install libglfw3-dev libglew-dev libsdl2-dev libsdl2-image-dev

# nom du programme à construire
EXEC = main

# liste des modules utilisateur : tous les .cpp (privés de cette extension) du dossier courant
MODULES = $(basename $(wildcard [A-Z]*.cpp))

# liste des fichiers sources des libs : tous les .cpp dans libs/
MODULES_LIBS = $(wildcard libs/*.cpp libs/*/*.cpp)

# liste des dossiers à inclure : tous ceux de libs
MODULES_INCS = $(sort $(dir $(wildcard libs/*/*.h)))

# options de compilation et librairies
CXXFLAGS = -std=c++11 -I. -Ilibs $(addprefix -I,$(MODULES_INCS)) -I/usr/include/SDL2 -g # -O3
LIBS = -lGLEW -lGL -lGLU -lglfw -lSDL2 -lSDL2_image

# fichiers objets des modules utilisateur
OBJECTS = $(patsubst %,.o/%.o,$(notdir $(MODULES)))

# fichiers objets des librairies
OBJECTS_LIBS = $(patsubst libs/%.cpp,.o/libs/%.o,$(MODULES_LIBS))

#### Ne pas modifier au-delà (sauf si vous savez ce que vous faites)

# exécution du programme
run:	$(EXEC)
	./$(EXEC)

# édition des liens entre tous les fichiers objets
$(EXEC): .o/main.o $(OBJECTS) $(OBJECTS_LIBS)
	$(CXX) -o $@ $^ $(LIBS)

# compilation des modules utilisateur
.o/%.o: %.cpp | .o
	$(CXX) $(CXXFLAGS) -c $< -o $@

# compilation des modules des librairies
.o/libs/%.o: libs/%.cpp | .o/libs
	$(CXX) $(CXXFLAGS) -c $< -o $@

# création du dossier .o/
.o:
	mkdir -p .o

# création du dossier .o/libs/
.o/libs:
	mkdir -p .o/libs

# exécution avec vérification de la mémoire
valgrind:	$(EXEC)
	valgrind --track-origins=yes --leak-check=full --num-callers=30 ./$(EXEC) | tee valgrind.log

# vérification avec glslangValidator
glslang:	$(EXEC)
	for f in *.vert ; do glslangValidator $${f} $${f%.vert}.frag ; done

# icone
icon:	run
	-convert -quality 95 image.ppm ../$(shell basename $(dir $(CURDIR))).jpg

# nettoyage complet : l'exécutable est supprimé aussi
cleanall: clean
	rm -f $(EXEC) image.ppm

# nettoyage du projet et des librairies
cleanalllibs:	cleanall cleanlibs

# nettoyage des fichiers objets et logs du projet
clean:
	rm -rf .o *.log *.vert *.frag *~

# suppression des fichiers objets des librairies
cleanlibs:
	rm -fr .o/libs/*.o
