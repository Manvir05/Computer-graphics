//
//  Entity.h
//  ComputerGraphics
//
//  Created by Manvir Kaur on 27/01/23.
//


#ifndef Entity_h
#define Entity_h
#include "mesh.h"
#include "image.h"
#include "Material.h"


enum class eRenderMode {
        POINTCLOUD,
        WIREFRAME,
        TRIANGLES,
        TRIANGLES_INTERPOLATED,
        TRIANGLES_INTERPOLATED_ZBUFFER,
        TRIANGLES_INTERPOLATED_TEXTURE,
        TRIANGLES_INTERPOLATED_TEXTURE_ZBUFFER
};


class Entity{
public:
    
    bool rotate = false;
    bool translateUP = false;
    bool translateDOWN = false;
    bool translateRIGHT = false;
    bool translateLEFT = false;
    bool ex1 = true;
    bool ex2 = false;

    
    Mesh* mesh = NULL;
    Matrix44 modelMatrix;
    Material * material;
    
    //borrar
    Image* texture;
    
    Entity();
    Entity(Mesh* m);
    Entity(Matrix44 mat, Mesh* m);
    
    eRenderMode RenderMode;
    
    
   
    
    //Destructor
    ~Entity();
    
    //void Render(Image* framebuffer, Camera* camera, const Color& c);
    void Render(Image* framebuffer, Camera* camera, FloatImage* zBuffer);
    void Render(void);
    void Render(uniformData uniformData);
    void Update(float seconds_elapsed);
    void Setmaterial(Material * mat);
    
};

#endif /* Entity_h */
