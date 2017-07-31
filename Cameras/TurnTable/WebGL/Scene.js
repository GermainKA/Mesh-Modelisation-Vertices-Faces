﻿// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Mesh/MeshObjectFromObj");
Requires("libs/Material/OmniLight.js");
Requires("libs/Misc/AxesXYZ.js");
Requires("libs/Misc/SceneBase.js");


class Scene extends SceneBase
{
    /**
     * Constructeur
     */
    constructor()
    {
        super(true);

        // créer les objets à dessiner
        this.m_Ground = new MeshObjectFromObj("data/models/TerrainSimple", "Terrain.obj", "Terrain.mtl", 2.0);
        this.m_Lorry  = new MeshObjectFromObj("data/models/Camion", "camion.obj", "camion.mtl", 2.0);
        this.m_PalmTree = new MeshObjectFromObj("data/models/Palm_Tree", "Palm_Tree.obj", "Palm_Tree.mtl", 2.0);
        this.m_Axes = new AxesXYZ(2.0, 1.0, 3.0);

        // définir une lampe directionnelle
        this.m_Light0 = new OmniLight();
        this.m_Light0.setPosition(vec4.fromValues(10, 5, 10, 0));
        this.m_Light0.setColor(vec3.fromValues(0.9,0.9,0.9));
        this.addLight(this.m_Light0);

        // définir une lampe positionnelle
        this.m_Light1 = new OmniLight();
        this.m_Light1.setPosition(vec4.fromValues(-10, 5, 5, 1));
        this.m_Light1.setColor(vec3.fromValues(100,100,100));
        this.addLight(this.m_Light1);

        // définir une lampe directionnelle
        this.m_Light2 = new OmniLight();
        this.m_Light2.setPosition(vec4.fromValues(-5, 5, -10, 0));
        this.m_Light2.setColor(vec3.fromValues(1.0,1.0,1.0));
        this.addLight(this.m_Light2);

        // configurer les modes de dessin
        gl.enable(gl.DEPTH_TEST);
        gl.enable(gl.CULL_FACE);

        // couleur du fond
        gl.clearColor(0.4, 0.5, 0.6, 0.0);

        // paramètres de la vue
        this.m_CameraAngleAzimut = 60.0;
        this.m_CameraAngleElevation = 30.0;
        this.m_CameraPivot = vec3.create();
        this.m_CameraDistance = 35;
    }


    /** affiche les informations sur la caméra **/
    updateInfos()
    {
        document.getElementById("infos").innerHTML =
            "azimut = " + Math.round(this.m_CameraAngleAzimut * 10)/10 + "<br/>" +
            "height = " + Math.round(this.m_CameraAngleElevation * 10)/10 + "<br/>" +
            "pivot = (" + Math.round(this.m_CameraPivot[0] * 10)/10 + ", " + Math.round(this.m_CameraPivot[1] * 10)/10 + ", " + Math.round(this.m_CameraPivot[2] * 10)/10 + ")<br/>" +
            "distance = " + Math.round(this.m_CameraDistance * 10)/10 + "<br/>";
    }


    /**
     * appelée quand la taille de la vue OpenGL change
     * @param width : largeur en nombre de pixels de la fenêtre
     * @param height : hauteur en nombre de pixels de la fenêtre
     */
    onSurfaceChanged(width, height)
    {
        // appeler la méthode de la superclasse
        super.onSurfaceChanged(width, height);

        // matrice de projection (champ de vision)
        mat4.perspective(this.m_Mat4Projection, Utils.radians(12.0), width / height, 1.0, 100.0);
    }


    /**
     * appelée quand on enfonce le bouton du milieu et qu'on déplace la souris
     * @param dd : nombre de pixels parcourus
     */
    onZoom(dd)
    {
        this.m_CameraDistance *= Math.exp(dd * 0.001);
        this.updateInfos();
    }


    /**
     * appelée quand on enfonce le bouton droit et qu'on déplace la souris
     * @param dx : nombre de pixels parcourus horizontalement
     * @param dy : nombre de pixels parcourus verticalement
     */
    onPan(dx, dy)
    {
        // NB : le décalage se fait dans le plan de l'image

        // construire la matrice inverse de l'orientation de la vue à la souris
        let ivMatrix = mat4.create();
        mat4.rotate(ivMatrix, ivMatrix, Utils.radians(this.m_CameraAngleAzimut), vec3.fromValues(0, -1, 0));
        mat4.rotate(ivMatrix, ivMatrix, Utils.radians(this.m_CameraAngleElevation), vec3.fromValues(-1, 0, 0));

        // transformer le mouvement de la souris dans l'espace de la scène
        let offset = vec3.create();
        vec3.transformMat4(offset, vec3.fromValues(dx*0.01, dy*0.01, 0), ivMatrix);
        vec3.add(this.m_CameraPivot, this.m_CameraPivot, offset);

        this.updateInfos();
    }


    /**
     * appelée quand on enfonce le bouton gauche et qu'on déplace la souris
     * @param oldX : ancienne absisse de la souris
     * @param oldY : ancienne ordonnée de la souris
     * @param x : nouvelle absisse de la souris
     * @param y : nouvelle ordonnée de la souris
     * @param width : largeur de la vue OpenGL
     * @param height : hauteur de la vue OpenGL
     */
    onRotate(oldX, oldY, x, y, width, height)
    {
        // différences de position
        let dx = x - oldX;
        let dy = y - oldY;

        this.m_CameraAngleAzimut  += dx * 0.1;
        //this.m_CameraAngleAzimut = Math.fmod(this.m_CameraAngleAzimut, 360.0);
        this.m_CameraAngleElevation += dy * 0.1;
        if (this.m_CameraAngleElevation >  90.0) this.m_CameraAngleElevation =  90.0;
        if (this.m_CameraAngleElevation < -90.0) this.m_CameraAngleElevation = -90.0;
        this.updateInfos();
    }


    /**
     * appelée quand on appuie sur une touche du clavier
     * @param code : touche enfoncée
     */
    onKeyDown(code)
    {
        // construire la matrice inverse de l'orientation de la vue à la souris
        let ivMatrix = mat4.create();
        mat4.identity(ivMatrix);
        mat4.rotate(ivMatrix, ivMatrix, Utils.radians(this.m_CameraAngleAzimut), vec3.fromValues(0, -1, 0));
        mat4.rotate(ivMatrix, ivMatrix, Utils.radians(this.m_CameraAngleElevation), vec3.fromValues(-1, 0, 0));

        // vecteur indiquant le décalage à appliquer au pivot de la rotation
        let offset = vec3.create();
        switch (code) {
        case 'Z'.charCodeAt(0):
            this.m_CameraDistance *= Math.exp(-0.01);
            break;
        case 'S'.charCodeAt(0):
            this.m_CameraDistance *= Math.exp(+0.01);
            break;
        case 'Q'.charCodeAt(0):
            vec3.transformMat4(offset, vec3.fromValues(+0.1, 0, 0), ivMatrix);
            break;
        case 'D'.charCodeAt(0):
            vec3.transformMat4(offset, vec3.fromValues(-0.1, 0, 0), ivMatrix);
            break;
        case 'A'.charCodeAt(0):
            vec3.transformMat4(offset, vec3.fromValues(0, -0.1, 0), ivMatrix);
            break;
        case 'W'.charCodeAt(0):
            vec3.transformMat4(offset, vec3.fromValues(0, +0.1, 0), ivMatrix);
            break;
        default:
            // appeler la méthode de la superclasse
            super.onKeyDown(code);
        }

        // appliquer le décalage au pivot de la rotation
        vec3.add(this.m_CameraPivot, this.m_CameraPivot, offset);

        // afficher les informations sur l'écran
        this.updateInfos();
    }


    /**
     * dessine les objets de la scène
     * @param mat4Projection : matrice de projection
     * @param mat4View : matrice de vue
     */
    onDraw(mat4Projection, mat4View)
    {
        // dessiner le trièdre
        this.m_Axes.onDraw(mat4Projection, mat4View);

        // définir le pivot des rotations
        mat4.translate(mat4View, mat4View, this.m_CameraPivot);

        // dessiner le terrain
        let mat4ModelView = mat4.create();
        mat4.scale(mat4ModelView, mat4View, vec3.fromValues(2.0, 2.0, 2.0));
        this.m_Ground.onDraw(mat4Projection, mat4ModelView);

        // dessiner les palmiers
        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(2.5,0,-1.0));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(120.0));
        mat4.scale(mat4ModelView, mat4ModelView, vec3.fromValues(0.2, 0.2, 0.2));
        this.m_PalmTree.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(3.0,0,1.5));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(20.0));
        mat4.scale(mat4ModelView, mat4ModelView, vec3.fromValues(0.2, 0.2, 0.2));
        this.m_PalmTree.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(-1,0,-2.0));
        mat4.rotate(mat4ModelView, mat4ModelView, Utils.radians(80.0), vec3.fromValues(-1,0,-1));
        mat4.scale(mat4ModelView, mat4ModelView, vec3.fromValues(0.2, 0.2, 0.2));
        this.m_PalmTree.onDraw(mat4Projection, mat4ModelView);

        // dessiner les camions
        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(1.5,0.5,-1.5));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(-70.0));
        mat4.rotate(mat4ModelView, mat4ModelView, Utils.radians(82.0), vec3.fromValues(0,0,1));
        this.m_Lorry.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(-2,0,0.5));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(120.0));
        this.m_Lorry.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(-1,0,-1));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(320.0));
        mat4.scale(mat4ModelView, mat4ModelView, vec3.fromValues(1.5,3.0,0.5));
        this.m_Lorry.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(1.15,1.0,0.7));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(60.0));
        this.m_Lorry.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(1,0,1));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(-20.0));
        mat4.scale(mat4ModelView, mat4ModelView, vec3.fromValues(2.0, 2.0, 2.0));
        this.m_Lorry.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(-1,0,2));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(200.0));
        this.m_Lorry.onDraw(mat4Projection, mat4ModelView);
    }


    /**
     * Dessine l'image courante
     */
    onDrawFrame()
    {
        // effacer l'écran
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        // construire la matrice caméra
        let mat4View = mat4.create();

        // éloignement de la scène
        mat4.translate(mat4View, mat4View, vec3.fromValues(0.0, 0.0, -this.m_CameraDistance));

        // orientation de la scène par rapport à la caméra
        mat4.rotateX(mat4View, mat4View, Utils.radians(this.m_CameraAngleElevation));
        mat4.rotateY(mat4View, mat4View, Utils.radians(this.m_CameraAngleAzimut));

        // transformer les lampes
        this.prepareLights(mat4View);

        // dessiner les objets dans cette transformation
        this.drawDeferredShading(this.m_Mat4Projection, mat4View);

        // dessiner les éclairements des lampes
        this.addLightings();
    }


    /** supprime tous les objets créés */
    destroy()
    {
        this.m_Light2.destroy();
        this.m_Light1.destroy();
        this.m_Light0.destroy();
        this.m_Axes.destroy();
        this.m_PalmTree.destroy();
        this.m_Lorry.destroy();
        this.m_Ground.destroy();
        super.destroy();
    }
}
