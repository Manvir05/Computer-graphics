#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "GL/glew.h"
#include "../extra/picopng.h"
#include "image.h"
#include "utils.h"
#include "camera.h"
#include "mesh.h"

Image::Image() {
    width = 0; height = 0;
    pixels = NULL;
}

Image::Image(unsigned int width, unsigned int height)
{
    this->width = width;
    this->height = height;
    pixels = new Color[width*height];
    memset(pixels, 0, width * height * sizeof(Color));
}

// Copy constructor
Image::Image(const Image& c)
{
    pixels = NULL;
    width = c.width;
    height = c.height;
    bytes_per_pixel = c.bytes_per_pixel;
    if(c.pixels)
    {
        pixels = new Color[width*height];
        memcpy(pixels, c.pixels, width*height*bytes_per_pixel);
    }
}

// Assign operator
Image& Image::operator = (const Image& c)
{
    if(pixels) delete pixels;
    pixels = NULL;

    width = c.width;
    height = c.height;
    bytes_per_pixel = c.bytes_per_pixel;

    if(c.pixels)
    {
        pixels = new Color[width*height*bytes_per_pixel];
        memcpy(pixels, c.pixels, width*height*bytes_per_pixel);
    }
    return *this;
}

Image::~Image()
{
    if(pixels)
        delete pixels;
}

void Image::Render()
{
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glDrawPixels(width, height, bytes_per_pixel == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}

// Change image size (the old one will remain in the top-left corner)
void Image::Resize(unsigned int width, unsigned int height)
{
    Color* new_pixels = new Color[width*height];
    unsigned int min_width = this->width > width ? width : this->width;
    unsigned int min_height = this->height > height ? height : this->height;

    for(unsigned int x = 0; x < min_width; ++x)
        for(unsigned int y = 0; y < min_height; ++y)
            new_pixels[ y * width + x ] = GetPixel(x,y);

    delete pixels;
    this->width = width;
    this->height = height;
    pixels = new_pixels;
}

// Change image size and scale the content
void Image::Scale(unsigned int width, unsigned int height)
{
    Color* new_pixels = new Color[width*height];

    for(unsigned int x = 0; x < width; ++x)
        for(unsigned int y = 0; y < height; ++y)
            new_pixels[ y * width + x ] = GetPixel((unsigned int)(this->width * (x / (float)width)), (unsigned int)(this->height * (y / (float)height)) );

    delete pixels;
    this->width = width;
    this->height = height;
    pixels = new_pixels;
}

Image Image::GetArea(unsigned int start_x, unsigned int start_y, unsigned int width, unsigned int height)
{
    Image result(width, height);
    for(unsigned int x = 0; x < width; ++x)
        for(unsigned int y = 0; y < height; ++x)
        {
            if( (x + start_x) < this->width && (y + start_y) < this->height)
                result.SetPixel( x, y, GetPixel(x + start_x,y + start_y) );
        }
    return result;
}

void Image::FlipY()
{
    int row_size = bytes_per_pixel * width;
    Uint8* temp_row = new Uint8[row_size];
#pragma omp simd
    for (int y = 0; y < height * 0.5; y += 1)
    {
        Uint8* pos = (Uint8*)pixels + y * row_size;
        memcpy(temp_row, pos, row_size);
        Uint8* pos2 = (Uint8*)pixels + (height - y - 1) * row_size;
        memcpy(pos, pos2, row_size);
        memcpy(pos2, temp_row, row_size);
    }
    delete[] temp_row;
}

bool Image::LoadPNG(const char* filename, bool flip_y)
{
    std::string sfullPath = absResPath(filename);
    std::ifstream file(sfullPath, std::ios::in | std::ios::binary | std::ios::ate);

    // Get filesize
    std::streamsize size = 0;
    if (file.seekg(0, std::ios::end).good()) size = file.tellg();
    if (file.seekg(0, std::ios::beg).good()) size -= file.tellg();

    if (!size)
        return false;

    std::vector<unsigned char> buffer;

    // Read contents of the file into the vector
    if (size > 0)
    {
        buffer.resize((size_t)size);
        file.read((char*)(&buffer[0]), size);
    }
    else
        buffer.clear();

    std::vector<unsigned char> out_image;

    if (decodePNG(out_image, width, height, buffer.empty() ? 0 : &buffer[0], (unsigned long)buffer.size(), true) != 0)
        return false;

    size_t bufferSize = out_image.size();
    unsigned int originalBytesPerPixel = (unsigned int)bufferSize / (width * height);
    
    // Force 3 channels
    bytes_per_pixel = 3;

    if (originalBytesPerPixel == 3) {
        pixels = new Color[bufferSize];
        memcpy(pixels, &out_image[0], bufferSize);
    }
    else if (originalBytesPerPixel == 4) {

        unsigned int newBufferSize = width * height * bytes_per_pixel;
        pixels = new Color[newBufferSize];

        unsigned int k = 0;
        for (unsigned int i = 0; i < bufferSize; i += originalBytesPerPixel) {
            pixels[k] = Color(out_image[i], out_image[i + 1], out_image[i + 2]);
            k++;
        }
    }

    // Flip pixels in Y
    if (flip_y)
        FlipY();

    return true;
}

// Loads an image from a TGA file
bool Image::LoadTGA(const char* filename, bool flip_y)
{
    unsigned char TGAheader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    unsigned char TGAcompare[12];
    unsigned char header[6];
    unsigned int imageSize;
    unsigned int bytesPerPixel;

    std::string sfullPath = absResPath( filename );

    FILE * file = fopen( sfullPath.c_str(), "rb");
       if ( file == NULL || fread(TGAcompare, 1, sizeof(TGAcompare), file) != sizeof(TGAcompare) ||
        memcmp(TGAheader, TGAcompare, sizeof(TGAheader)) != 0 ||
        fread(header, 1, sizeof(header), file) != sizeof(header))
    {
        std::cerr << "File not found: " << sfullPath.c_str() << std::endl;
        if (file == NULL)
            return NULL;
        else
        {
            fclose(file);
            return NULL;
        }
    }

    TGAInfo* tgainfo = new TGAInfo;
    
    tgainfo->width = header[1] * 256 + header[0];
    tgainfo->height = header[3] * 256 + header[2];
    
    if (tgainfo->width <= 0 || tgainfo->height <= 0 || (header[4] != 24 && header[4] != 32))
    {
        fclose(file);
        delete tgainfo;
        return NULL;
    }
    
    tgainfo->bpp = header[4];
    bytesPerPixel = tgainfo->bpp / 8;
    imageSize = tgainfo->width * tgainfo->height * bytesPerPixel;
    
    tgainfo->data = new unsigned char[imageSize];
    
    if (tgainfo->data == NULL || fread(tgainfo->data, 1, imageSize, file) != imageSize)
    {
        if (tgainfo->data != NULL)
            delete tgainfo->data;
            
        fclose(file);
        delete tgainfo;
        return false;
    }

    fclose(file);

    // Save info in image
    if(pixels)
        delete pixels;

    width = tgainfo->width;
    height = tgainfo->height;
    pixels = new Color[width*height];

    // Convert to float all pixels
    for (unsigned int y = 0; y < height; ++y) {
        for (unsigned int x = 0; x < width; ++x) {
            unsigned int pos = y * width * bytesPerPixel + x * bytesPerPixel;
            // Make sure we don't access out of memory
            if( (pos < imageSize) && (pos + 1 < imageSize) && (pos + 2 < imageSize))
                SetPixel(x, height - y - 1, Color(tgainfo->data[pos + 2], tgainfo->data[pos + 1], tgainfo->data[pos]));
        }
    }

    // Flip pixels in Y
    if (flip_y)
        FlipY();

    delete tgainfo->data;
    delete tgainfo;

    return true;
}

// Saves the image to a TGA file
bool Image::SaveTGA(const char* filename)
{
    unsigned char TGAheader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    std::string fullPath = absResPath(filename);
    FILE *file = fopen(fullPath.c_str(), "wb");
    if ( file == NULL )
    {
        perror("Failed to open file: ");
        return false;
    }

    unsigned short header_short[3];
    header_short[0] = width;
    header_short[1] = height;
    unsigned char* header = (unsigned char*)header_short;
    header[4] = 24;
    header[5] = 0;

    fwrite(TGAheader, 1, sizeof(TGAheader), file);
    fwrite(header, 1, 6, file);

    // Convert pixels to unsigned char
    unsigned char* bytes = new unsigned char[width*height*3];
    for(unsigned int y = 0; y < height; ++y)
        for(unsigned int x = 0; x < width; ++x)
        {
            Color c = pixels[y*width+x];
            unsigned int pos = (y*width+x)*3;
            bytes[pos+2] = c.r;
            bytes[pos+1] = c.g;
            bytes[pos] = c.b;
        }

    fwrite(bytes, 1, width*height*3, file);
    fclose(file);

    return true;
}

void Image::DrawRect(int x, int y, int w, int h, const Color& c)
{
    for (int i = 0; i < w; ++i) {
        SetPixel(x + i, y, c);
        SetPixel(x + i, y + h, c);
    }

    for (int j = 0; j < h; ++j) {
        SetPixel(x, y + j, c);
        SetPixel(x + w, y + j, c);
    }
}

#ifndef IGNORE_LAMBDAS

// You can apply and algorithm for two images and store the result in the first one
// ForEachPixel( img, img2, [](Color a, Color b) { return a + b; } );
template <typename F>
void ForEachPixel(Image& img, const Image& img2, F f) {
    for(unsigned int pos = 0; pos < img.width * img.height; ++pos)
        img.pixels[pos] = f( img.pixels[pos], img2.pixels[pos] );
}

#endif

FloatImage::FloatImage(unsigned int width, unsigned int height)
{
    this->width = width;
    this->height = height;
    pixels = new float[width * height];
    memset(pixels, 0, width * height * sizeof(float));
}

// Copy constructor
FloatImage::FloatImage(const FloatImage& c) {
    pixels = NULL;

    width = c.width;
    height = c.height;
    if (c.pixels)
    {
        pixels = new float[width * height];
        memcpy(pixels, c.pixels, width * height * sizeof(float));
    }
}

// Assign operator
FloatImage& FloatImage::operator = (const FloatImage& c)
{
    if (pixels) delete pixels;
    pixels = NULL;

    width = c.width;
    height = c.height;
    if (c.pixels)
    {
        pixels = new float[width * height * sizeof(float)];
        memcpy(pixels, c.pixels, width * height * sizeof(float));
    }
    return *this;
}

FloatImage::~FloatImage()
{
    if (pixels)
        delete pixels;
}

// Change image size (the old one will remain in the top-left corner)
void FloatImage::Resize(unsigned int width, unsigned int height)
{
    float* new_pixels = new float[width * height];
    unsigned int min_width = this->width > width ? width : this->width;
    unsigned int min_height = this->height > height ? height : this->height;

    for (unsigned int x = 0; x < min_width; ++x)
        for (unsigned int y = 0; y < min_height; ++y)
            new_pixels[y * width + x] = GetPixel(x, y);

    delete pixels;
    this->width = width;
    this->height = height;
    pixels = new_pixels;
}


void Image::DrawLineDDA(int x0, int y0, int x1, int y1, const Color &c){
    
    int steps;
    // calculate the delta x and delta y
    int dx = x1 - x0;
    int dy = y1 - y0;

    // calculate the number of steps
    if(abs(dx) > abs(dy)){
        steps = abs(dx);
    }else{
        steps = abs(dy);
    }
    // calculate the x and y increment
    float xIncrement = dx / (float) steps;
    float yIncrement = dy / (float) steps;

    // set the initial x and y values
    float x = x0;
    float y = y0;
    
    // draw the line
    for (int i = 0; i <= steps; i++) {
        SetPixelSafe(round(x), round(y), c);
        x += xIncrement;
        y += yIncrement;
    }
}

//draw many lines each time with a different starting and ending points x and y.
void Image::DrawThickLineDDA(int x0, int y0, int x1, int y1, int thickness, const Color &c){
    for (int i = -thickness/2; i <= thickness/2; i++) {
        for (int j = -thickness/2; j <= thickness/2; j++) {
            DrawLineDDA(x0+i, y0+j, x1+i, y1+j, c);
        }
    }
}


void Image::DrawLineBresenham(int x0, int y0, int x1, int y1, const Color &c) {
    //calculate the absolute difference between the x and y coordinates
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);

    int incrementx;
    int incrementy;

    // direction of movement in x
    if (x0 < x1) {
        incrementx = 1;
    } else {
        incrementx = -1;
    }
    
    // direction of movement in y
    if(y0 < y1){
        incrementy = 1;
    }else{
        incrementy =-1;
    }
    
    if (dy < dx) {
        int d = 2 * dy - dx;
        int inc_E = 2 * dy;
        int inc_NE = 2 * (dy - dx);
        
        int x = x0;
        int y = y0;
        while (x != x1) {
            SetPixelSafe(x, y, c);
            if (d > 0) {
                y += incrementy;
                d += inc_NE;
            }
            else {
                d += inc_E;
            }
            x += incrementx;
        }
    }
    else {
        int d = 2 * dx - dy;
        int inc_E = 2 * dx;
        int inc_NE = 2 * (dx - dy);
        
        int x = x0;
        int y = y0;
        while (y != y1) {
            SetPixelSafe(x, y, c);
            if (d > 0) {
                x += incrementx;
                d += inc_NE;
            }
            else {
                d += inc_E;
            }
            y += incrementy;
        }
    }
}

void Image::DrawThickLineBresenham(int x0, int y0, int x1, int y1, int thickness, const Color &c){
    for (int i = -thickness/2; i <= thickness/2; i++) {
        for (int j = -thickness/2; j <= thickness/2; j++) {
            DrawLineBresenham(x0+i, y0+j, x1+i, y1+j, c);
        }
    }
}

void Image::DrawCircle(int x, int y, int r, const Color &c, bool fill){
    
    int x_center=x;
    int y_center=y;
    
    int v;
    x=0;
    y=r;
    v=1-r;
    
    while(y>=x){
        if(fill){
            for(int i=-x+x_center; i<=x+x_center;i++){
                SetPixelSafe(i,-y + y_center, c);
                SetPixelSafe(i,+y + y_center, c);
            }
            
            for(int i=-y+x_center; i<=y+x_center;i++){
                SetPixelSafe(i, x+y_center,c);
                SetPixelSafe(i, -x+y_center, c);
            }
        }
        else{
            SetPixelSafe(x + x_center, y + y_center, c);
            SetPixelSafe(x + x_center, -y + y_center, c);
            SetPixelSafe(-x + x_center, -y + y_center, c);
            SetPixelSafe(-x + x_center, y + y_center, c);
            SetPixelSafe(y + x_center, x + y_center, c);
            SetPixelSafe(y + x_center, -x + y_center, c);
            SetPixelSafe(-y + x_center, -x + y_center, c);
            SetPixelSafe(-y + x_center, x + y_center, c);
        }
        
        if(v<0){
            v = v+2*x + 3;
            x++;
        }
        else{
            v = v+2*(x-y) + 5;
            y--;
            x++;
        }
    }
}

void Image::DrawImagePixels(const Image& image, int x, int y, bool top){
    if(top){
        y = height-y-image.height;
    }
    for(int i=0; i<image.width;i++){
        for(int j=0; j<image.height;j++){
            SetPixelSafe(i+x, j+y, image.GetPixelSafe(i, j));
        }
    }
}


void Image::ScanLineBresenham(int x0, int y0, int x1, int y1, std::vector<myCell> &table) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);

    int incrementx;
    int incrementy;

    // direction of movement in x
    if (x0 < x1) {
        incrementx = 1;
    } else {
        incrementx = -1;
    }
    
    // direction of movement in y
    if(y0 < y1){
        incrementy = 1;
    }else{
        incrementy =-1;
    }
        
    if (dy < dx) {
        int d = 2 * dy - dx;
        int inc_E = 2 * dy;
        int inc_NE = 2 * (dy - dx);
        
       
        while (x0 != x1) {
            if (y0 >= 0 && y0 < height) {
                myCell& cell = table[y0];
                if (x0 < cell.minx) {
                    cell.minx = x0;
                }
                if (x0 > cell.maxx) {
                    cell.maxx = x0;
                }
            }
            if (d > 0) {
                y0 += incrementy;
                d += inc_NE;
            }
            else {
                d += inc_E;
            }
            x0 += incrementx;
        }
    }
    else {
        int d = 2 * dx - dy;
        int inc_E = 2 * dx;
        int inc_NE = 2 * (dx - dy);
        
       
        while (y0 != y1) {
            if (y0 >= 0 && y0 < height) {
                myCell& cell = table[y0];
                if (x0 < cell.minx) {
                    cell.minx = x0;
                }
                if (x0 > cell.maxx) {
                    cell.maxx = x0;
                }
            }
            if (d > 0) {
                x0 += incrementx;
                d += inc_NE;
            }
            else {
                d += inc_E;
            }
            y0 += incrementy;
        }
    }
}


void Image::DrawTriangle(const Vector2 &p0, const Vector2 &p1, const Vector2 &p2, const Color& color)
{
    // Create local AET variable
    std::vector<myCell> AET(height);
    // Compute and store the edges of the triangle
    std::vector<std::pair<Vector2, Vector2>> edges = {
        {p0, p1},
        {p1, p2},
        {p2, p0}
    };
    
    //iterating through the edges of the triangle,
    for (auto edge : edges) {
        int x0 = (int)edge.first.x;
        int y0 = (int)edge.first.y;
        int x1 = (int)edge.second.x;
        int y1 = (int)edge.second.y;
        ScanLineBresenham(x0, y0, x1, y1, AET);
    }
    // Iterate through the table to fill the triangle
    for (int y = 0; y < height; y++) {
        int minx = AET[y].minx;
        int maxx = AET[y].maxx;
        if (maxx >= minx) {
            for (int x = minx; x <= maxx; x++) {
                SetPixelSafe(x, y, color);
            }
        }
    }
}


void Image::DrawTriangleInterpolated(const sTriangleInfo &triangle, FloatImage* zbuffer){
    Vector2 P0(triangle.p0.x, triangle.p0.y);
    Vector2 P1(triangle.p1.x, triangle.p1.y);
    Vector2 P2(triangle.p2.x, triangle.p2.y);
  
    // Create local AET variable
    std::vector<myCell> AET(height);
    // Compute and store the edges of the triangle
    std::vector<std::pair<Vector2, Vector2>> edges = {
        {P0, P1},
        {P1, P2},
        {P2, P0}
    };
    
    //iterating through the edges of the triangle,
    for (auto edge : edges) {
        int x0 = (int)edge.first.x;
        int y0 = (int)edge.first.y;
        int x1 = (int)edge.second.x;
        int y1 = (int)edge.second.y;
        ScanLineBresenham(x0, y0, x1, y1, AET);
    }
    // Iterate through the table to fill the triangle
    for (int y = 0; y < height; y++) {
        int minx = AET[y].minx;
        int maxx = AET[y].maxx;
        if (maxx >= minx) {
            for (int x = minx; x <= maxx; x++) {
                Vector2 p(x,y);
                Vector2 v0 = P1-P0;
                Vector2 v1 = P2-P0;
                Vector2 v2 = p-P0;
                
                float d00 = v0.Dot(v0);
                float d01 = v0.Dot(v1);
                float d11 = v1.Dot(v1);
                float d20 = v2.Dot(v0);
                float d21 = v2.Dot(v1);
                
                float denom = d00*d11 -d01*d01;
                
                float v = (d11*d20-d01*d21)/denom;
                float w = (d00*d21-d01*d20)/denom;
                float u = 1.0 -v -w;
                v = clamp(v, 0,1);
                w = clamp(w, 0, 1);
                u = clamp(u, 0, 1);
                
                float s = u+v+w;
                u /=s;
                w /=s;
                v /=s;
                
                int i_ = clamp(x,0, width-1);
                //int j_ = clamp(y, 0, height);//quitar
                
                if (zbuffer == nullptr) {
                    Color c;
                    if (triangle.texture == nullptr) {
                        c = triangle.c0 * u + triangle.c1 * v + triangle.c2 * w;
                    } else {
                        Vector2 uv = triangle.uv0 * u + triangle.uv1 * v + triangle.uv2 * w;
                        c = triangle.texture->GetPixel(uv.x * (triangle.texture->width-1), uv.y * (triangle.texture->height-1));
                    }
                    SetPixelSafe(i_, y, c);
                } else {
                    float zpixel = triangle.p0.z * u + triangle.p1.z * v + triangle.p2.z * w;
                    if (zbuffer->GetPixel(i_, y) > zpixel) {
                        zbuffer->SetPixel(i_, y, zpixel);
                        Color c;
                        if (triangle.texture == nullptr) {
                            c = triangle.c0 * u + triangle.c1 * v + triangle.c2 * w;
                        } else {
                            Vector2 uv = triangle.uv0 * u + triangle.uv1 * v + triangle.uv2 * w;
                            c = triangle.texture->GetPixel(uv.x * (triangle.texture->width-1), uv.y * (triangle.texture->height-1));
                        }
                        SetPixelSafe(i_, y, c);
                    }
                }
            }
        }
    }
}


