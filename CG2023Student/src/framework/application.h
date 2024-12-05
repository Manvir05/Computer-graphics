/*  
	+ This class encapsulates the application, is in charge of creating the data, getting the user input, process the update and render.
*/

#pragma once

#include "main/includes.h"
#include "framework.h"
#include "image.h"
#include "entity.h"
#include "Material.h"


enum exercises {
    Ex1, Ex2, Ex3, Ex4, Ex5
};

class ParticleSystem
{
    typedef struct Particle {
        Vector2 position;
        unsigned int size;
        Vector2 velocity;
        //we had to add a radius here to do the manage collision
        int radius;
    } particle;

public:

    particle p[1000];
    Image framebuffer;

    void Init();
    void Render(Image& framebuffer);
    void Update(float seconds_elapsed);

};

class Application
{
public:

    bool lab1 = false;
    bool lab2 = false;
    bool lab3 = false;
    bool lab4 = false;
    bool lab5 = true;
    
    //FIRST LAB
    int margin=5;
    int iconsize=50;
    
    Color c;

    //to draw selecting the start and end points
    bool StartSelected = false;
    bool EndSelected = false;
    Vector2 Start;
    Vector2 End;
   
    bool ontop;
    Image im;
    bool fillCircle;
    Color selCol;
    int thickness;
    
    bool draw_Circle;
    bool draw_Line;
    bool paint;
    
    //to know the previous point of mouse to draw
    Vector2 previous;
    //to know the first point selected to draw lines and circles
    ParticleSystem ps;
    exercises exercise;
    
    
    //SECOND LAB
    std::vector<Entity> entities;
    std::vector<Color> col;
    
    Camera* camera = NULL;
    Mesh* object = NULL;
    Mesh* object2 = NULL;
    Mesh* object3 = NULL;
    
    bool eyeCenter=false;
    bool orbit=false;
    int meshNum=0;
    bool all = false;
    float time_ = 0.05;

    //lab3
    FloatImage* zbuffer;
    bool isInterpolated = false;
    bool isInterpolatedZbuffer = false;
    bool isInterpolatedTextureZbuffer = false;

    //lab4
    float aspect;
    
    //lab5
    
    Material * material;
    uniformData unidata;
    lightRepresentation light;
    Vector3 Ia;
    
    Vector3 Ip;

	// Window

	SDL_Window* window = nullptr;
	int window_width;
	int window_height;

	float time;

	// Input
	const Uint8* keystate;
	int mouse_state; // Tells which buttons are pressed
	Vector2 mouse_position; // Last mouse position
	Vector2 mouse_delta; // Mouse movement in the last frame

	void OnKeyPressed(SDL_KeyboardEvent event);
	void OnMouseButtonDown(SDL_MouseButtonEvent event);
	void OnMouseButtonUp(SDL_MouseButtonEvent event);
	void OnMouseMove(SDL_MouseButtonEvent event);
	void OnWheel(SDL_MouseWheelEvent event);
	void OnFileChanged(const char* filename);
    void SetButtons(int bID);
    Color RandomColor();
    void setlight(int lightID);

	// CPU Global framebuffer
	Image framebuffer;

	// Constructor and main methods
	Application(const char* caption, int width, int height);
	~Application();

	void Init( void );
	void Render( void );
	void Update( float dt);

	// Other methods to control the app
	void SetWindowSize(int width, int height) {
		glViewport( 0,0, width, height );
		this->window_width = width;
		this->window_height = height;
        framebuffer.Resize(width, height);
        aspect = window_width/(float)window_height;
        camera->SetAspectRatio(aspect);
        camera->UpdateViewProjectionMatrix();
        //camera->UpdateProjectionMatrix();
        unidata.viewProjection = camera->GetViewProjectionMatrix();
        
	}

	Vector2 GetWindowSize()
	{
		int w,h;
		SDL_GetWindowSize(window,&w,&h);
		return Vector2(float(w), float(h));
	}
    
    //lab2 functions
    void eyeCenterInteract(float dx,float dy);
    void Orbitcamera(float dx,float dy);
    void setRot();
    void setUP();
    void setDown();
    void stopEv();
    void setLeft();
    void setRight();
};
