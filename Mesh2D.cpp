#include "Mesh2D.h"
#include <algorithm>
#define N_VERTICES 100
#define N_LINES 500

void Mesh2D::initMesh2D(){
    
    if(!vertices.empty()){
        for (auto& vertice : vertices) {
        vertice.to2DCoords();
        std::cout << "To 2D coordinates" << std::endl;
        vertice.display();
    }
        
    }
    /// Shader
    const char* srcVertexShader =
        "#version 330\n"
        "in vec3 glVertex;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(glVertex, 1.0);\n"
        "}";

    const char* srcFragmentShader =
        "#version 330\n"
        "precision mediump float;\n"
        "out vec4 glFragColor;\n"
        "void main()\n"
        "{\n"
        "    glFragColor = vec4(0.84, 0.15, 0.51, 1.0);\n"
        "}";

    // Compile Shader 
    m_ShaderId = Utils::makeShaderProgram(srcVertexShader, srcFragmentShader, "Mesh2D");

    // GetLoc
    m_VertexLoc = glGetAttribLocation(m_ShaderId, "glVertex");
    
    /** VBOs */
    
    for (auto ver : vertices) {
        ver.display();
    }
    m_VertexLoc = glGetAttribLocation(m_ShaderId, "glVertex");

    glGenVertexArrays(1, &m_VertexArrayId);
    glBindVertexArray(m_VertexArrayId);

    glGenBuffers(1, &m_VertexBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId);
    glBufferData(GL_ARRAY_BUFFER, N_VERTICES*sizeof(Vertex),nullptr, GL_DYNAMIC_DRAW);
    
    glVertexAttribPointer(m_VertexLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, coords));
    glEnableVertexAttribArray(m_VertexLoc);
    

    glGenBuffers(1, &m_indexBufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, N_LINES*sizeof(unsigned short), nullptr, GL_DYNAMIC_DRAW);

    glBindVertexArray(0);

}
Mesh2D::Mesh2D() : Mesh(){
    initMesh2D();
}
Mesh2D::Mesh2D(const std::string& filename) :Mesh(filename) {
    initMesh2D();
}

Mesh2D::Mesh2D(Mesh& mesh) :Mesh(mesh) {
    initMesh2D();
}
void Mesh2D::onDrawMesh(){
    if (vertices.size()>N_VERTICES ) {
        std::cerr << "Warning: Mesh is too large to display!" << std::endl;
    }
    
        glUseProgram(m_ShaderId);
        glBindVertexArray(m_VertexArrayId);
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferId);

        std::vector<unsigned short> m_faces_indices;  // Populate this with your face vertex indices
        for (const auto& face : faces) {
            m_faces_indices.push_back(face.getfaceVertices()[0]);
            m_faces_indices.push_back(face.getfaceVertices()[1]);
            m_faces_indices.push_back(face.getfaceVertices()[1]);
            m_faces_indices.push_back(face.getfaceVertices()[2]);
            m_faces_indices.push_back(face.getfaceVertices()[2]);
            m_faces_indices.push_back(face.getfaceVertices()[0]);
        }
        if (m_faces_indices.size()>N_LINES ) {
        std::cerr << "Warning: Mesh is too large to display!" << std::endl;
        }
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, m_faces_indices.size() * sizeof(unsigned short), m_faces_indices.data());
        std::cout << "Nb faces: " << faces.size() << std::endl;
        glDrawElements(GL_LINES,Utils::clamp(0,m_faces_indices.size(), N_LINES) ,GL_UNSIGNED_SHORT,0);
        glUseProgram(0);
        

};

void Mesh2D::AddVertex(std::array<float, 3>& vertex) {
    //DEBUG <<
   //std::cout << "Adding vertex(" << vertex[0] << ", " << vertex[1] << ", " << vertex[2] << ")" << std::endl;
    if (vertex.empty() || vertex.size()!= 3) {
        std::cerr << "Error: Invalid vertex coordinates!" << std::endl;
        return;
    }
    vertex[2] = 0.0;
    //Check vertices.size() is lower or greater  than  3 
    if (vertices.empty() || vertices.size()<3) {
        if(vertices.size() == 2) {
        //If there is only one vertex, add a new face with the vertex and visible Mesh edges
        vertices.emplace_back(vertices.size(), faces.size(), vertex[0], vertex[1], vertex[2]);
        faces.emplace_back(faces.size(), vertices.size() - 3, vertices.size() - 2, vertices.size() - 1);
            std::cout << "Vertex added and Faces ADD successfully" << std::endl;
    vertices[vertices.size()-1].display();
    faces[faces.size()-1].display();
        return;
    }
        vertices.emplace_back(vertices.size(),faces.size(), vertex[0], vertex[1], vertex[2]);
            std::cout << "Vertex added successfully" << std::endl;
    vertices[vertices.size()-1].display();
        return;
    }
    
    else {
        //Check if Vertex already exists
        for (const auto& v : vertices) {
            if (std::equal(v.getCoords().begin(), v.getCoords().end(), vertex.begin())) {
                return;
            }
        }

        //If not, Check if the vertex is inside a existing faces
        for (auto& face : faces) {
            auto position = vertexPositionWithFace(vertex, face);
            switch (position)
            {
            case 1:
                splitFaceOnVertexAdding(face.getId(),vertex);                   
                break;
            case 0:
                                
                break;
            
            default:
                break;
            }
        }
        //If vertex is not inside a existing faces, create a new face with the vertex and visible Mesh edges
        //Find visible mesh edges with the vertex

        

    
    }

}

char Mesh2D::orientation(const std::array<float, 3>& vertex1, const std::array<float, 3>& vertex2, const std::array<float, 3>& vertex3) const{
    auto vec1 = vertex2 - vertex1;
    auto vec2 = vertex3 - vertex1;
    auto cross = crossProd(vec1, vec2);
    const float epsilon = 1e-6;
    if (std::abs(cross[2]) < epsilon) return 0;
    else return cross[2] > 0? 1 : -1;
}

void Mesh2D::flipEdgeEdge(int faceId, int vertex1, int vertex2) {
    //Get face vertices and adjacent faces
    Face& face0 = faces[faceId];
    auto face0_Vertices = faces[faceId].getfaceVertices();
    auto face0_OppFaces = faces[faceId].getoppFaces();

    //Get Local indices of vertex1 and vertex2 in face0
    int idxV1_InFace0 = localIndice(vertex1, faceId);
    int idxV2_InFace0 = localIndice(vertex2, faceId);

    if(idxV1_InFace0 == -1 || idxV2_InFace0 == -1) {
        std::cerr << "Vertex Not In Face" << std::endl;
        return;
    }
    //Get 3rd vertex in face0
    int idxV3_InFace0 = 3 - idxV1_InFace0 - idxV2_InFace0;
    int vertex3_InFace0 = face0_Vertices[idxV3_InFace0];

    //Get Adjacency face of face0
    int adjFaceId = face0_OppFaces[idxV1_InFace0];
    if (adjFaceId == -1) {
        std::cerr << "Edge On Boundary,Cannot flip" << std::endl;
        return;
    }
    Face& face1 = faces[adjFaceId];
    auto face1_Vertices = face1.getfaceVertices();
    auto face1_OppFaces = face1.getoppFaces();

    //Get Local indices of vertex1 and vertex2 in face1
    int idxV1_InFace1 = localIndice(vertex1, adjFaceId);
    int idxV2_InFace1 = localIndice(vertex2, adjFaceId);

    if(idxV1_InFace1 == -1 || idxV2_InFace1 == -1) {
        std::cerr << "Error Edge not in Adjface" << std::endl;
        return;
    }
    //Get 3rd vertex in face1
    int idxV3_InFace1 = 3 - idxV1_InFace1 - idxV2_InFace1;
    int vertex3_InFace1 = face1_Vertices[idxV3_InFace1];

    //Flip
    face0.setfaceVertices(vertex3_InFace0,vertex3_InFace1,vertex1);
    face1.setfaceVertices(vertex3_InFace1, vertex3_InFace0, vertex2);

    // Update Opposites faces of face0 and face1

    face0.setoppFaces(face1_OppFaces[idxV2_InFace1],face0_OppFaces[idxV2_InFace0],adjFaceId);

    face1.setoppFaces(face0_OppFaces[idxV1_InFace0],face1_OppFaces[idxV1_InFace1] ,faceId);

    // Update adjacent faces of the flipped faces
    //Opposite faces
    //Check if the faces is not boundary
    if (face0_OppFaces[idxV1_InFace0]!= -1) {
        int idxV3_InOppFace0 = localIndice(vertex3_InFace0, face0_OppFaces[idxV1_InFace0]);
        int idxV2_InOppFace0 = localIndice(vertex2, face0_OppFaces[idxV1_InFace0]);
        //Check the indices was found in the opposite face
        if (idxV3_InOppFace0!= -1 && idxV2_InOppFace0!= -1) {
            int idxLV_InOppFace0 = 3 - idxV3_InOppFace0 - idxV2_InOppFace0;
            faces[face0_OppFaces[idxV1_InFace0]].setoppFaces(idxLV_InOppFace0, adjFaceId);
        }
    }
    if (face1_OppFaces[idxV1_InFace1]!= -1) {
        int idxV3_InOppFace1 = localIndice(vertex3_InFace1, face1_OppFaces[idxV1_InFace1]);
        int idxV2_InOppFace1 = localIndice(vertex2, face1_OppFaces[idxV1_InFace1]);
        //Check the indices was found in the opposite face
        if (idxV3_InOppFace1!= -1 && idxV2_InOppFace1!= -1) {
            int idxLV_InOppFace1 = 3 - idxV3_InOppFace1 - idxV2_InOppFace1;
            faces[face1_OppFaces[idxV1_InFace1]].setoppFaces(idxLV_InOppFace1, adjFaceId);
        }
    }
    if (face0_OppFaces[idxV2_InFace0]!= -1) {
        int idxV3_InOppFace0 = localIndice(vertex3_InFace0, face0_OppFaces[idxV2_InFace0]);
        int idxV1_InOppFace0 = localIndice(vertex1, face0_OppFaces[idxV2_InFace0]);
        //Check the indices was found in the opposite face
        if (idxV3_InOppFace0!= -1 && idxV1_InOppFace0!= -1) {
            int idxLV_InOppFace0 = 3 - idxV3_InOppFace0 - idxV1_InOppFace0;
            faces[face0_OppFaces[idxV2_InFace0]].setoppFaces(idxLV_InOppFace0, faceId);
        }
    }
    if (face1_OppFaces[idxV2_InFace1]!= -1) {
        int idxV3_InOppFace1 = localIndice(vertex3_InFace0, face0_OppFaces[idxV2_InFace0]);
        int idxV1_InOppFace1 = localIndice(vertex1, face0_OppFaces[idxV2_InFace0]);
        //Check the indices was found in the opposite face
        if (idxV3_InOppFace1!= -1 && idxV1_InOppFace1!= -1) {
            int idxLV_InOppFace1 = 3 - idxV3_InOppFace1 - idxV1_InOppFace1;
            faces[face1_OppFaces[idxV2_InFace1]].setoppFaces(idxLV_InOppFace1, faceId);
        }

    }

}
//Check if a vertex is inside / outside /on edge
char Mesh2D::vertexPositionWithFace(const std::array<float, 3>& vertex,const Face& face){

    const std::array<float, 3>& V0 = vertices[face.getfaceVertices()[0]].getCoords();
    const std::array<float, 3>& V1 = vertices[face.getfaceVertices()[1]].getCoords();
    const std::array<float, 3>& V2 = vertices[face.getfaceVertices()[2]].getCoords();

    float d0 = orientation(V0, V1, vertex);
    float d1 = orientation(V1, V2, vertex);
    float d2 = orientation(V2, V0, vertex);
    // Check if the vertex is on the boundary of the face
    if ( (d0 == 0 && ((d1 > 0 && d2 > 0) || (d1 < 0 && d2 < 0))) ||
         (d1 == 0 && ((d0 > 0 && d2 > 0) || (d0 < 0 && d2 < 0))) ||
         (d2 == 0 && ((d0 > 0 && d1 > 0) || (d0 < 0 && d1 < 0))) ) 
    {
        return 0;//on edge
    }else if((d0 > 0 && d1 > 0 && d2 > 0) || (d0 < 0 && d1 < 0 && d2 < 0)){
        return 1; // inside
    }else return -1; // outside
}

void Mesh2D::splitFaceOnVertexAdding(int faceId,const std::array<float ,3>& vertCoord ){
    // Add new vertex
    vertices.emplace_back(vertices.size(), vertCoord[0], vertCoord[1], vertCoord[2]);
    int newVId = vertices.size() - 1;
    vertices[newVId].setInFace(faceId);

    // Get old face vertices and adjacent faces
    auto& oldFace = faces[faceId];
    auto oldFvertices = oldFace.getfaceVertices();
    auto oldOppFaces = oldFace.getoppFaces();

    int A = oldFvertices[0];
    int B = oldFvertices[1];
    int C = oldFvertices[2];

    int oppFaceA = oldOppFaces[0];
    int oppFaceB = oldOppFaces[1];
    int oppFaceC = oldOppFaces[2];

    // Replace original face with one off new face
    faces[faceId].setfaceVertices(newVId, A, B); // Face PAB
    // Add 2 new other faces
    int newFId1 = faces.size();
    faces.emplace_back(newFId1, newVId, B, C); // Face PBC

    int newFId2 = faces.size();
    faces.emplace_back(newFId2, newVId, C, A); // Face PCA

    // Update Opposites faces of new faces
    // Face PAB (faces[faceId])
    faces[faceId].setoppFaces(
        oppFaceC,    
        newFId1,     
        newFId2      
    );

    // Face PBC (faces[newFId1])
    faces[newFId1].setoppFaces(
        oppFaceA,    
        newFId2,     
        faceId       
    );

    // Face PCA (faces[newFId2])
    faces[newFId2].setoppFaces(
        oppFaceB,    
        faceId,      
        newFId1      
    );

    // 6. Update adjacent faces of the added faces
    // oppFaceC
    if (oppFaceC != -1) {
        int localIndex = localIndice(C,oppFaceC);
        if (localIndex != -1) {
            faces[oppFaceC].setoppFaces(localIndex, faceId);
        }
    }

    // oppFaceA
    if (oppFaceA != -1) {
        int localIndex = localIndice(A,oppFaceA);
        if (localIndex != -1) {
            faces[oppFaceA].setoppFaces(localIndex, newFId1);
        }
    }

    // oppFaceB
    if (oppFaceB != -1) {
        int localIndex = localIndice(B,oppFaceB);
        if (localIndex != -1) {
            faces[oppFaceB].setoppFaces(localIndex, newFId2);
        }
    }
}

Mesh2D::~Mesh2D()
{
    glDeleteBuffers(1, &m_VertexBufferId);
    glDeleteVertexArrays(1, &m_VertexArrayId);
    glDeleteProgram(m_ShaderId);
}