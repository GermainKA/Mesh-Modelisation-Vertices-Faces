#ifndef SCENE_H
#define SCENE_H

// Définition de la classe Scene

#include <gl-matrix.h>

#include "M_Mesh.h"
#include "Mesh2D.h"
#include <random>
#include <iostream>


class Scene
{
private:

    Mesh * m_mesh;


public:

    /** constructeur, crée les objets 3D à dessiner */
    Scene();

    /** destructeur, libère les ressources */
    ~Scene();

    /**
     * appelée quand la taille de la vue OpenGL change
     * @param width : largeur en nombre de pixels de la fenêtre
     * @param height : hauteur en nombre de pixels de la fenêtre
     */
    void onSurfaceChanged(int width, int height);

    /** Dessine l'image courante */
    void onAddRandomVertex(std::array<float, 3> &vertex);
    void onAddMouseClickVertex(float x, float y);
    void onDrawFrame();
};

#endif
