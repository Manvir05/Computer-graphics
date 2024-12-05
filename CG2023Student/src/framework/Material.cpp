//
//  Material.cpp
//  ComputerGraphics
//
//  Created by Manvir Kaur on 10/03/23.
//

#include <stdio.h>
#include "Material.h"

Material::Material(){
    
}


Material::Material(Vector3 ka, Vector3 kd, Vector3 ks, float alpha, Shader* shader, Texture *specular, Texture *normal){
    this->ka = ka;
    this->kd = kd;
    this->ks = ks;
    this->alpha = alpha;
    this->shader = shader;
    this->specular = specular;
    this->normal = normal;
}

void Material::Enable(const uniformData& uniformData, int light_index){
    
    shader->Enable();
    
    shader->SetMatrix44("u_modelMatrix", uniformData.modelMatrix);
    shader->SetMatrix44("u_viewprojectionMatrix", uniformData.viewProjection);
    shader->SetVector3("u_Ia", uniformData.Ia);
    
    if(light_index == 0){
        shader->SetVector3("u_ka", ka);
    }
    
    shader->SetVector3("u_kd", kd);
    shader->SetVector3("u_ks", ks);
    
    /*
    shader->SetVector3("u_position", uniformData.light.position);
    shader->SetVector3("u_Id", uniformData.light.Id);
    shader->SetVector3("u_Is", uniformData.light.Is);
    */
    
    shader->SetVector3("u_position", uniformData.lightvec[light_index].position);
    shader->SetVector3("u_Id", uniformData.lightvec[light_index].Id);
    shader->SetVector3("u_Is", uniformData.lightvec[light_index].Is);
    
    shader->SetVector3("u_eye", uniformData.eye);
    shader->SetVector3("u_flag", uniformData.flag);
    
    shader->SetFloat("u_alpha", alpha);
    shader->SetTexture("u_specular", specular);
    shader->SetTexture("u_normal", normal);

}



void Material::Disable(){
    shader->Disable();
}

void Material::setShader(Shader* shader){
    this->shader = shader;
}

void Material::setSpecular(Texture* specular){
    this->specular = specular;
}

void Material::setNormal(Texture* normal){
    this->normal = normal;
}
