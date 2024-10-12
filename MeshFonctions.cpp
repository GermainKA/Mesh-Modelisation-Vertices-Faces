#include "MeshFonctions.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>

MeshFonctions::MeshFonctions(Mesh& mesh) : mesh(mesh) {
    u.resize(mesh.vertices.size(), 0.0f);
    laplacian.resize(mesh.vertices.size(), 0.0f);
}

void MeshFonctions::initializeU(float val) {
    std::fill(u.begin(), u.end(), val);
}

int MeshFonctions::localIndice(int vertex_globalIndice, int faceId) {
    return mesh.localIndice(vertex_globalIndice, faceId);
}

std::vector<int> MeshFonctions::vertexShareFaces(int vertexId) {

    if (vertexId < 0 || vertexId >= mesh.vertices.size()) {
        std::cerr << "error invalid vertex!" << std::endl;
        return std::vector<int>();
    }

    std::vector<int> verShareFaces;
    int originFaceId = mesh.vertices[vertexId].getInFace();
    verShareFaces.push_back(originFaceId);

    int currentFaceId = originFaceId;
    int nextFaceId = -2; // Just INIT

    int currentVerLocalId = localIndice(vertexId, currentFaceId);
    int nextVerLocalId = (currentVerLocalId + 1) % 3;

    while (nextFaceId != originFaceId && nextFaceId != -1) {
        nextFaceId = mesh.faces[currentFaceId].getoppFaces()[nextVerLocalId];

        if (nextFaceId != originFaceId && nextFaceId != -1) {
            verShareFaces.push_back(nextFaceId);
            currentFaceId = nextFaceId;
            currentVerLocalId = localIndice(vertexId, currentFaceId);
            nextVerLocalId = (currentVerLocalId + 1) % 3;
        }
        else {
            break;
        }
    }

    return verShareFaces;
}

std::array<int, 3> MeshFonctions::faceAdjacentFaces(int faceId) {
    if (faceId < 0 || faceId >= mesh.faces.size()) {
        std::cerr << "Error: Invalid face ID!" << std::endl;
        return std::array<int, 3>();
    }
    return mesh.faces[faceId].getoppFaces();
}


void MeshFonctions::computeLaplacian() {
    laplacian.resize(mesh.vertices.size(), 0.0f);
    for (size_t idx = 0; idx < mesh.vertices.size(); ++idx) {
        float lapVal = 0.0f;
        int vertexId = static_cast<int>(idx);
        auto sharedFacesId = vertexShareFaces(vertexId);
        int numFaces = static_cast<int>(sharedFacesId.size());

        double sumValues = 0.0f;
        float sumFacesArea = 0.0f;

        for (int i = 0; i < numFaces; ++i) {
            int currentFaceId = sharedFacesId[i];
            Face& currentFace = mesh.faces[currentFaceId];
            int currentLocalInd = localIndice(vertexId, currentFaceId);

            int nextFaceId = sharedFacesId[(i + 1) % numFaces];
            Face& nextFace = mesh.faces[nextFaceId];
            int nextLocalInd = localIndice(vertexId, nextFaceId);

            int prev_j = currentFace.getfaceVertices()[(currentLocalInd + 1) % 3];
            int curr_j = currentFace.getfaceVertices()[(currentLocalInd + 2) % 3];
            int next_j = nextFace.getfaceVertices()[(nextLocalInd + 2) % 3];

            auto st_vec = mesh.vertices[vertexId].getCoords() - mesh.vertices[prev_j].getCoords();
            auto nd_vec = mesh.vertices[curr_j].getCoords() - mesh.vertices[prev_j].getCoords();

            auto rd_vec = mesh.vertices[vertexId].getCoords() - mesh.vertices[next_j].getCoords();
            auto th_vec = mesh.vertices[curr_j].getCoords() - mesh.vertices[next_j].getCoords();

            float cot1 = dotProd(st_vec, nd_vec) / length(crossProd(st_vec, nd_vec));
            float cot2 = dotProd(rd_vec, th_vec) / length(crossProd(rd_vec, th_vec));


            sumValues += (cot1 + cot2) * (u[curr_j] - u[vertexId]);

            auto last_vec = mesh.vertices[vertexId].getCoords() - mesh.vertices[curr_j].getCoords();

            //COMPUTE NORMAL CROSS PRODUCT AND SURFACE SUM
            auto cross = crossProd(st_vec, last_vec);

            currentFace.setNormal(cross);

            sumFacesArea += (float)(1.0 / 2.0) * length(cross);
            //std::cout << "SumAire " << sumFacesArea << std::endl;
        }
        sumFacesArea /= 3.0f;

        if (sumFacesArea != 0) {
            lapVal = (1.0 / (2.0 * sumFacesArea)) * sumValues;
        }
        else {

            lapVal = 0.0;
        }

        laplacian[vertexId] = lapVal;
    }
}

void MeshFonctions::computeCoordLaplacian() {
    
    std::array<float, 3> default_value = { 0.0f, 0.0f, 0.0f };
    delta_s.resize(mesh.vertices.size(), default_value);
    for (size_t idx = 0; idx < mesh.vertices.size(); ++idx) {
        std::array<float, 3> lapVal = default_value;
        std::array<float, 3> cross = default_value;
        int vertexId = static_cast<int>(idx);
        auto sharedFacesId = vertexShareFaces(vertexId);
        int numFaces = static_cast<int>(sharedFacesId.size());

        std::array<float, 3> sumValues = default_value;
        float sumFacesArea = 0.0f;

        for (int i = 0; i < numFaces; ++i) {
            //std::cout<<"Vertex "<< vertexId << " Faces " << sharedFacesId[i] << std::endl;
            int currentFaceId = sharedFacesId[i];
            Face& currentFace = mesh.faces[currentFaceId];
            int currentLocalInd = localIndice(vertexId, currentFaceId);

            int nextFaceId = sharedFacesId[(i + 1) % numFaces];
            Face& nextFace = mesh.faces[nextFaceId];
            int nextLocalInd = localIndice(vertexId, nextFaceId);

            int prev_j = currentFace.getfaceVertices()[(currentLocalInd + 1) % 3];
            int curr_j = currentFace.getfaceVertices()[(currentLocalInd + 2) % 3];
            int next_j = nextFace.getfaceVertices()[(nextLocalInd + 2) % 3];

            auto st_vec = mesh.vertices[vertexId].getCoords() - mesh.vertices[prev_j].getCoords();
            auto nd_vec = mesh.vertices[curr_j].getCoords() - mesh.vertices[prev_j].getCoords();

            auto rd_vec = mesh.vertices[vertexId].getCoords() - mesh.vertices[next_j].getCoords();
            auto th_vec = mesh.vertices[curr_j].getCoords() - mesh.vertices[next_j].getCoords();

            float cot1 = dotProd(st_vec, nd_vec) / length(crossProd(st_vec, nd_vec));
            float cot2 = dotProd(rd_vec, th_vec) / length(crossProd(rd_vec, th_vec));

            sumValues += ((cot1 + cot2) * (mesh.vertices[curr_j].getCoords() - mesh.vertices[vertexId].getCoords()));

            auto last_vec = mesh.vertices[vertexId].getCoords() - mesh.vertices[curr_j].getCoords();

            //COMPUTE NORMAL CROSS PRODUCT AND SURFACE SUM
            cross = crossProd(st_vec, last_vec);

            currentFace.setNormal(cross);

            sumFacesArea += (float)(1.0 / 2.0) * length(cross);
            //std::cout << "SumAire " << sumFacesArea << std::endl;
        }
        sumFacesArea /= 3.0f;

        if (sumFacesArea != 0) {
            lapVal = (1.0 / (2.0 * sumFacesArea)) * sumValues;
        }
        else {
            
            lapVal = default_value;
        }
        delta_s[vertexId] = lapVal;
    }
}

// Calcul des normals avec Les Laplacien des fonction coordinates
void MeshFonctions::computeVertexNormals() {
    if (delta_s.empty())
        computeCoordLaplacian();

    std::array<float, 3> default_value = { 0.0f, 0.0f, 0.0f };
    normales.resize(mesh.vertices.size(), default_value);
    for (int i = 0; i < mesh.vertices.size(); i++)
        normales[i] = delta_s[i];
}
void MeshFonctions::normalOrientationCorrect() {
    if (delta_s.empty())
        computeCoordLaplacian();
    for (int i = 0; i < mesh.vertices.size(); i++) {
        int inFaceId = mesh.vertices[i].getInFace();
        
        if(dotProd(mesh.faces[inFaceId].getNormal(), normales[i])<0.0)
            normales[i] = -1*normales[i];//Orientation Correction
    }

        
}
//Calcul des courbure Moyen aves les Laplacien des fonctions coordinates
void MeshFonctions::computeMeanCurvatures() {
    if (delta_s.empty())
        computeCoordLaplacian();
    std::array<float, 3> default_value = { 0.0f, 0.0f, 0.0f };
    mean_curvatures.resize(mesh.vertices.size());
    for (int i = 0; i < mesh.vertices.size(); i++)
        mean_curvatures[i] = length(delta_s[i]) / 2.0f;

}

void MeshFonctions::heatDiffusion(float dt,int source) {
    computeLaplacian();
    for (size_t idx = 0; idx < u.size(); ++idx) {
        if (idx != source) {//MAJ sans la sources
            u[idx] += dt * laplacian[idx];  //u[t+dt] = delta_u[t]*dt + u[t]
        }
        
    }
}

void MeshFonctions::exportNormalsVisualOBJ(const std::string& filename) {
    std::ofstream outFile(filename);
    if (!outFile) {
        std::cerr << "cannot open " << std::endl;
        return;
    }
    for (size_t idx = 0; idx < mesh.vertices.size(); ++idx) {
        const auto& vertex = mesh.vertices[idx];
        outFile << "v " << vertex.getCoords()[0] << " " << vertex.getCoords()[1] << " " << vertex.getCoords()[2] << std::endl;;
        
    }
    //  normals
    if (!normales.empty()) {
        normalOrientationCorrect();

        for (const auto& normal : normales) {
            outFile << "vn " << normal[0] << " " << normal[1] << " " << normal[2] << std::endl;
        }
    }
    //  faces
    for (const auto& face : mesh.faces) {
        outFile << "f ";
        for (int vertexId : face.getfaceVertices()) {
            outFile << vertexId + 1 << "//" << vertexId + 1 << " ";
        }
        outFile << std::endl;
    }

    outFile.close();
}

void MeshFonctions::exportCurvaturesVisualOBJ(const std::string& filename) {
    std::ofstream outFile(filename);
    if (!outFile) {
        std::cerr << "cannot open " << std::endl;
        return;
    }
    for (size_t idx = 0; idx < mesh.vertices.size(); ++idx) {
        const auto& vertex = mesh.vertices[idx];
        outFile << "v " << vertex.getCoords()[0] << " " << vertex.getCoords()[1] << " " << vertex.getCoords()[2] << std::endl;;

    }
    //  Texture1D
    if (!mean_curvatures.empty()) {

        auto minmax = std::minmax_element(begin(mean_curvatures), end(mean_curvatures));
        auto minIt = minmax.first;
        auto maxIt = minmax.second;

        float minVal = *minIt;
        float maxVal = *maxIt;
        char format[50];
        for (const auto& mean_curvature : mean_curvatures) {
            float interpolatVal = (mean_curvature - minVal) / (maxVal - minVal);
            std::snprintf(format, sizeof(format), "vt %.1f %.1f", interpolatVal, interpolatVal+0.1);
            outFile << format << std::endl;
        }
    }
    //  faces
    for (const auto& face : mesh.faces) {
        outFile << "f ";
        for (int vertexId : face.getfaceVertices()) {
            outFile << vertexId + 1 << "/" << vertexId + 1 << " ";
        }
        outFile << std::endl;
    }

    outFile.close();
}

void MeshFonctions::exportHeatDiffusVisualOBJ(const std::string& filename) {
    std::ofstream outFile(filename);
    if (!outFile) {
        std::cerr << "cannot open " << std::endl;
        return;
    }

    for (size_t idx = 0; idx < mesh.vertices.size(); ++idx) {
        const auto& vertex = mesh.vertices[idx];
        outFile << "v " << vertex.getCoords()[0] << " " << vertex.getCoords()[1] << " " << vertex.getCoords()[2] << std::endl;;

    }
    //  Heat
    //  Texture1D
    if (!u.empty()) {

        auto minmax = std::minmax_element(begin(u), end(u));
        auto minIt = minmax.first;
        auto maxIt = minmax.second;

        float minVal = *minIt;
        float maxVal = *maxIt;
        char format[50];
        for (const auto& ui : u) {
            float interpolatVal = (ui - minVal) / (maxVal - minVal);
            std::snprintf(format, sizeof(format), "vt %.1f %.1f", interpolatVal, interpolatVal + 0.1);
            outFile << format << std::endl;
        }
    }
    //  faces
    for (const auto& face : mesh.faces) {
        outFile << "f ";
        for (int vertexId : face.getfaceVertices()) {
            outFile << vertexId + 1 << "/" << vertexId + 1 << " ";
        }
        outFile << std::endl;
    }

    outFile.close();
}





