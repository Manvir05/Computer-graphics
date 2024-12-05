#include "application.h"
#include "mesh.h"
#include "shader.h"
#include "utils.h"
#include "entity.h"

//ka=kd --> color of texture
//ks=kd --> ks-->how much specular
//Id=Is

//create one shader for each task
Shader* formulas;
Shader* images;
Shader* transformations;
Shader* render;
Shader* current;

Mesh* quad;

int tasks;

int option;

//lab5
Shader* Gouraud;
Shader* Phong;

Texture * texture;
Texture * tmesh;

Texture * tspecularAnna;
Texture * tnormalAnna;

Texture * tspecularLee;
Texture * tnormalLee;

Texture * tspecularCleo;
Texture * tnormalCleo;

int color_t = 0;
int normal_t = 0;

int meshID;
int lightID;
bool movelight = false;
bool moving = true;
Application::Application(const char* caption, int width, int height)
{
	this->window = createWindow(caption, width, height);

	int w,h;
	SDL_GetWindowSize(window,&w,&h);

	this->mouse_state = 0;
	this->time = 0.f;
	this->window_width = w;
	this->window_height = h;
	this->keystate = SDL_GetKeyboardState(nullptr);
    this->aspect = window_width/(float)window_height;
	this->framebuffer.Resize(w, h);

}

Application::~Application()
{
}

void Application::Init(void)
{
    std::cout << "Initiating app..." << std::endl;
    
    selCol = Color::RED;
    if(lab1){
        ps.Init();
        //by default values
        previous.set(-1,-1);
        thickness = 1;
        ontop = true;
        fillCircle = true;
        
        draw_Circle = false;
        draw_Line = false;
        paint = true;
        if(im.LoadPNG("images/toolbar.png", true)==false){
            printf("Image not found");
        }
        else{
            im.LoadPNG("images/toolbar.png",true);
        }
    }
    if(lab2==true || lab3==true || lab4==true || lab5==true ){
        camera = new Camera();
        camera->LookAt(Vector3(0.0,0.2,0.5), Vector3(0.0,0.25,0.0), Vector3::UP);
        //camera->LookAt(Vector3(0.0,0.0,0.5) ,Vector3(0.0,0.25,0),Vector3::UP);
        
        camera->SetPerspective(60, window_width/(float)window_height, 0.01, 20);
        
        camera->UpdateViewMatrix();
        camera->UpdateProjectionMatrix();
        entities.resize(3);
        
        object = new Mesh();
        object->LoadOBJ("meshes/anna.obj");
        
        object2 = new Mesh();
        object2->LoadOBJ("meshes/lee.obj");
        
        object3 = new Mesh();
        object3->LoadOBJ("meshes/cleo.obj");
        
        std::vector<Mesh*> meshes = {object, object2, object3};
        
        Matrix44 aux_model;
        aux_model.SetIdentity();
        
        col.resize(3);
        for (int i = 0; i < 3; i++) {
            entities[i].modelMatrix = aux_model;
            entities[i].mesh = meshes[i];
            col[i] = RandomColor();
        }
        if(!lab5){
            entities[0].modelMatrix.Translate(0.3, -0.3, -1);
            entities[1].modelMatrix.Translate(0, -0.3, -1);
            entities[2].modelMatrix.Translate(-0.3,-0.3, -1);
        }
        
    }
    
    if(lab3){
        zbuffer = new FloatImage(framebuffer.width, framebuffer.height);
        entities[0].texture->LoadTGA("textures/anna_color_specular.tga",true);
        entities[1].texture->LoadTGA("textures/lee_color_specular.tga",true);
        entities[2].texture->LoadTGA("textures/cleo_color_specular.tga",true);
    }
    
    if(lab4==true )
    {
        //Quad creation
        quad = new Mesh();
        quad->CreateQuad();

        //exercise 2
        texture = new Texture();
        tmesh = new Texture();
        
        formulas = Shader::Get("shaders/quad.vs", "shaders/form.fs");
        images = Shader::Get("shaders/quad.vs", "shaders/img.fs");
        transformations = Shader::Get("shaders/quad.vs", "shaders/transf.fs");
        render = Shader::Get("shaders/raster.vs", "shaders/raster.fs");
        
        texture->Load("images/fruits.png");
        tmesh->Load("textures/lee_color_specular.tga");
        
        current = formulas;
        option = 0;
    }
    if(lab5){
        
        tspecularLee = new Texture();
        tnormalLee = new Texture();
        
        tspecularAnna = new Texture();
        tnormalAnna = new Texture();
        
        tspecularCleo = new Texture();
        tnormalCleo = new Texture();
        
        //Lee
        tspecularLee->Load("textures/lee_color_specular.tga");
        tnormalLee->Load("textures/lee_normal.tga");
        //Anna
        tspecularAnna->Load("textures/anna_color_specular.tga");
        tnormalAnna->Load("textures/anna_normal.tga");
        //Cleo
        tspecularCleo->Load("textures/cleo_color_specular.tga");
        tnormalCleo->Load("textures/cleo_normal.tga");
        
        meshID = 0;
        Vector3 ka(1.0,1.0,1.0);
        Vector3 kd(1.0,1.0,1.0);
        Vector3 ks(1.0,1.0,1.0);
        float alpha(60);
        
        Gouraud = Shader::Get("shaders/gouraud.vs", "shaders/gouraud.fs");
        Phong = Shader::Get("shaders/phong.vs","shaders/phong.fs");
        

        material = new Material(ka, kd, ks, alpha, Phong, tspecularAnna, tnormalAnna);
        entities[meshID].material = material;

        unidata.numLight = 4;
        
        unidata.lightvec.resize(4);
        unidata.lightvec[0].position.Set(1, 1, 1);
        unidata.lightvec[0].Id.Set(1.0, 1.0, 1.0);
        unidata.lightvec[0].Is.Set(1.0, 1.0, 1.0);
        
        
        unidata.flag.Set(color_t, normal_t,1.0);

        unidata.Ia.Set(0.1, 0.1,0.1);
        unidata.viewProjection = camera->viewprojection_matrix;
        unidata.eye = camera->eye;
    
        lightID = 0;
    }
    
}

// Render one frame
void Application::Render(void)
{
    if(lab1){
        
        if(exercise == Ex1){
            //
        }
        if(exercise == Ex2){
            int centerx = this->window_width / 2;
            int centery = this->window_height / 2;
            framebuffer.Fill(Color::BLACK);
            framebuffer.DrawThickLineBresenham(centerx, centery, centerx + 200 * cos(time), centery + 200 * sin(time),thickness, selCol);
        }
        if(exercise == Ex3){
            
        }
        if(exercise == Ex4 && paint==true){
            framebuffer.DrawImagePixels(im, 0, 0, ontop);
            
            if(mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT) && paint==true){
                if(previous.x!=-1 && previous.y!=-1){
                    framebuffer.DrawThickLineDDA(previous.x, previous.y, mouse_position.x, mouse_position.y,thickness, c);
                }
            }
            previous.x= mouse_position.x;
            previous.y=mouse_position.y;
        }
        if(exercise == Ex5){
            ps.Render(framebuffer);
        }
    }
    if(lab2){
        //render of entity
        for (int i = 0; i < entities.size(); i++) {
            //entities[i].Render(&framebuffer, camera, col[i]);
        }
    }
    if(lab3){
        zbuffer->Fill(100000000.0);
        for(int i=0; i< entities.size();i++){
            entities[i].Render(&framebuffer, camera, zbuffer);
        }
    }
    
    if(lab1==true || lab2==true || lab3==true){
        framebuffer.Render();
    }
    
    if(lab4){
        current->Enable();
        
        if(current==render){
            current->SetTexture("u_texture", tmesh);
            current->SetMatrix44("u_modelMatrix", entities[1].modelMatrix);
            current->SetMatrix44("u_viewprojectionMatrix", camera->projection_matrix);
            current->SetUniform1("u_aspect", aspect);
            entities[1].Render();
            
        }else{
            current->SetFloat("u_time", time);
            current->SetUniform1("option", option);
            current->SetTexture("u_texture", texture);
            quad->Render();
            current->SetUniform1("u_aspect", aspect);
            
        }
        current->Disable();
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
    }
    if(lab5){

        entities[meshID].Render(unidata);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        
        unidata.viewProjection = camera->viewprojection_matrix;
        unidata.eye = camera->eye;
    }
}

// Called after render
void Application::Update(float seconds_elapsed)
{
    framebuffer.Fill(Color::BLACK);
    if(exercise==Ex5 && lab1==true){
        ps.Update(0.2);
    }
    if(lab2==true || lab3==true){
        for (int i = 0; i < entities.size(); i++) {
            entities[i].Update(time_);
        }
    }
}

//keyboard press event 
void Application::OnKeyPressed( SDL_KeyboardEvent event )
{
    if(lab1){
        // KEY CODES: https://wiki.libsdl.org/SDL2/SDL_Keycode
        switch(event.keysym.sym) {
            case SDLK_ESCAPE: exit(0); break; // ESC key, kill the app
            case SDLK_1: exercise = Ex1;framebuffer.Fill(Color::BLACK); break;
            case SDLK_2: exercise  = Ex2;framebuffer.Fill(Color::BLACK); break;
            case SDLK_3: exercise  = Ex3;framebuffer.Fill(Color::BLACK); break;
            case SDLK_4: exercise  = Ex4;framebuffer.Fill(Color::WHITE); break;
            case SDLK_5: exercise = Ex5;framebuffer.Fill(Color::BLACK); break;
        }
        
        //COLOR
        if(exercise!=Ex4){
            switch(event.keysym.sym){
                case SDLK_r: selCol=Color::RED; break;
                case SDLK_g: selCol=Color::GREEN; break;
                case SDLK_b: selCol=Color::BLUE; break;
                case SDLK_c: selCol=Color::CYAN; break;
                case SDLK_p: selCol=Color::PURPLE; break;
                case SDLK_y: selCol=Color::YELLOW; break;
                case SDLK_w: selCol=Color::WHITE;break;
                case SDLK_x: selCol=RandomColor();break;
            }
        }
        //Change thickness of lines
        if(exercise==Ex1  || exercise==Ex2 || exercise==Ex4){
            switch(event.keysym.sym){
                case SDLK_s: thickness=1; break;
                case SDLK_m: thickness=5; break;
                case SDLK_l: thickness=10; break;
            }
        }
        
        //Circles filled or not
        if(exercise==Ex3 ||(exercise==Ex4 && draw_Circle==true)){
            switch(event.keysym.sym) {
                case SDLK_f: fillCircle=true; break;
                case SDLK_o: fillCircle=false; break;
            }
        }
        
        //d to draw the toolbaar down, and u to draw the toolbaar up, c to draw one circle, r to draw a line
        if(exercise==Ex4){
            switch(event.keysym.sym) {
                case SDLK_d:framebuffer.Fill(Color::WHITE);ontop=false;break;
                case SDLK_u:framebuffer.Fill(Color::WHITE);ontop=true;break;
                case SDLK_c:draw_Circle=true;draw_Line=false;paint=false;break;
                case SDLK_r:draw_Line=true;draw_Circle=false;paint=false;break;
            }
        }
    }
    
    if(lab2==true || lab3==true){
        
        //interactive: Orthographic and perspective, far and near planes, zoom and orbit, field of view
        
        switch (event.keysym.sym){
            case SDLK_ESCAPE: exit(0); break; // ESC key, kill the app
            case SDLK_o:camera->type = camera->ORTHOGRAPHIC;camera->UpdateProjectionMatrix();break;
            case SDLK_p:camera->type = camera->PERSPECTIVE;camera->UpdateProjectionMatrix();break;
            case SDLK_f:camera->far_plane -= 0.1 ;camera->near_plane -= 0.1;camera->UpdateProjectionMatrix();break;
            case SDLK_n:camera->near_plane += 0.1 ; camera->far_plane += 0.1 ;camera->UpdateProjectionMatrix();break;
            case SDLK_i:eyeCenter=true;orbit=false;break; //zoom in zoom out
            case SDLK_q:orbit=true;eyeCenter=false;break; //orbit
            //case SDLK_u:if(camera->type==camera->PERSPECTIVE){camera->fov+=5;camera->UpdateProjectionMatrix();}break;
            //case SDLK_d:if(camera->type==camera->PERSPECTIVE){camera->fov-=5;camera->UpdateProjectionMatrix();}break;
        }
    
        switch (event.keysym.sym){
            case SDLK_1:meshNum=2;all=false;break;
            case SDLK_2:meshNum=0;all=false;break;
            case SDLK_3:meshNum=1;all=false;break;
            case SDLK_a:all=true;break;
            case SDLK_m:time_ += 0.05;break;
            case SDLK_l:time_ -= 0.05;if(time_<0){time_=0.0;}break;
        }
        
        switch (event.keysym.sym){
            case SDLK_r:setRot();break;
            case SDLK_UP:setUP();break;
            case SDLK_DOWN:setDown();break;
            case SDLK_SPACE:stopEv();break;
            case SDLK_LEFT:setLeft();break;
            case SDLK_RIGHT:setRight();break;
            case SDLK_PLUS:
                if(all==false){entities[meshNum].modelMatrix.Scale(1.2, 1.2 , 1.2);}
                else{for (int i = 0; i < entities.size(); i++) {entities[i].modelMatrix.Scale(1.2, 1.2 , 1.2);}}
                break;
            case SDLK_MINUS:
                if(all==false){entities[meshNum].modelMatrix.Scale(0.8, 0.8 , 0.8);}
                else{for (int i = 0; i < entities.size(); i++) {entities[i].modelMatrix.Scale(0.8, 0.8 , 0.8);}}
                break;
        }
    }
    if(lab3){
        switch (event.keysym.sym){
            case SDLK_c:
                for (int i = 0; i < entities.size(); i++) {
                    if (isInterpolated) {entities[i].RenderMode = eRenderMode::TRIANGLES_INTERPOLATED;}
                    else {entities[i].RenderMode = eRenderMode::TRIANGLES;}}
                isInterpolated = !isInterpolated;break;
            
            case SDLK_z:
                for (int i = 0; i < entities.size(); i++) {
                    if (isInterpolatedZbuffer) {entities[i].RenderMode = eRenderMode::TRIANGLES_INTERPOLATED_ZBUFFER;}
                    else {entities[i].RenderMode = eRenderMode::TRIANGLES_INTERPOLATED;}}
                isInterpolatedZbuffer = !isInterpolatedZbuffer;break;
            
            case SDLK_t:
                for (int i = 0; i < entities.size(); i++){
                    if (isInterpolatedTextureZbuffer) {entities[i].RenderMode = eRenderMode::TRIANGLES_INTERPOLATED_TEXTURE_ZBUFFER;}
                    else {entities[i].RenderMode = eRenderMode::TRIANGLES_INTERPOLATED_TEXTURE;}}
                isInterpolatedTextureZbuffer = !isInterpolatedTextureZbuffer;break;
            
            case SDLK_d:for (int i = 0; i < entities.size(); i++){entities[i].RenderMode = eRenderMode::POINTCLOUD;}
                break;
                
            case SDLK_w:for (int i = 0; i < entities.size(); i++){entities[i].RenderMode = eRenderMode::WIREFRAME;}
                break;
        }
    }
    if (lab4) {
        switch (event.keysym.sym) {
        case SDLK_ESCAPE: exit(0); break;
        case SDLK_1: option = (option+1)%6 ; current = formulas;break;
        case SDLK_2: option = (option+1)%6 ; current = images;break;
        case SDLK_3: option = (option+1)%2 ; current = transformations;break;
        case SDLK_4: current = render ; break;
        }
    }
    if(lab5){
        switch (event.keysym.sym) {
            case SDLK_ESCAPE: exit(0); break;
            case SDLK_1: material->setShader(Gouraud) ; entities[1].Setmaterial(material); break;
            case SDLK_2: material->setShader(Phong) ; entities[1].Setmaterial(material); break;
                //first and second elements of the vec2 flag
            case SDLK_s: color_t = (color_t+1)%2;unidata.flag.x = color_t;break;
            case SDLK_n: normal_t = (normal_t+1)%2;unidata.flag.y = normal_t;break;
    
            case SDLK_c: material->setNormal(tnormalCleo); material->setSpecular(tspecularCleo); entities[2].Setmaterial(material) ; meshID = 2;break;
            case SDLK_l: material->setNormal(tnormalLee); material->setSpecular(tspecularLee);entities[1].Setmaterial(material);meshID = 1;break;
            case SDLK_a: material->setNormal(tnormalAnna); material->setSpecular(tspecularAnna);entities[0].Setmaterial(material);meshID = 0;break;
            //first press moves light, second press stops the light
            case SDLK_m:
                if(moving){
                    movelight=true;
                }else{
                    movelight = false;
                }
                moving = !moving;
                 break;
            case SDLK_o: orbit=true ; eyeCenter=false ; break;
            case SDLK_z: orbit=false ; eyeCenter=true ; break;
            case SDLK_RETURN:lightID = (lightID+1)%(unidata.numLight+1);setlight(lightID);break;
        }
    }
}

void Application::OnMouseButtonDown( SDL_MouseButtonEvent event )
{
    if(lab1){
        if (event.button == SDL_BUTTON_LEFT) {
            if (exercise==Ex1 || (exercise==Ex4 && draw_Line==true)) {
                if (!StartSelected) {
                    Start.set(mouse_position.x, mouse_position.y);
                    StartSelected = true;
                } else if (StartSelected && !EndSelected) {
                    End.set(mouse_position.x, mouse_position.y);
                    EndSelected = true;
                    
                    Color color;
                    if(exercise==Ex4){
                        color = c;
                    }
                    else{
                        color = selCol;
                    }
                    framebuffer.DrawThickLineDDA(Start.x, Start.y, End.x, End.y,thickness, color);
                    StartSelected = false;
                    EndSelected = false;
                    draw_Line = false;
                    paint = true;
                }
            }
        }
        //toolbar
        int mouseX = event.x;
        int s = 50;
        int buttonID = mouseX/s;
        
        int mouseY=event.y;
        
        if(ontop){
            if(margin <= mouseY  && mouseY<= margin+iconsize){
                SetButtons(buttonID);
            }
        }else{
            if(window_height-margin >= mouseY && mouseY >= window_height-(margin+iconsize)){
                SetButtons(buttonID);
            }
        }
    }
}

void Application::OnMouseButtonUp( SDL_MouseButtonEvent event )
{
    if(lab1){
        if (event.button == SDL_BUTTON_LEFT) {
            if(exercise == Ex3 || (exercise==Ex4 && draw_Circle==true)){
                if (!StartSelected) {
                    Start.set(mouse_position.x, mouse_position.y);
                    StartSelected = true;
                } else if (StartSelected && !EndSelected) {
                    End.set(mouse_position.x, mouse_position.y);
                    EndSelected = true;
                    Color color;
                    if(exercise==Ex4){
                        color = c;
                    }
                    else{
                        color = selCol;
                    }
                    double radius = sqrt(pow(End.x - Start.x , 2) + pow(End.y - Start.y, 2));
                    framebuffer.DrawCircle(Start.x, Start.y , radius, color, fillCircle);
                    StartSelected = false;
                    EndSelected = false;
                    draw_Circle = false;
                    paint = true;
                }
            }
        }
    }
}

void Application::OnMouseMove(SDL_MouseButtonEvent event)
{
    //with mouse
    
    if(lab5==true && orbit==true){
        if (event.button == SDL_BUTTON_LEFT) {
            camera->Orbit(-mouse_delta.x * 0.01, Vector3::UP);
            camera->Orbit(-mouse_delta.y * 0.01, Vector3::RIGHT);
        }
        else if (event.button == SDL_BUTTON_MIDDLE) {
            camera->Move(Vector3(-mouse_delta.x * 0.001, mouse_delta.y * 0.001, 0));
        }
    }
     
    if(lab5==true && movelight==true){
        Matrix44 R;
        R.SetRotation(mouse_delta.x*0.01, Vector3::UP);
        Vector3 new_front = R * (unidata.lightvec[lightID].position - camera->center);
        unidata.lightvec[lightID].position = camera->center + new_front;

        Matrix44 T;
        T.SetRotation(-mouse_delta.y * 0.01, Vector3::RIGHT);
        Vector3 new_frontT = T * (unidata.lightvec[lightID].position - camera->center);
        unidata.lightvec[lightID].position = camera->center + new_frontT;
    }

}


void Application::OnWheel(SDL_MouseWheelEvent event)
{
	float dy = event.preciseY;
    float dx = event.preciseX;

    if(lab2==true || lab3==true || lab5==true){
        if(eyeCenter){ //also zoom
            eyeCenterInteract(dx, dy);
        }
        //orbit without mouse
        if(orbit){
            Orbitcamera(dx, dy);
        }
    }
    //if(lab5==true && eyeCenter==true){ //zoom
      //  camera->Zoom(dy < 0 ? 1.1 : 0.9);
   // }
}

void Application::OnFileChanged(const char* filename)
{ 
	Shader::ReloadSingleShader(filename);
}

void Application::SetButtons(int bID)
{
 switch(bID){
     case 0:
         framebuffer.Fill(Color::WHITE);
     break;
     case 1:
         if(framebuffer.SaveTGA("image.tga")==false){
             printf("Not saved");
         };
     break;
     case 2:c=Color::BLACK;break;
     case 3:c=Color::RED;break;
     case 4:c=Color::GREEN;break;
     case 5:c=Color::BLUE;break;
     case 6:c=Color::YELLOW;break;
     case 7:c=Color::PURPLE;break;
     case 8:c=Color::CYAN;break;
     case 9:c=Color::WHITE;break;
    }
}
Color Application::RandomColor(){
    return Color(rand() % 255, rand() % 255, rand() % 255);
}

//ParticleSystem class methods
void ParticleSystem::Init()
{
    for (int i = 0; i < 100; i++) {
        p[i].position.x = rand() % (1200 - 60) + 30;
        p[i].position.y = rand() % (700 - 80) + 40;
        p[i].velocity.x = rand() % 5;
        p[i].velocity.y = rand() % 5;
        p[i].radius =rand() % 10 + 2;
    }
}

void ParticleSystem::Render(Image& framebuffer)
{
    for (int i = 0; i < 100; i++) {
        framebuffer.DrawCircle(p[i].position.x, p[i].position.y, p[i].radius,  Color(rand() % 256, rand() % 256, rand() % 256), true);
    }
}

void ParticleSystem::Update(float seconds_elapsed)
{
    for (int i = 0; i < 100; i++) {
        //update the position
        p[i].position.x = p[i].position.x + p[i].velocity.x * seconds_elapsed;
        p[i].position.y = p[i].position.y + p[i].velocity.y * seconds_elapsed;
        
        // Check for collision with boundaries
        if (p[i].position.x - p[i].radius < 0 || p[i].position.x + p[i].radius > 1200) {
            p[i].velocity.x = -p[i].velocity.x;
        }
        if (p[i].position.y - p[i].radius < 0 || p[i].position.y + p[i].radius > 700) {
            p[i].velocity.y = -p[i].velocity.y;
        }

        // Check for collision with other particles
        for (int j = i + 1; j < 100; j++) {
            float distance = sqrt(pow(p[i].position.x - p[j].position.x, 2) + pow(p[i].position.y - p[j].position.y, 2));
            if (distance < p[i].radius + p[j].radius) {
                p[i].velocity = p[i].velocity * -1;
                p[j].velocity = p[j].velocity * -1;
            }
        }
    }
}


void Application::eyeCenterInteract(float dx,float dy){
    
    Vector3 camera_position = camera->eye;
    Vector3 camera_target = camera->center;

    Vector3 direction = camera_target - camera_position;
    float distance = direction.Length();
    direction.Normalize();
 
    camera_position = camera_position + direction * dy * distance * 0.07;

    camera->LookAt(camera_position, camera_target, Vector3::UP);
}


void Application::Orbitcamera(float dx,float dy){
    //static to not reset it to 0 everytime the function is called
    
    static float a = 0; //controls the rotation around the Y-axis
    static float b = 0; //controls the rotation around the X-axis.

    a += dx * 0.01;
    b -= dy * 0.01;

    float radius = 0.6; //control the distance between the camera and the center point that it is orbiting around
    //cartesian coordinates of sphere
    float x = radius * sinf(b) * cosf(a);
    float y = radius * cosf(b);
    float z = radius * sinf(b) * sinf(a); //in radians

    Vector3 eye(x, y, z);

    camera->LookAt(eye, camera->center, camera->up);
}

void Application::setRot(){
    if(all==false){
        entities[meshNum].rotate=true;entities[meshNum].translateUP = false; entities[meshNum].translateDOWN=false;entities[meshNum].translateLEFT=false;
        entities[meshNum].translateRIGHT=false;
    }
    else{
        for (int i = 0; i < entities.size(); i++) {
            entities[i].rotate=true;entities[i].translateUP = false; entities[i].translateDOWN=false;entities[i].translateLEFT=false;
            entities[i].translateRIGHT=false;
        }
    }
}

void Application::setUP(){
    if(all==false){
        entities[meshNum].translateUP=true; entities[meshNum].rotate = false; entities[meshNum].translateDOWN=false;entities[meshNum].translateLEFT=false;
        entities[meshNum].translateRIGHT=false;
    }
    else{
        for (int i = 0; i < entities.size(); i++) {
            entities[i].translateUP=true; entities[i].rotate = false; entities[i].translateDOWN=false;entities[i].translateLEFT=false;
            entities[i].translateRIGHT=false;
        }
    }
}

void Application::setDown(){
    if(all==false){
        entities[meshNum].translateDOWN=true; entities[meshNum].translateUP=false;entities[meshNum].rotate = false; entities[meshNum].translateLEFT=false;entities[meshNum].translateRIGHT=false;
    }
    else{
        for (int i = 0; i < entities.size(); i++) {
            entities[i].translateDOWN=true; entities[i].translateUP=false;entities[i].rotate = false; entities[i].translateLEFT=false;entities[i].translateRIGHT=false;
        }
    }
}

void Application::stopEv(){
    if(all==false){
        entities[meshNum].translateDOWN=false;entities[meshNum].translateUP=false;entities[meshNum].rotate = false; entities[meshNum].translateLEFT=false;entities[meshNum].translateRIGHT=false;
    }
    else{
        for (int i = 0; i < entities.size(); i++) {
            entities[i].translateDOWN=false;entities[i].translateUP=false;entities[i].rotate = false; entities[i].translateLEFT=false;entities[i].translateRIGHT=false;
        }
    }
}

void Application::setLeft(){
    if(all==false){
        entities[meshNum].translateLEFT=true;entities[meshNum].translateDOWN=false; entities[meshNum].translateUP=false;entities[meshNum].rotate = false; entities[meshNum].translateRIGHT=false;
    }
    else{
        for (int i = 0; i < entities.size(); i++) {
            entities[i].translateLEFT=true;entities[i].translateDOWN=false; entities[i].translateUP=false;entities[i].rotate = false; entities[i].translateRIGHT=false;
        }
    }
}

void Application::setRight(){
    if(all==false){
        entities[meshNum].translateRIGHT=true;entities[meshNum].translateDOWN=false; entities[meshNum].translateUP=false;entities[meshNum].rotate = false; entities[meshNum].translateLEFT=false;
    }
    else{
        for (int i = 0; i < entities.size(); i++) {
            entities[i].translateRIGHT=true;entities[i].translateDOWN=false; entities[i].translateUP=false;entities[i].rotate = false; entities[i].translateLEFT=false;
        }
    }
}

void Application::setlight(int lightID){
    for (int i = 0; i < unidata.numLight; i++) {
        unidata.lightvec[i].position.Set(rand() % 17 - 8, rand() % 17 - 8, rand() % 17 - 8);
        unidata.lightvec[i].Id.Set((float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX);
        unidata.lightvec[i].Is.Set((float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX);
    }
    if(lightID==unidata.numLight){
        unidata.lightvec[0].position.Set(1, 1, 1);
        unidata.lightvec[0].Id.Set(1.0, 1.0, 1.0);
        unidata.lightvec[0].Is.Set(1.0, 1.0, 1.0);
        for(int i=1; i<unidata.numLight;i++){
            unidata.lightvec[i].Id.Set(0.0, 0.0, 0.0);
            unidata.lightvec[i].Is.Set(0.0, 0.0, 0.0);
        }
    }
}
