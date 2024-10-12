#ifndef __MESH_H__
#define __MESH_H__

#include "Vertex.h"
#include "Face.h"
#include <vector>
#include <string>

class MeshFonctions; 

class Mesh {
    friend class MeshFonctions;
public:
    Mesh();
    Mesh(const std::string& filename);
    virtual ~Mesh();
    virtual void onDrawMesh() = 0 ;

    void displayInfo() const;
    void findOppositeFaces();
    void displayOppositeFaces(int faceId);
    virtual void AddVertex(std::array<float, 3>& vertex) = 0 ;

    const std::vector<Vertex>& getVertices() const { return vertices; }
    std::vector<Vertex>& getVertices() { return vertices; }
    const std::vector<Face>& getFaces() const { return faces; }
protected:
    std::vector<Vertex> vertices;
    std::vector<Face> faces;
    int localIndice(int vertex_globalIndice, int faceId);
};
std::array<float, 3> crossProd(const std::array<float, 3>& vecA, const std::array<float, 3>& vecB);
float dotProd(const std::array<float, 3>& vecA, const std::array<float, 3>& vecB);
float length(const std::array<float, 3>& vec);
float computeArea(const std::array<float, 3>& vecAB, const std::array<float, 3>& vecAC);
std::array<float, 3> normalize(const std::array<float, 3>& vec);

std::array<float, 3> operator+(const std::array<float, 3>& a, const std::array<float, 3>& b);
std::array<float, 3> operator+=(std::array<float, 3>& a, const std::array<float, 3>& b);

std::array<float, 3> operator-(const std::array<float, 3>& a, const std::array<float, 3>& b);

std::array<float, 3> operator*(const std::array<float, 3>& a, float b);

std::array<float, 3> operator*(float b, const std::array<float, 3>& a);

#endif // __MESH_H__
