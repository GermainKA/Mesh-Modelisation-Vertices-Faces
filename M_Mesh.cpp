#include "M_Mesh.h"
#include <array>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>
#include <cmath>

Mesh::Mesh() {}

Mesh::Mesh(const std::string& filename) {
    std::ifstream infile(filename);
    if (!infile.is_open()) {
        return;
    }

    std::string line;
    int numVertices = 0, numFaces = 0;
    while (std::getline(infile, line)) {
        if (line.substr(0, 3) == "OFF") {
            continue;  //skip for first line
        }
        std::istringstream iss(line);
        if (numVertices == 0 && numFaces == 0) {
            iss >> numVertices >> numFaces;
        }
        else if (numVertices > 0) {
            float x, y, z;
            iss >> x >> y >> z;
            vertices.emplace_back(vertices.size(), x, y, z);//emplace_back pour eviter la creation d'une copie temp  
            --numVertices;
        }
        else if (numFaces > 0) {
            int count, idx1, idx2, idx3;
            iss >> count >> idx1 >> idx2 >> idx3;
            faces.emplace_back(faces.size(), idx1, idx2, idx3);
            for (int idx : { idx1, idx2, idx3 }) {
                if (vertices[idx].getInFace() == -1) { //PAcq je l'avais init a -1
                    vertices[idx].setInFace(static_cast<int>(faces.size()) - 1);
                }
            }
            --numFaces;
        }
    }
    infile.close();
}

Mesh::~Mesh() {}

void Mesh::displayInfo() const {
    std::cout << "Mesh Info: " << "Num Vert: " << vertices.size() << "Num of Faces: " << faces.size() << std::endl;
}

void Mesh::findOppositeFaces() {
    std::map<std::pair<int, int>, int> edgeToFaceMap;

    for (auto& face : faces) {
        std::pair<int, int> edge1 = std::minmax(face.getfaceVertices()[0], face.getfaceVertices()[1]);
        std::pair<int, int> edge2 = std::minmax(face.getfaceVertices()[1], face.getfaceVertices()[2]);
        std::pair<int, int> edge3 = std::minmax(face.getfaceVertices()[2], face.getfaceVertices()[0]);

        for (const auto& edge : { edge1, edge2, edge3 }) {
            if (edgeToFaceMap.count(edge)) {
                int otherFaceId = edgeToFaceMap[edge];

                Face& otherFace = faces[otherFaceId];
                for (int vid : face.getfaceVertices()) {
                    if (vid != edge.first && vid != edge.second) {
                        int indLocal = localIndice(vid, face.getId());
                        face.setoppFaces(indLocal, otherFaceId);
                    }
                }
                for (int vid : otherFace.getfaceVertices()) {
                    if (vid != edge.first && vid != edge.second) {
                        int indLocal = localIndice(vid, otherFace.getId());
                        otherFace.setoppFaces(indLocal, face.getId());
                    }
                }
            }
            else {
                edgeToFaceMap[edge] = face.getId();
            }
        }
    }
}

void Mesh::displayOppositeFaces(int faceId) {
    if (faceId < 0 || faceId >= faces.size()) {
        std::cerr << "Error: Invalid face ID!" << std::endl;
        return;
    }
    Face& face = faces[faceId];
    std::cout << "Adjacent faces for Face " << faceId << " [" << face.getfaceVertices()[0] << ", " << face.getfaceVertices()[1] << ", " << face.getfaceVertices()[2] << "]:" << std::endl;
    for (int oppFace : face.getoppFaces()) {
        std::cout << oppFace << " | ";
    }
    std::cout << std::endl;
}

int Mesh::localIndice(int vertex_globalIndice, int faceId) {
    auto faceVertices = faces[faceId].getfaceVertices();
    auto it = std::find(faceVertices.begin(), faceVertices.end(), vertex_globalIndice);
    int index = -1;
    if (it != faceVertices.end()) {
        index = static_cast<int>(std::distance(faceVertices.begin(), it));
    }
    else {
        return -1;
    }
    return index;
}


float dotProd(const std::array<float, 3>& vecA, const std::array<float, 3>& vecB) {
    return vecA[0] * vecB[0] + vecA[1] * vecB[1] + vecA[2] * vecB[2];
}

std::array<float, 3> crossProd(const std::array<float, 3>& vecA, const std::array<float, 3>& vecB) {
    return {
        vecA[1] * vecB[2] - vecA[2] * vecB[1],
        vecA[2] * vecB[0] - vecA[0] * vecB[2],
        vecA[0] * vecB[1] - vecA[1] * vecB[0]
    };
}

float length(const std::array<float, 3>& vec) {
    return std::sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
}

float computeArea(const std::array<float, 3>& vecAB, const std::array<float, 3>& vecAC) {
    auto vec = crossProd(vecAB, vecAC); 
    return (float)(1.0 / 2.0) * length(vec);
}

std::array<float, 3> normalize(const std::array<float, 3>& vec) {
    float len = length(vec);
    if (len > 0) {
        return { vec[0] / len, vec[1] / len, vec[2] / len };
    }
    else {
        return { 0.0f, 0.0f, 0.0f }; 
    }
}




std::array<float, 3> operator+(const std::array<float, 3>& a, const std::array<float, 3>& b) {
    std::array<float, 3> result;
    for (size_t i = 0; i < 3; ++i) {
        result[i] = a[i] + b[i];
    }
    return result;
}
std::array<float, 3> operator+=( std::array<float, 3>& a, const std::array<float, 3>& b) {
    for (size_t i = 0; i < 3; ++i) {
         a[i] += b[i];
    }
    return a;
}

std::array<float, 3> operator-(const std::array<float, 3>& a, const std::array<float, 3>& b) {
    std::array<float, 3> result;
    for (size_t i = 0; i < 3; ++i) {
        result[i] = a[i] - b[i];
    }
    return result;
}

std::array<float, 3> operator*(const std::array<float, 3>& a, float b) {
    std::array<float, 3> result;
    for (size_t i = 0; i < 3; ++i) {
        result[i] = a[i] * b;
    }
    return result;
}

std::array<float, 3> operator*(float b ,const std::array<float, 3>& a) {
    std::array<float, 3> result;
    for (size_t i = 0; i < 3; ++i) {
        result[i] = a[i] * b;
    }
    return result;
}