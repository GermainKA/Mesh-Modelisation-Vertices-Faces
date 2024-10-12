#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>

#include <utils.h>

#include "Scene.h"


/** constructeur */
Scene::Scene()
{
    // créer les objets à dessiner
    // m_RedTriangle = new RedTriangle();
    // m_GreenTriangle = new GreenTriangle();

    m_mesh = new Mesh2D();
    
    // couleur du fond : gris foncé
    glClearColor(0.4, 0.4, 0.4, 0.0);

    // activer le depth buffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}


/**
 * appelée quand la taille de la vue OpenGL change
 * @param width : largeur en nombre de pixels de la fenêtre
 * @param height : hauteur en nombre de pixels de la fenêtre
 */
void Scene::onSurfaceChanged(int width, int height)
{
    // met en place le viewport
    glViewport(0, 0, width, height);
}


/**
 * Dessine l'image courante
 */
void Scene::onDrawFrame()
{
    // effacer l'écran
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_mesh->onDrawMesh();

}
void Scene::onAddRandomVertex(std::array<float, 3>& vertex ){

    m_mesh->AddVertex(vertex);

}
void Scene::onAddMouseClickVertex(float x, float y){
    std::array<float, 3> vertex = {x, y, 0.0f}; 
    m_mesh->AddVertex(vertex);
}


/** supprime tous les objets de cette scène */
Scene::~Scene()
{
    delete m_mesh;
}
