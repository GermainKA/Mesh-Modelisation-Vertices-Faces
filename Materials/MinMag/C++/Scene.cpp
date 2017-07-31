#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>

#include <utils.h>
#include <Rectangle.h>
#include "Scene.h"



/**
 * Constructeur
 */
Scene::Scene() : TurnTableScene(false)
{
    // créer l'objet
    m_Object = new Rectangle();

    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);      // laisser voir le dos du rectangle

    // couleur du fond
    glClearColor(0.4, 0.4, 0.4, 0.0);

    // paramètres de la vue
    m_CameraAngleAzimut = -7.0;
    m_CameraAngleElevation = 5.0;
    m_CameraDistance = 16;
}


/**
 * appelée quand la taille de la vue OpenGL change
 * @param width : largeur en nombre de pixels de la fenêtre
 * @param height : hauteur en nombre de pixels de la fenêtre
 */
void Scene::onSurfaceChanged(int width, int height)
{
    // appeler la méthode de la superclasse
    TurnTableScene::onSurfaceChanged(width, height);

    // matrice de projection (champ de vision)
    mat4::perspective(m_Mat4Projection, Utils::radians(10.0), (float)width / height, 0.1, 100.0);
}


/**
 * dessine les objets de la scène
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice de vue
 */
void Scene::onDraw(mat4& mat4Projection, mat4& mat4ModelView)
{
    mat4 mat4ModelViewTmp = mat4::create();

    // dessiner l'objet à des distances croissantes
    int inc = 10;
    for (int z=10; z>-100; z=z-inc, inc=inc+4) {
        mat4::translate(mat4ModelViewTmp, mat4ModelView, vec3::fromValues(0, 0,  z));
        m_Object->onDraw(mat4Projection, mat4ModelViewTmp);
    }
}


/**
 * Cette méthode supprime les ressources allouées
 */
Scene::~Scene()
{
    delete m_Object;
}
