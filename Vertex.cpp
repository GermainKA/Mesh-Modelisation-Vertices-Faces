#include "Vertex.h"
#include <iostream>

Vertex::Vertex() : id(-1), coords({ 0.0f, 0.0f, 0.0f }), inFace(-1), normal({ 0.0f, 0.0f, 0.0f }) {}

Vertex::Vertex(int id, float x, float y, float z) : id(id), coords({ x, y, z }), inFace(-1), normal({ 0.0f, 0.0f, 0.0f }) {}
Vertex::Vertex(int id, int inFace ,float x, float y, float z) : id(id), coords({ x, y, z }), inFace(inFace), normal({ 0.0f, 0.0f, 0.0f }) {}

Vertex::~Vertex() {}

void Vertex::display() const {
    std::cout << "Vertex " << id << ": (" << coords[0] << ", " << coords[1] << ", " << coords[2] << ")";
    std::cout << " Normal: (" << normal[0] << ", " << normal[1] << ", " << normal[2] << ")";
    std::cout << " in Face: " << inFace;
    std::cout << std::endl;
}

Vertex Vertex::operator+(const Vertex& other) const {
    return Vertex(id, coords[0] + other.coords[0], coords[1] + other.coords[1], coords[2] + other.coords[2]);
}

Vertex Vertex::operator-(const Vertex& other) const {
    return Vertex(id, coords[0] - other.coords[0], coords[1] - other.coords[1], coords[2] - other.coords[2]);
}

Vertex Vertex::operator*(float scalar) const {
    return Vertex(id, coords[0] * scalar, coords[1] * scalar, coords[2] * scalar);
}
