//
//  Material.h
//  ComputerGraphics
//
//  Created by Manvir Kaur on 10/03/23.
//

#ifndef Material_h
#define Material_h
#include "texture.h"
#include "shader.h"
#include "camera.h"
struct lightRepresentation{
    Vector3 position, Id, Is;
};

struct uniformData{
    Matrix44 modelMatrix;
    Matrix44 viewProjection;
    Vector3 Ia;
    Vector3 eye;
    //lightRepresentation light;
    std::vector<lightRepresentation> lightvec;
    int numLight;
    Vector3 flag;
};



class Material{
    
    public:
    //??
    Vector3 Ia;
    
    Shader* shader;
    Vector3 ka,kd,ks;
    float alpha;
    Texture * specular;
    Texture * normal;
    Material();
    Material(Vector3 ka, Vector3 kd, Vector3 ks, float alpha, Shader* shader, Texture *specular, Texture *normal);
    void Enable(const uniformData& uniformData, int light_index);
    
    void setSpecular(Texture* specular);
    void setNormal(Texture* normal);
    
    void Disable();
    void setShader(Shader* shader);
};


#endif /* Material_h */
