//
//  entity.cpp
//  ComputerGraphics
//
//  Created by Manvir Kaur on 27/01/23.
//

#include <stdio.h>
#include "entity.h"
#include "framework.h"
#include "image.h"


Mesh* mesh = new Mesh();

Entity::Entity(){
    texture = new Image();
}

Entity::Entity(Mesh* m){
    mesh = m;
}

Entity::Entity(Matrix44 mat, Mesh* m){
    modelMatrix = mat;
    mesh = m;
    
}

Entity::~Entity() {
 
}

/*
void Entity::Render(Image* framebuffer, Camera* camera, Color::c){
    
}
 */

//void Image::DrawTriangleInterpolated(const sTriangleInfo &triangle, FloatImage* zbuffer);
void Entity::Render(Image* framebuffer, Camera* camera,FloatImage* zBuffer){
    std::vector<Vector3> vertices = mesh->GetVertices();
    std::vector<Vector2> UVcord = mesh->GetUVs();
    
    for(int i=0; i<vertices.size();i+=3){
        Vector4 local_space1(vertices[i].x, vertices[i].y, vertices[i].z, 1);
        Vector4 local_space2(vertices[i+1].x, vertices[i+1].y, vertices[i+1].z, 1);
        Vector4 local_space3(vertices[i+2].x, vertices[i+2].y, vertices[i+2].z, 1);
        
        Vector2 uv0( UVcord[i].x, UVcord[i].y);
        Vector2 uv1( UVcord[i+1].x, UVcord[i+1].y);
        Vector2 uv2( UVcord[i+2].x, UVcord[i+2].y);
        
        
        
        Vector4 world_space1 = modelMatrix * local_space1;
        Vector4 world_space2 = modelMatrix * local_space2;
        Vector4 world_space3 = modelMatrix * local_space3;
        
        bool negZ1, negZ2, negZ3;
        
        Vector3 clip_space1 = camera->ProjectVector(Vector3(world_space1.x, world_space1.y, world_space1.z), negZ1);
        Vector3 clip_space2 = camera->ProjectVector(Vector3(world_space2.x, world_space2.y, world_space2.z), negZ2);
        Vector3 clip_space3 = camera->ProjectVector(Vector3(world_space3.x, world_space3.y, world_space3.z), negZ3);
        
        
        if (!negZ1 && !negZ2 && !negZ3){
            
            
            Vector2 screen_space1((clip_space1.x+1)/2 * framebuffer->width, (clip_space1.y +1)/2 * framebuffer->height);
            Vector2 screen_space2((clip_space2.x+1)/2 * framebuffer->width, (clip_space2.y +1)/2 * framebuffer->height);
            Vector2 screen_space3((clip_space3.x+1)/2 * framebuffer->width, (clip_space3.y +1)/2 * framebuffer->height);
            
            if(RenderMode==eRenderMode::POINTCLOUD){
                framebuffer->SetPixelSafe(screen_space1.x, screen_space1.y, Color::RED);
                framebuffer->SetPixelSafe(screen_space2.x, screen_space2.y, Color::RED);
                framebuffer->SetPixelSafe(screen_space3.x, screen_space3.y, Color::RED);
            }
            if(RenderMode==eRenderMode::WIREFRAME){
                framebuffer->DrawLineBresenham(screen_space1.x, screen_space1.y, screen_space2.x, screen_space2.y, Color::RED);
                framebuffer->DrawLineBresenham(screen_space2.x, screen_space2.y, screen_space3.x, screen_space3.y, Color::RED);
                framebuffer->DrawLineBresenham(screen_space3.x, screen_space3.y, screen_space1.x, screen_space1.y, Color::RED);
            }
            
            if(RenderMode==eRenderMode::TRIANGLES){
                framebuffer->DrawTriangle(screen_space1, screen_space2, screen_space3, Color::RED);
            }
            
            Vector3 screen_space_1 (screen_space1.x, screen_space1.y, clip_space1.z);
            Vector3 screen_space_2 (screen_space2.x, screen_space2.y, clip_space2.z);
            Vector3 screen_space_3 (screen_space3.x, screen_space3.y, clip_space3.z);
            
   
            if(RenderMode==eRenderMode::TRIANGLES_INTERPOLATED){
                Image::sTriangleInfo triangle = {screen_space_1, screen_space_2, screen_space_3,Color::RED, Color::GREEN, Color::BLUE, nullptr, uv0, uv1, uv2};
                framebuffer->DrawTriangleInterpolated(triangle, nullptr);
            }
            
            if(RenderMode==eRenderMode::TRIANGLES_INTERPOLATED_ZBUFFER){
                Image::sTriangleInfo triangle = {screen_space_1, screen_space_2, screen_space_3,Color::RED, Color::GREEN, Color::BLUE, nullptr, uv0, uv1, uv2};
                framebuffer->DrawTriangleInterpolated(triangle, zBuffer);
            }
            
            if(RenderMode==eRenderMode::TRIANGLES_INTERPOLATED_TEXTURE){
                Image::sTriangleInfo triangle = {screen_space_1, screen_space_2, screen_space_3,Color::RED, Color::GREEN, Color::BLUE, texture, uv0, uv1, uv2};
                framebuffer->DrawTriangleInterpolated(triangle, nullptr);
            }
            if(RenderMode==eRenderMode::TRIANGLES_INTERPOLATED_TEXTURE_ZBUFFER){
                Image::sTriangleInfo triangle = {screen_space_1, screen_space_2, screen_space_3,Color::RED, Color::GREEN, Color::BLUE, texture, uv0, uv1, uv2};
                framebuffer->DrawTriangleInterpolated(triangle, zBuffer);
            }
        }
    }
}

void Entity::Render(void){
    mesh->Render();
}

void Entity::Render(uniformData uniformData){
    uniformData.modelMatrix = modelMatrix;
    
    
    for(int i=0; i<uniformData.numLight;i++){
        material->Enable(uniformData,i);
        if(i==0){
            glDisable(GL_BLEND);
        }
        else{
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE);
        }
       
        mesh->Render();
    }
    /*
    material->Enable(uniformData);
    mesh->Render();
     */
    material->Disable();
}

    
void Entity::Update(float seconds_elapsed){
    if(rotate){
        modelMatrix.Rotate(seconds_elapsed, Vector3(0,1,0));
    }
    if(translateUP){
        modelMatrix.Translate(0.0, 0.005, 0.0);
    }
    if(translateDOWN){
        modelMatrix.Translate(0.0, -0.005, 0.0);
    }
    if(translateRIGHT){
        modelMatrix.Translate(0.005, 0.0, 0.0);
    }
    if(translateLEFT){
        modelMatrix.Translate(-0.005, 0.0, 0.0);
    }
}

void Entity::Setmaterial(Material * mat){
    material = mat;
}
