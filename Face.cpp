#include "Face.h"
#include <iostream>

Face::Face() : id(-1), faceVertices({ -1, -1, -1 }), oppFaces({ -1, -1, -1 }), normal({ 0.0f, 0.0f, 0.0f }) {}

Face::Face(int id, int v1, int v2, int v3) : id(id), faceVertices({ v1, v2, v3 }), oppFaces({ -1, -1, -1 }), normal({ 0.0f, 0.0f, 0.0f }) {}

Face::~Face() {}



void Face::display() const {
    std::cout << "Face " << id << ": [" << getfaceVertices()[0] << ", " << getfaceVertices()[1] << ", " << getfaceVertices()[2] << "]";
    std::cout << " Normal: (" << normal[0] << ", " << normal[1] << ", " << normal[2] << ")";
    std::cout << std::endl;
}