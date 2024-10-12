#ifndef __FACE_H__
#define __FACE_H__

#include <vector>
#include <array>

class Face {
public:
    Face();
    Face(int id, int v1, int v2, int v3);
    ~Face();

    
    int getId() const { return id; }
    std::array<int, 3> getfaceVertices() const { return faceVertices; }
    std::array<int, 3> getoppFaces() const { return oppFaces; }
    std::array<float, 3> getNormal() const { return normal; }
    void setId(int newId) { id = newId; }
    void setfaceVertices(int v1, int v2, int v3) { faceVertices = { v1, v2, v3 }; }
    void setoppFaces(int f1, int f2, int f3) { oppFaces = { f1, f2, f3 }; }
    void setoppFaces(int pos, int f) { oppFaces[pos] = f; }
    void setNormal(const std::array<float, 3>& newNormal) { normal = newNormal; } //Compute with CrossProd

    void display() const;
    std::array<int ,3> faceVertices;

private:
    int id;
    std::array<int, 3> oppFaces;
    std::array<float, 3> normal; //With CrossProduct to  check Laplatien Normal Orientation
};

#endif  // __FACE_H__
