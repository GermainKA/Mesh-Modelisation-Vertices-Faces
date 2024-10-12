#ifndef __MESH_2D_H__
#define __MESH_2D_H__


#include <gl-matrix.h>
#include <utils.h>
#include <vector>
#include <string>
#include <iostream>
#include <stdlib.h>
#include "Face.h"
#include "M_Mesh.h"
#include "Vertex.h"



class Mesh2D : public Mesh {

    public :
    
    Mesh2D();
    Mesh2D(const std::string& filename) ;
    Mesh2D(Mesh& mesh) ;
    void initMesh2D();
    virtual ~Mesh2D() override;
    void onDrawMesh() override;
    void onSurfaceChanged(int width, int height);
    void AddVertex(std::array<float, 3>& vertex) override;
    void flipEdgeEdge(int face, int vertex1, int vertex2) ;
    char orientation(const std::array<float, 3>& vertex1, const std::array<float, 3>& vertex2, const std::array<float, 3>& vertex3) const;
    char vertexPositionWithFace(const std::array<float, 3>& vertex,const Face& face);
    void splitFaceOnVertexAdding(int faceId,const std::array<float ,3>& vertCoord );
    
    private :

    GLuint m_VertexBufferId;
    GLuint m_indexBufferId;
    GLuint m_VertexArrayId;

    /** identifiants liés au shader */
    GLuint m_ShaderId;
    GLuint m_VertexLoc;
    

}; 

#endif // __MESH_2D_H__
