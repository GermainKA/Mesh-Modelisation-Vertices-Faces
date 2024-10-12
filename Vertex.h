#ifndef __VERTEX_H__
#define __VERTEX_H__

#include <array>

class Vertex {
public:
    Vertex();
    Vertex(int id, float x, float y, float z);
    Vertex(int id,int inFace, float x, float y, float z);
    ~Vertex();

    int getId() const { return id; }
    int getInFace() const { return inFace; }
    std::array<float, 3> getCoords() const { return coords; }
    std::array<float, 3> getNormal() const { return normal; }

    void setId(int newId) { id = newId; }
    void setInFace(int newInFace) { inFace = newInFace; }
    void setCoords(float x, float y, float z) { coords = { x, y, z }; }
    void to2DCoords() { coords[2] = 0.0; }
    void setNormal(const std::array<float, 3>& newNormal) { normal = newNormal; }

    void display() const;

    Vertex operator+(const Vertex& other) const;
    Vertex operator-(const Vertex& other) const;
    Vertex operator*(float scalar) const;

    std::array<float, 3> coords;

private:
    int id;
    int inFace;
    std::array<float, 3> normal;
};

#endif // __VERTEX_H__
