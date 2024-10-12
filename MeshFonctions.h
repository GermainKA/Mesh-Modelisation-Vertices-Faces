#ifndef __MESHFONCTIONS_H__
#define __MESHFONCTIONS_H__

#include "M_Mesh.h"
#include <vector>


class MeshFonctions {
public:
    MeshFonctions(Mesh& mesh);
    void initializeU(float val);
    void computeLaplacian();
    void computeCoordLaplacian();
    void heatDiffusion(float dt, int source);
    void exportNormalsVisualOBJ(const std::string& filename);
    void exportCurvaturesVisualOBJ(const std::string& filename);
    void exportHeatDiffusVisualOBJ(const std::string& filename);
    void computeVertexNormals();
    void normalOrientationCorrect();
    void computeMeanCurvatures();

    const std::vector<float>& getU() const { return u; }
    void setU(size_t pos, float value) { if (pos < u.size()) u[pos] = value; }
    const std::vector<float>& getLaplacian() const { return laplacian; }

private:
    Mesh& mesh;
    std::vector<float> u;
    
    std::vector<float> laplacian;
    std::vector<std::array<float,3>> delta_s;
    std::vector<std::array<float, 3>> normales;
    std::vector<float> mean_curvatures;

    int localIndice(int vertex_globalIndice, int faceId);
    std::vector<int> vertexShareFaces(int vertexId);
    std::array<int, 3> faceAdjacentFaces(int faceId);
};



#endif // __MESHFONCTIONS_H__
