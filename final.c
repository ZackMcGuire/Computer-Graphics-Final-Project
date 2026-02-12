#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

#ifdef USEGLEW
#include <GL/glew.h>
#else
    #include <GL/gl.h>
    #include <GL/glext.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#ifndef RES
#define RES 1
#endif

//global constants
const int unique_objects = 7;
const float habitat_radius = 0.5;
const float habitat_height = 0.5;
const float player_speed = 0.007; //should be around 0.002 for normal gameplay
const float player_head_height = 0.02;
const float player_head_width  = 0.01;
const float mouse_sensitivity_x = 0.3;
const float mouse_sensitivity_y = 0.2;
const float white_vec[] = {1.0,1.0,1.0,1.0};
const float green_vec[] = {0.2,0.6,0.2,1.0};
const float gray_vec[] = {0.2,0.2,0.2,1.0};
const float max_angle_up_down = 85;
const float fov_first_person = 40;
const float fullscreen_immunity = 0.3;
const float reach = 0.6;
const float house_radius = 0.025;
const float tree_radius = 0.003;
const int texture_portions = 8;
const float game_speed = 4; //should be around 2-5 for normal gameplay
const float sun_distance = 20;
#define houses_size    256
#define trees_size     256
#define keys_size      256
#define particles_size 128
#define textures_size  16
#define habitat_sides  60
#define particles_attributes 5

//global variables
float angle_left_right = 0;
float angle_up_down    = 0;
int camera = 1;
int debug_mode = 0;
int axes_on = 0;
int hud_text_on = 1;
int debug_sides = 5;
float aspect_ratio = 1;
float world_size = 1.0;
float fov = 60;
float orbit_habitat = 0;
float orbit_earth = 0;
float yaw   = 0;
float roll  = 0;
float player_r  = habitat_radius - player_head_height;
float player_th = 0.0;
float player_h  = 0.2;
float player_x  = 0.0;
float player_y  = 0.0;
float player_z  = 0.0;
int window_height = 600;
int window_width  = 700;
int window_height_non_fullscreen = 600;
int window_width_non_fullscreen  = 700;
int texturing = 1;
int lighting = 1;
int light_movement = 1;
float light_y = 0.5;
float light_distance = 0.7;
float light_rotation = 0.0;
float ambient  = 0.1;
float diffuse  = 0.7;
float specular = 0.1;
float shiny = 4.0;
int ignore_next_look = 0;
int switch_1 = 0;
float look_x = 0;
float look_y = 0;
float look_z = 0;
float up_x = 0;
float up_y = 0;
float up_z = 0;
int fullscreen = 0;
int keys[keys_size] = {0};
int special_keys[keys_size] = {0};
float frame_timer = 0;
float fullscreen_press_timestamp = 0;
float line_clipping_x = 0;
float line_clipping_y = 0;
float line_clipping_z = 0;
int esc_menu = 0;
int house_count = 4;
double time = 0;
double last_keyboard_functionality = 0;
GLint useTexture = 0;
int create_particle = 0;
float houses[houses_size][8] = {
    {0.5, 0.4, 0.5, house_radius, habitat_radius, 45, 0.16, 45},
    {0.8, 0.8, 0.4, house_radius, habitat_radius, 80, 0.32, 60},
    {0.2, 0.2, 0.9, house_radius, habitat_radius, 65, 0.18, 30},
    {0.4, 0.3, 1.0, house_radius, habitat_radius,-10, 0.34, 15},
    {0.7, 0.4, 0.6, house_radius, habitat_radius,-30, 0.18, 20},
    {0.2, 0.5, 0.9, house_radius, habitat_radius,-45, 0.35, 35},
    {0.3, 0.3, 0.7, house_radius, habitat_radius, 20, 0.32, 70}
};

float trees[trees_size][9] = {
    {0.6, 0.4, 0.5, tree_radius*0.9, habitat_radius, 36, 0.34, 55, 68765},
    {0.8, 0.7, 0.4, tree_radius*1.5, habitat_radius, 25, 0.30, 40, 15762},
    {0.2, 0.5, 0.9, tree_radius*1.1, habitat_radius, 10, 0.37, 35, 54676},
    {0.4, 0.3, 0.8, tree_radius*0.9, habitat_radius, 89, 0.19, 20, 97531},
    {0.6, 0.4, 0.7, tree_radius*0.9, habitat_radius, 76, 0.17, 65, 78755},
    {0.8, 0.2, 0.4, tree_radius*1.5, habitat_radius, 26, 0.20, 45, 15462},
    {0.5, 0.5, 0.9, tree_radius*1.1, habitat_radius, 55, 0.16, 15, 24636},
    {0.4, 0.1, 0.7, tree_radius*1.2, habitat_radius, 70, 0.22, 22, 51531},
    {0.2, 0.5, 0.9, tree_radius*1.4, habitat_radius, 10, 0.37, 35, 76776},
    {0.4, 0.3, 0.8, tree_radius*0.9, habitat_radius, 89, 0.19, 20, 75331},
    {0.5, 0.4, 0.5, tree_radius*0.8, habitat_radius, 05, 0.14, 55, 71755},
    {0.8, 0.1, 0.4, tree_radius*1.0, habitat_radius, 17, 0.16, 25, 15742},
    {0.2, 0.6, 0.7, tree_radius*1.3, habitat_radius, 26, 0.34, 75, 34676},
    {0.1, 0.5, 0.7, tree_radius*0.9, habitat_radius, -8, 0.17, 25, 71351}
};

unsigned int textures[textures_size];
int grass_random_x[habitat_sides];
int grass_random_y[habitat_sides];
int shader[] = {0, 0, 0};
float particles[particles_size*particles_attributes] = {0};

#define Cos(th) cos(3.14159265/180*th)
#define Sin(th) sin(3.14159265/180*th)
#define Cosf(th) cosf(3.14159265/180*th)
#define Sinf(th) sinf(3.14159265/180*th)
#define aTan(x) 180/3.14159265*atan(x)
typedef struct {float x, y, z;} vec3;

void SetColor(float R, float G, float B)
{
   float color[] = {R, G, B, 1.0};
   glColor3f(R, G, B);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
}

vec3 Normalize(vec3 vector)
{
    float len = sqrt(vector.x*vector.x + vector.y*vector.y + vector.z*vector.z);
    if (len > 0)
    {
        vector.x /= len;
        vector.y /= len;
        vector.z /= len;
    }
    return (vec3){vector.x, vector.y, vector.z};
}

vec3 Cross(vec3 a, vec3 b) 
{
    vec3 result;
    a = Normalize(a);
    b = Normalize(b);
    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z;
    result.z = a.x * b.y - a.y * b.x;
    return result;
}

void Print(const char* format , ...)
{
    char    buf[8192]; //text storage
    char*   ch = buf;  //text pointer
    //create text to be display
    va_list args;
    va_start(args, format);
    vsnprintf(buf, 8192, format, args);
    va_end(args);
    //display text string
    while (*ch)
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *ch++);
}

void ErrCheck(char* where)
{
    int err = glGetError();
    if (err) fprintf(stderr,"ERROR: %s [%s]\n",gluErrorString(err),where);
}

void Fatal(const char* format , ...)
{
   va_list args;
   va_start(args,format);
   vfprintf(stderr,format,args);
   va_end(args);
   exit(1);
}

static void Reverse(void* x,const int n)
{
    char* ch = (char*)x;
    for (int k=0;k<n/2;k++)
    {
        char tmp = ch[k];
        ch[k] = ch[n-1-k];
        ch[n-1-k] = tmp;
    }
}

unsigned int LoadTexBMP(const char* file)
{
    //  Open file
    FILE* f = fopen(file,"rb");
    if (!f) Fatal("Cannot open file %s\n",file);
    //  Check image magic
    unsigned short magic;
    if (fread(&magic,2,1,f)!=1) Fatal("Cannot read magic from %s\n",file);
    if (magic!=0x4D42 && magic!=0x424D) Fatal("Image magic not BMP in %s\n",file);
    //  Read header
    unsigned int dx,dy,off,k; // Image dimensions, offset and compression
    unsigned short nbp,bpp;   // Planes and bits per pixel
    if (fseek(f,8,SEEK_CUR) || fread(&off,4,1,f)!=1 ||
        fseek(f,4,SEEK_CUR) || fread(&dx,4,1,f)!=1 || fread(&dy,4,1,f)!=1 ||
        fread(&nbp,2,1,f)!=1 || fread(&bpp,2,1,f)!=1 || fread(&k,4,1,f)!=1)
        Fatal("Cannot read header from %s\n",file);
    //  Reverse bytes on big endian hardware (detected by backwards magic)
    if (magic==0x424D)
    {
        Reverse(&off,4);
        Reverse(&dx,4);
        Reverse(&dy,4);
        Reverse(&nbp,2);
        Reverse(&bpp,2);
        Reverse(&k,4);
    }
    //  Check image parameters
    unsigned int max;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE,(int*)&max);
    if (dx<1 || dx>max) Fatal("%s image width %d out of range 1-%d\n",file,dx,max);
    if (dy<1 || dy>max) Fatal("%s image height %d out of range 1-%d\n",file,dy,max);
    if (nbp!=1)  Fatal("%s bit planes is not 1: %d\n",file,nbp);
    if (bpp!=24) Fatal("%s bits per pixel is not 24: %d\n",file,bpp);
    if (k!=0)    Fatal("%s compressed files not supported\n",file);
    #ifndef GL_VERSION_2_0
    //  OpenGL 2.0 lifts the restriction that texture size must be a power of two
    for (k=1;k<dx;k*=2);
    if (k!=dx) Fatal("%s image width not a power of two: %d\n",file,dx);
    for (k=1;k<dy;k*=2);
    if (k!=dy) Fatal("%s image height not a power of two: %d\n",file,dy);
    #endif

    //  Allocate image memory
    unsigned int size = 3*dx*dy;
    unsigned char* image = (unsigned char*) malloc(size);
    if (!image) Fatal("Cannot allocate %d bytes of memory for image %s\n",size,file);
    //  Seek to and read image
    if (fseek(f,off,SEEK_SET) || fread(image,size,1,f)!=1) Fatal("Error reading data from image %s\n",file);
    fclose(f);
    //  Reverse colors (BGR -> RGB)
    for (k=0;k<size;k+=3)
    {
        unsigned char temp = image[k];
        image[k]   = image[k+2];
        image[k+2] = temp;
    }

    //  Sanity check
    ErrCheck("LoadTexBMP");
    //  Generate 2D texture
    unsigned int texture;
    glGenTextures(1,&texture);
    glBindTexture(GL_TEXTURE_2D,texture);
    //  Copy image
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,dx,dy,0,GL_RGB,GL_UNSIGNED_BYTE,image);
    if (glGetError()) Fatal("Error in glTexImage2D %s %dx%d\n",file,dx,dy);
    //  Scale linearly when image size doesn't match
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

    //  Free image memory
    free(image);
    //  Return texture name
    return texture;
}

static void Projection()
{
    //select projection matrix
    glMatrixMode(GL_PROJECTION);
    //set projection to identity
    glLoadIdentity();
    //select projection matrix
    switch (camera)
    {
        case 0:
            gluPerspective(fov, aspect_ratio, 1.0/50, 150.0);
            break;
        case 1:
            {
                float actual_fov = fov+fov_first_person;
                if (actual_fov > 179) {actual_fov = 179;}
                gluPerspective(actual_fov, aspect_ratio, 1.0/300, 50.0);
                break;
            }
    }
    //select model view matrix
    glMatrixMode(GL_MODELVIEW);
    //set model view to identity
    glLoadIdentity();
}

void hudText()
{
    SetColor(1, 1, 1);

    char* camera_text = "Error";
    if (camera == 0) camera_text = "Perspective";
    if (camera == 1) camera_text = "First Person";

    char* texturing_text = "Off";
    if (texturing) texturing_text = "On";

    char* lighting_text = "Off";
    if (lighting) lighting_text = "On";

    char* light_movement_text = "Off";
    if (light_movement) light_movement_text = "On";

    char* fullscreen_text = "No";
    if (fullscreen) fullscreen_text = "Yes";

    char* axes_text = "Off";
    if (axes_on) axes_text = "On";

    glWindowPos2i(5, 5);
    Print("Camera (m): %s, FOV (-/+): %.0f, World size (PgUp/PgDn): %.1f", camera_text, camera ? fov + fov_first_person : fov, world_size);
    glWindowPos2i(5, 30);
    Print("Texturing (t): %s, Lighting (l): %s, Specular (z): %.2f, Shiny (x): %.1f", texturing_text, lighting_text, specular, shiny);
    glWindowPos2i(5, 55);
    Print("Fullscreen (f11): %s, Debug mode (r): %d, Switch (1): %d", fullscreen_text, debug_mode, switch_1);
    glWindowPos2i(5, 80);
    Print("Axes (q): %s, HUD (e): %s", axes_text, "On");
    
    if (debug_mode)
    {
        glWindowPos2i(5, window_height-20);
        Print("Light movement (k): %s, Distance (j): %.1f, Height (h): %.1f", light_movement_text, light_distance, light_y);
    }
    // glWindowPos2i(5, window_height-20);
    // Print("XYZ: (%.2f, %.2f, %.2f)", player_x, player_y, player_z);
    // glWindowPos2i(5, window_height-45);
    // Print("RThH: (%.2f, %.2f, %.2f)", player_r, player_th, player_h);
    // glWindowPos2i(5, window_height-70);
    // Print("View angle: (%.1f, %.1f)", angle_left_right, angle_up_down); 
    // glWindowPos2i(5, window_height-95);
    // Print("Line clipping: (%.2f, %.2f, %.2f)", line_clipping_x, line_clipping_y, line_clipping_z); 
}

void hud()
{
    SetColor(1, 1, 1);
    
    //draw crosshair if esc menu isn't open
    if (!esc_menu)
    {
        glWindowPos2i(window_width/2 - 5, window_height/2 - 5);
        (line_clipping_x == -5) ? Print("o") : Print("x");
    }
    else
    {
        glWindowPos2i(window_width/2 - 85, window_height/2 - 5);
        Print("Press ESC to resume");
    }
}

float* toCartesian(float r, float th, float h)
{
    float* position = (float*) malloc(3 * sizeof(float));
    /*x*/ position[0] = r * Cos(th);
    /*y*/ position[1] = h;
    /*z*/ position[2] = r * Sin(th);
    return position;
}

float* toCylindrical(float x, float y, float z)
{
    float* position = (float*) malloc(3 * sizeof(float));
    /*r*/ position[0] = sqrt(pow(x, 2) + pow(z, 2));
    /*th*/position[1] = aTan(z/x);
    /*h*/ position[2] = y;
    if (x < 0) {position[1] += 180;}
    if (x >= 0 && z < 0) {position[1] += 360;}
    return position;
}

void axes(float look_x, float look_y, float look_z, float up_x, float up_y, float up_z)
{
    float len = 0.5;
    glBegin(GL_LINES);
    SetColor(1, 0, 0);
    glVertex3f(0.0, 0.0, 0.0); 
    glVertex3f(len, 0.0, 0.0);
    glEnd();
    glRasterPos3d(len, 0.0, 0.0);
    Print("X");

    glBegin(GL_LINES);
    SetColor(0, 1, 0);
    glVertex3f(0.0, 0.0, 0.0); 
    glVertex3f(0.0, len, 0.0);
    glEnd();
    glRasterPos3d(0.0, len, 0.0);
    Print("Y");

    glBegin(GL_LINES);
    SetColor(0, 0, 1);
    glVertex3f(0.0, 0.0, 0.0); 
    glVertex3f(0.0, 0.0, len); 
    glEnd();
    glRasterPos3d(0.0, 0.0, len);
    Print("Z");

    glBegin(GL_LINES);
    SetColor(1, 1, 0);
    glVertex3f(0, 0, 0); 
    glVertex3f(up_x, up_y, up_z);
    glEnd();
    glRasterPos3d(up_x, up_y, up_z);
    Print("Up");

    glBegin(GL_LINES);
    SetColor(1.0, 0.4, 0.6);
    glVertex3f(player_x, player_y, player_z); 
    glVertex3f(look_x, look_y, look_z);
    glEnd();
    glRasterPos3d(look_x, look_y, look_z);
    Print("Look");
}

void square(float red, float green, float blue, float diameter, int texture)
{   
    glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, shiny);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  white_vec);

    if (texturing)
    {
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    }
    
    glBindTexture(GL_TEXTURE_2D, texture);
    glNormal3f(0, 0, 1);
    SetColor(red, green, blue);
    glBegin(GL_QUADS);
    glTexCoord2f(1.0, 1.0); glVertex3f( diameter/2, diameter/2, 0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-diameter/2, diameter/2, 0);
    glTexCoord2f(0.0, 0.0); glVertex3f(-diameter/2,-diameter/2, 0);
    glTexCoord2f(1.0, 0.0); glVertex3f( diameter/2,-diameter/2, 0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void cube(float red, float green, float blue, float radius, int door_texture, int wall_texture)
{
    glDisable(GL_BLEND);
    glDepthMask(1);

    glPushMatrix();
        glTranslatef(0, 0, radius);
        square(red, green, blue, radius*2, wall_texture);
    glPopMatrix();

    glPushMatrix();
        glRotatef(90, 1, 0, 0);
        glTranslatef(0, 0, radius);
        square(red, green, blue, radius*2, wall_texture);
    glPopMatrix();

    glPushMatrix();
        glRotatef(90, 0, 1, 0);
        glTranslatef(0, 0, radius);
        square(red, green, blue, radius*2, wall_texture);
    glPopMatrix();

    glPushMatrix();
        glRotatef(180, 0, 1, 0);
        glTranslatef(0, 0, radius);
        square(red, green, blue, radius*2, door_texture);
    glPopMatrix();

    glPushMatrix();
        glRotatef(90, 1, 0, 0);
        glRotatef(180, 0, 1, 0);
        glTranslatef(0, 0, radius);
        square(red, green, blue, radius*2, wall_texture);
    glPopMatrix();

    glPushMatrix();
        glRotatef(90, 0, 1, 0);
        glRotatef(180, 0, 1, 0);
        glTranslatef(0, 0, radius);
        square(red, green, blue, radius*2, wall_texture);
    glPopMatrix();
}

void sphere(float red, float green, float blue, float radius, int sides, int texture)
{
    if (texturing)
    {
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    }

    float inc = 360.0/sides;

    SetColor(red, green, blue);
    glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, shiny);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  white_vec);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDisable(GL_BLEND);
    glDepthMask(1);

    glPushMatrix();
    glScaled(radius, radius, radius);
    for(float ph = -90; ph <= 90; ph += inc)
    {
        for (float th = 0; th <= 360; th += inc)
        {
            glBegin(GL_POLYGON);

            float vert_x = Sin(th)*Cos(ph);
            float vert_y = Cos(th)*Cos(ph);
            float vert_z =         Sin(ph);
            glNormal3d(vert_x, vert_y, vert_z);
            glTexCoord2f(-th/360, (ph/180)+0.5);
            glVertex3d(vert_x, vert_y, vert_z);

            vert_x = Sin((th+inc))*Cos(ph);
            vert_y = Cos((th+inc))*Cos(ph);
            vert_z =               Sin(ph);
            glNormal3d(vert_x, vert_y, vert_z);
            glTexCoord2f(-(th+inc)/360, (ph/180)+0.5);
            glVertex3d(vert_x, vert_y, vert_z);
            
            vert_x = Sin((th+inc))*Cos((ph+inc));
            vert_y = Cos((th+inc))*Cos((ph+inc));
            vert_z =               Sin((ph+inc));
            glNormal3d(vert_x, vert_y, vert_z);
            glTexCoord2f(-(th+inc)/360, ((ph+inc)/180)+0.5);
            glVertex3d(vert_x, vert_y, vert_z);
   
            vert_x = Sin(th)*Cos((ph+inc));
            vert_y = Cos(th)*Cos((ph+inc));
            vert_z =         Sin((ph+inc));
            glNormal3d(vert_x, vert_y, vert_z);
            glTexCoord2f(-th/360, ((ph+inc)/180)+0.5);
            glVertex3d(vert_x, vert_y, vert_z);

            glEnd();
        }
    }
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void circle(float red, float green, float blue, float radius, int sides, int texture)
{
    if (texturing)
    {
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    }
    glDisable(GL_BLEND);
    glDepthMask(1);

    float inc = 360.0/sides;

    SetColor(red, green, blue);
    glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, shiny);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  white_vec);
    glBindTexture(GL_TEXTURE_2D, texture);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);

    glPushMatrix();
        glRotatef(inc/2, 0, 1, 0);
        glBegin(GL_POLYGON);
        glNormal3d(0, 1, 0);
        glTexCoord2f(0.5, 0.5);
        glVertex3d(0, 0, 0);
        for (int i = 0; i <= sides; i++)
        {
            glTexCoord2f(Sin(i*inc)/2+0.5, Cos(i*inc)/2+0.5);
            glVertex3f(Sin(i*inc)*radius, 0.0, Cos(i*inc)*radius);
        }
        glEnd();
    glPopMatrix();

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
    glDisable(GL_TEXTURE_2D);
}

void spike(float red, float green, float blue, float height, float width, int sides, int texture)
{
    glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, shiny);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  white_vec);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
    glDisable(GL_BLEND);
    glDepthMask(1);

    if (texturing)
    {
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glBindTexture(GL_TEXTURE_2D, texture);
    }
    
    float inc = 360.0/sides;
    SetColor(red, green, blue);
    for (int i = 0; i < sides; i++)
    {
        glPushMatrix();
        glRotatef(i*inc, 0, 1, 0);
        glBegin(GL_POLYGON);
        glTexCoord2f(0.5, 1.0); 
        glNormal3f( 0.0,               Cos(inc/2)*width,  height          );
        glVertex3f( 0.0,               height,            0.0             );
        glTexCoord2f(1.0, 0.0); 
        glNormal3f( Sin(inc/2)*width,  Cos(inc/2)*width,  Cos(inc/2)*width);
        glVertex3f( Sin(inc/2)*width,  0.0,               Cos(inc/2)*width);
        glTexCoord2f(0.0, 0.0); 
        glNormal3f(-Sin(inc/2)*width,  Cos(inc/2)*width,  Cos(inc/2)*width);
        glVertex3f(-Sin(inc/2)*width,  0.0,               Cos(inc/2)*width);
        glEnd();
        glPopMatrix();
    }
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
    glDisable(GL_TEXTURE_2D);
}

void prism(float red, float green, float blue, float height, float radius, float thickness, int sides, const float* color, float inner_red, float inner_green, float inner_blue, int inner_texture, int outer_texture)
{
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, color);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
    glDisable(GL_BLEND);
    glDepthMask(1);

    float inc = 360.0/sides;
    SetColor(red, green, blue);

    if (texturing)
    {
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    }

    //outer wall
    glBindTexture(GL_TEXTURE_2D, outer_texture);
    glPushMatrix();
        for (int i = 0; i < sides; i++)
        {
            glRotatef(inc, 0, 1, 0);
            glBegin(GL_QUADS);
            glNormal3f( Sin(inc/2)*radius, 0.0,    Cos(inc/2)*radius);
            glTexCoord2f(1.0, 1.0); 
            glVertex3f( Sin(inc/2)*radius, height, Cos(inc/2)*radius);
            glTexCoord2f(1.0, 0.0); 
            glVertex3f( Sin(inc/2)*radius, 0.0,    Cos(inc/2)*radius);
            glNormal3f(-Sin(inc/2)*radius, 0.0,    Cos(inc/2)*radius);
            glTexCoord2f(0.0, 0.0); 
            glVertex3f(-Sin(inc/2)*radius, 0.0,    Cos(inc/2)*radius);
            glTexCoord2f(0.0, 1.0); 
            glVertex3f(-Sin(inc/2)*radius, height, Cos(inc/2)*radius);
            glEnd();
        }
    glPopMatrix();
    if (thickness)
    {
        //edges
        glPushMatrix();
            for (int i = 0; i < sides; i++)
            {
                glRotatef(inc, 0, 1, 0);
                glNormal3f(0, -1, 0);
                glBegin(GL_QUADS);
                glVertex3f( Sin(inc/2)*(radius),           0.0,    Cos(inc/2)*(radius));
                glVertex3f(-Sin(inc/2)*(radius),           0.0,    Cos(inc/2)*(radius));
                glVertex3f(-Sin(inc/2)*(radius-thickness), 0.0,    Cos(inc/2)*(radius-thickness));
                glVertex3f( Sin(inc/2)*(radius-thickness), 0.0,    Cos(inc/2)*(radius-thickness));
                glEnd();
                glNormal3f(0, 1, 0);
                glBegin(GL_QUADS);
                glVertex3f( Sin(inc/2)*(radius),           height, Cos(inc/2)*(radius));
                glVertex3f(-Sin(inc/2)*(radius),           height, Cos(inc/2)*(radius));
                glVertex3f(-Sin(inc/2)*(radius-thickness), height, Cos(inc/2)*(radius-thickness));
                glVertex3f( Sin(inc/2)*(radius-thickness), height, Cos(inc/2)*(radius-thickness));
                glEnd();
            }
        glPopMatrix();
        //inner wall
        SetColor(inner_red, inner_green, inner_blue);
        float x = 0;
        float y = 0;
        float size = 1;
        glBindTexture(GL_TEXTURE_2D, inner_texture);
        glPushMatrix();
            for (int i = 0; i < sides; i++)
            {
                if (sides == habitat_sides)
                {
                    x = (float)grass_random_x[i]/texture_portions;
                    y = (float)grass_random_y[i]/texture_portions; 
                    size = 1.0/texture_portions;
                }
                glRotatef(inc, 0, 1, 0);
                glBegin(GL_QUADS);
                glNormal3f(-Sin(inc/2)*(radius-thickness), 0.0,   -Cos(inc/2)*(radius-thickness));
                glTexCoord2f(x+size, y+size); 
                glVertex3f( Sin(inc/2)*(radius-thickness), height, Cos(inc/2)*(radius-thickness));
                glTexCoord2f(x+size, y     ); 
                glVertex3f( Sin(inc/2)*(radius-thickness), 0.0,    Cos(inc/2)*(radius-thickness));
                glNormal3f( Sin(inc/2)*(radius-thickness), 0.0,   -Cos(inc/2)*(radius-thickness));
                glTexCoord2f(x,      y     ); 
                glVertex3f(-Sin(inc/2)*(radius-thickness), 0.0,    Cos(inc/2)*(radius-thickness));
                glTexCoord2f(x,      y+size); 
                glVertex3f(-Sin(inc/2)*(radius-thickness), height, Cos(inc/2)*(radius-thickness));
                glEnd();
            }
        glPopMatrix();
    }
    glDisable(GL_TEXTURE_2D);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
}

void house(float red, float green, float blue, float radius, float r, float th, float h, float angle)
{
    float brownish_red   = red/4   + 0.50;
    float brownish_green = green/4 + 0.05;
    float brownish_blue  = blue/4  + 0.05;

    glPushMatrix();
    glRotatef(th, 0, -1, 0);
    glTranslatef(0, h, 0);
    glRotatef(angle, 1, 0, 0);
    glTranslatef(r - radius*1.06, 0, 0);
    glRotatef(90, 0, 0, 1);
    cube(red, green, blue, radius, textures[3], textures[0]);
    glTranslatef(0.0, radius/2, 0.0);
    spike(brownish_red, brownish_green, brownish_blue, radius*2, radius*2, 4, textures[1]);
    glPopMatrix();
}

void houseWireframe(float radius, float r, float th, float h, float angle)
{
    if (esc_menu) {return;}
    if (!camera)  {return;}

    glPushMatrix();
    glRotatef(th, 0, -1, 0);
    glTranslatef(0, h, 0);
    glRotatef(angle, 1, 0, 0);
    glTranslatef(r - radius/14, 0, 0);
    glRotatef(90, 0, 0, 1);
    glLineWidth(5);
    glBegin(GL_LINE_LOOP);
    glVertex3f( radius, 0.0, radius);
    glVertex3f( radius, 0.0,-radius);
    glVertex3f(-radius, 0.0,-radius);
    glVertex3f(-radius, 0.0, radius);
    glEnd();
    glPopMatrix();
}

void fins(float red, float green, float blue, float height, float width, int sides)
{
    glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, shiny);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  white_vec);

    glNormal3f(1, 0, 0);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
    int every_nth_side = 1;
    if (sides % 3 == 0) {every_nth_side = sides/3;}
    else if (sides % 4 == 0) {every_nth_side = sides/4;}
    else if (sides % 5 == 0) {every_nth_side = sides/5;}
    else if (sides % 2 == 0) {every_nth_side = sides/2;}
    else if (sides % 7 == 0) {every_nth_side = sides/7;}

    float inc = 360.0/sides;
    SetColor(red, green, blue);
    for (int i = 0; i < sides; i++)
    {
        if (i % every_nth_side == 0)
        {
            glPushMatrix();
            glRotatef(i*inc, 0, 1, 0);
            glBegin(GL_POLYGON);
            glVertex3f(0.0,-height/6, 3*Cos(inc/2)*width);
            glVertex3f(0.0, height/10,  Cos(inc/2)*width);
            glVertex3f(0.0, height/2,   Cos(inc/2)*width);
            glEnd();
            glPopMatrix();
        }
    }
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
}

void jack(float red, float green, float blue, float height, float x, float y, float z, float orbit_habitat, float yaw, float roll)
{
    float width = height*0.2;

    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(orbit_habitat, 1, 0, 0);
    glRotatef(yaw,   0, 1, 0);
    glRotatef(roll,  0, 0, 1);
    glRotatef(180, 0, 1, 0);
    spike(red, green, blue, height, width, 3, 0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(orbit_habitat, 1, 0, 0);
    glRotatef(yaw,   0, 1, 0);
    glRotatef(roll,  0, 0, 1);
    glRotatef(109.5, 1, 0, 0);
    spike(red, green, blue, height, width, 3, 0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(orbit_habitat, 1, 0, 0);
    glRotatef(yaw,   0, 1, 0);
    glRotatef(roll,  0, 0, 1);
    glRotatef(120, 0, 1, 0);
    glRotatef(109.5, 1, 0, 0);
    glRotatef(-120, 0, 1, 0);
    spike(red, green, blue, height, width, 3, 0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(orbit_habitat, 1, 0, 0);
    glRotatef(yaw,   0, 1, 0);
    glRotatef(roll,  0, 0, 1);
    glRotatef(240, 0, 1, 0);
    glRotatef(109.5, 1, 0, 0);
    glRotatef(-240, 0, 1, 0);
    spike(red, green, blue, height, width, 3, 0);
    glPopMatrix();
}

void ship(float height, float width, int sides, float distance, float central_axis_rotation, float flight_speed, float flight_path)
{
    glUniform1i(useTexture, 0);

    glPushMatrix();
    //travel around a different axis
    glRotatef(flight_path, 1.0, 0.0, 0.0);
    //rotate around center of screen
    glRotatef(flight_speed, 0.0, 0.0, 1.0);
    //offset from center
    glTranslatef(distance, 0.0, 0.0);
    //spin around ship's central axis
    glRotatef(central_axis_rotation, 0.0, 1.0, 0.0);

    //ship body
    prism(0.9, 0.3, 0.3, height,   width, 0, sides, white_vec, 0.0, 0.0, 0.0, 0, 0);
    //ship engine
    spike(0.2, 0.2, 0.2, height/4, width, sides, 0);
    //ship fins
    fins (0.3, 0.9, 0.6, height,   width, sides);
    //ship nose cone
    glTranslatef(0.0, height, 0.0);
    spike(0.4, 0.9, 0.4, height,   width, sides, 0);
    glPopMatrix();

    glUniform1i(useTexture, 1);
}

void wheelLarge(float red, float green, float blue, float height, float radius, float thickness, int sides, int spokes, float rotation, const float* color, int texture)
{
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, color);

    float inc = 360.0/spokes;

    glPushMatrix();
        glRotatef(90, 1, 0, 0);
        glRotatef(-rotation, 0, 1, 0);
        prism(red, green, blue, height, radius,   thickness, sides, color, 0, 0, 0, 0, texture);
        prism(red, green, blue, height, radius/4, radius/4,  sides, color, 0, 0, 0, 0, texture);
        SetColor(red, green, blue);

        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
        float r = radius*0.97;
        for (int i = 0; i < spokes; i++)
        {
            glNormal3f(Sin(i*inc+90), 0, Cos(i*inc+90));
            
            glBegin(GL_QUADS);
            glVertex3f(0.0,          0.0,    0.0);
            glVertex3f(0.0,          height, 0.0);
            glVertex3f(Sin(i*inc)*r, height, Cos(i*inc)*r);
            glVertex3f(Sin(i*inc)*r, 0.0,    Cos(i*inc)*r);
            glEnd();
        }
    glPopMatrix();

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
}

void wheelSmall(float red, float green, float blue, float height, float radius, int sides, float rotation, const float* color, int texture)
{
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
    SetColor(red, green, blue);

    glPushMatrix();
        glRotatef(90, 1, 0, 0);
        glRotatef(-rotation, 0, 1, 0);
        prism(red, green, blue, height,     radius,   radius,    sides, color, 0, 0, 0, 0, texture);
        prism(red, green, blue, height*1.1, radius/3, radius/3,  sides, color, 0, 0, 0, 0, texture);
    glPopMatrix();

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
}

void cabin(float red, float green, float blue, float size, const float* color, int texture, int opaque)
{
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, color);

    glPushMatrix();
        glTranslatef(0.0, 0.0, -0.1);

        if (texturing)
        {
            glEnable(GL_TEXTURE_2D);
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        }

        float side_buffer   = 0.03;
        float top_buffer    = 0.03;
        float bottom_buffer = 0.08;
        float beam_radius   = 0.005;
        float horizontal_beam = size-(side_buffer+side_buffer);
        float vertical_beam   = size-(top_buffer+bottom_buffer);

        if (!opaque)
        {
            glEnable(GL_BLEND);
            SetColor(0.5, 0.5, 0.5);
            glColor4f(0.5, 0.5, 0.5, 0.05);
            //glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDepthMask(0);

            for (float z = 0; z <= size*1.5; z += size)
            {
                glNormal3f(0, 0, 1);
                if (!z)
                    glNormal3f(0, 0, -1);
                glBegin(GL_QUADS);
                glVertex3f(size-bottom_buffer, size-side_buffer, z);
                glVertex3f(top_buffer,         size-side_buffer, z);
                glVertex3f(top_buffer,         side_buffer,      z);
                glVertex3f(size-bottom_buffer, side_buffer,      z);
                glEnd();
            }
        }

        if (opaque)
        {
            glDisable(GL_BLEND);
            glDepthMask(1);

            glPushMatrix();
                SetColor(red, green, blue);
                glBindTexture(GL_TEXTURE_2D, texture);

                //sides
                for (float z = 0; z <= size*1.5; z += size)
                {
                    //walls
                    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
                    glNormal3f(0, 0, z ? 1 : -1);

                    glBegin(GL_POLYGON);
                    glVertex3f(size, side_buffer, z);
                    glVertex3f(0.0,  side_buffer, z);
                    glVertex3f(0.0,  0.0,         z);
                    glVertex3f(size, 0.0,         z);
                    glEnd();
                    glBegin(GL_POLYGON);
                    glVertex3f(size,               size-side_buffer, z);
                    glVertex3f(size-bottom_buffer, size-side_buffer, z);
                    glVertex3f(size-bottom_buffer, side_buffer,      z);
                    glVertex3f(size,               side_buffer,      z);
                    glEnd();
                    glBegin(GL_POLYGON);
                    glVertex3f(size, size,             z);
                    glVertex3f(0.0,  size,             z);
                    glVertex3f(0.0,  size-side_buffer, z);
                    glVertex3f(size, size-side_buffer, z);
                    glEnd();
                    glBegin(GL_POLYGON);
                    glVertex3f(top_buffer, size-side_buffer, z);
                    glVertex3f(0.0,        size-side_buffer, z);
                    glVertex3f(0.0,        side_buffer,      z);
                    glVertex3f(top_buffer, side_buffer,      z);
                    glEnd();

                    //beams
                    glPushMatrix();
                        glTranslatef(top_buffer, side_buffer, z);
                        prism(red, green, blue, horizontal_beam, beam_radius, 0.0, 10, white_vec, 0.0, 0.0, 0.0, 0, 0);
                        glTranslatef(size-top_buffer-bottom_buffer, 0.0, 0.0);
                        prism(red, green, blue, horizontal_beam, beam_radius, 0.0, 10, white_vec, 0.0, 0.0, 0.0, 0, 0);
                    glPopMatrix();
                    glPushMatrix();
                        glTranslatef(top_buffer, side_buffer, z);
                        glRotatef(-90, 0, 0, 1);
                        prism(red, green, blue, vertical_beam, beam_radius, 0.0, 10, white_vec, 0.0, 0.0, 0.0, 0, 0);
                        sphere(red, green, blue, beam_radius, 10, 0);
                        glTranslatef(0.0, vertical_beam, 0.0);
                        sphere(red, green, blue, beam_radius, 10, 0);
                        glTranslatef(0.0,-vertical_beam, 0.0);
                        glTranslatef(-horizontal_beam/2, 0.0, 0.0);
                        prism(red, green, blue, vertical_beam, beam_radius, 0.0, 10, white_vec, 0.0, 0.0, 0.0, 0, 0);
                        glTranslatef(-horizontal_beam/2, 0.0, 0.0);
                        prism(red, green, blue, vertical_beam, beam_radius, 0.0, 10, white_vec, 0.0, 0.0, 0.0, 0, 0);
                        sphere(red, green, blue, beam_radius, 10, 0);
                        glTranslatef(0.0, vertical_beam, 0.0);
                        sphere(red, green, blue, beam_radius, 10, 0);
                    glPopMatrix();
                }

                //front and back
                glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
                glNormal3f(0, -1, 0);

                for (float y = 0; y <= size*1.5; y += size)
                {
                    glNormal3f(0, y ? 1 : -1, 0);
                    glBegin(GL_POLYGON);
                    glVertex3f(0.0,  y, 0.0);
                    glVertex3f(size, y, 0.0);
                    glVertex3f(size, y, size);
                    glVertex3f(0.0,  y, size);
                    glVertex3f(-0.05, y, size/2);
                    glEnd();
                }

                //roof
                glNormal3f(-2, 0, 1);
                glBegin(GL_POLYGON);
                glVertex3f(-0.05, 0.22, size/2);
                glVertex3f(-0.05,-0.02, size/2);
                glVertex3f(0.01,-0.02, (size/2)+0.12);
                glVertex3f(0.01, 0.22, (size/2)+0.12);
                glEnd();
                glNormal3f(-2, 0,-1);
                glBegin(GL_POLYGON);
                glVertex3f(-0.05,-0.02, size/2);
                glVertex3f(-0.05, 0.22, size/2);
                glVertex3f(0.01, 0.22,(size/2)-0.12);
                glVertex3f(0.01,-0.02,(size/2)-0.12);
                glEnd();
                
                //floor
                glNormal3f(1, 0, 0);
                glBegin(GL_POLYGON);
                glVertex3f(size, size, size);
                glVertex3f(size, size, 0.0);
                glVertex3f(size, 0.0,  0.0);
                glVertex3f(size, 0.0,  size);
                glEnd();
                float false_floor_height = 0.035;
                glBegin(GL_POLYGON);
                glVertex3f(size-false_floor_height, size, size);
                glVertex3f(size-false_floor_height, size, 0.0);
                glVertex3f(size-false_floor_height, 0.0,  0.0);
                glVertex3f(size-false_floor_height, 0.0,  size);
                glEnd();

                glDisable(GL_TEXTURE_2D);
            glPopMatrix();
        }
    glPopMatrix();

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
}

void cowcatcher(float red, float green, float blue, const float* color, int texture)
{
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, color);
    glDisable(GL_BLEND);
    glDepthMask(1);

    if (texturing)
    {
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    }

    glPushMatrix();
        SetColor(red, green, blue);
        glBindTexture(GL_TEXTURE_2D, texture);

        //bottom platform
        float top_platform_height    = 0.02;
        float bottom_platform_height = 0.01;
        float bottom_platform_width  = 0.2;
        for (float y=0; y<=bottom_platform_height*1.5; y+=bottom_platform_height)
        {
            glNormal3f(0,-1, 0);
            if (y)
                glNormal3f(0, 1, 0);
            glBegin(GL_POLYGON);
            glVertex3f(0.2,  y, 0.0);
            glVertex3f(0.05, y, bottom_platform_width/2);
            glVertex3f(0.05, y,-bottom_platform_width/2);
            glEnd();
        }
        glPushMatrix();
            //top edge is y=0.01
            glTranslatef(0.0375, 0.005, 0.0);
            glScalef(0.125, 0.05, 1.0);
            cube(red, green, blue, 0.1, 0, 0);
        glPopMatrix();

        glNormal3f(2, 0, 3);
        glBegin(GL_POLYGON);
        glVertex3f(0.2,  bottom_platform_height, 0.0);
        glVertex3f(0.05, bottom_platform_height, 0.1);
        glVertex3f(0.05, 0.0,                    0.1);
        glVertex3f(0.2,  0.0,                    0.0);
        glEnd();
        glNormal3f(2, 0,-3);
        glBegin(GL_POLYGON);
        glVertex3f(0.2,  bottom_platform_height, 0.0);
        glVertex3f(0.05, bottom_platform_height,-0.1);
        glVertex3f(0.05, 0.0,                   -0.1);
        glVertex3f(0.2,  0.0,                    0.0);
        glEnd();

        //vertical connection pillars
        glPushMatrix();
            glTranslatef(0.0375, 0.04, 0.07);
            glScalef(1.0, 3.0, 1.0);
            cube(red, green, blue, 0.01, 0, 0);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(0.0375, 0.04,-0.07);
            glScalef(1.0, 3.0, 1.0);
            cube(red, green, blue, 0.01, 0, 0);
        glPopMatrix();

        //top platform
        glPushMatrix();
            //front edge is  x=0.05
            //bottom edge is y=0.07
            //top edge is    y=0.09
            //side edges are z=+-0.1
            glTranslatef(0.0, 0.08, 0.0);
            glScalef(0.05, top_platform_height/2, 0.1);
            cube(red, green, blue, 1.0, 0, 0);
        glPopMatrix();

        glPushMatrix();
            glTranslatef(0.05, 0.08, 0.09);
            sphere(0.0, 0.0, 0.0, 0.0025, 10, 0);
            glTranslatef(-0.1, 0.0, 0.0);
            sphere(0.0, 0.0, 0.0, 0.0025, 10, 0);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(0.05, 0.08,-0.09);
            sphere(0.0, 0.0, 0.0, 0.0025, 10, 0);
            glTranslatef(-0.1, 0.0, 0.0);
            sphere(0.0, 0.0, 0.0, 0.0025, 10, 0);
        glPopMatrix();
        SetColor(red, green, blue);

        //diagonal bars
        int bars   = 8;
        float edge = 0.085; //distance from center of cowcatcher to middle of outermost bar
        float inc  = edge/bars;
        for (int i=0; i<bars; i++)
        {
            float z = -edge + i*inc*2 + inc/2;
            float x1 = 1.5*(z)     + 0.20;
            float x2 = 1.5*(z+inc) + 0.20;
            if (z > 0)
            {
                x1 = 0.4 - x1;
                x2 = 0.4 - x2;
            }
            vec3 vert = {0.05-fminf(x1, x2), 0.09-bottom_platform_height, 0.0};
            vec3 horz = {x2-x1, 0.0, inc};
            vec3 norm = Cross(vert, horz);
            
            glNormal3f(norm.x, norm.y, norm.z);
            glBegin(GL_POLYGON);
            glTexCoord2f(0.1, 0.5); glVertex3f(0.05, 0.09,                   z);
            glTexCoord2f(0.0, 0.5); glVertex3f(0.05, 0.09,                   z+inc);
            glTexCoord2f(0.0, 0.0); glVertex3f(x2,   bottom_platform_height, z+inc);
            glTexCoord2f(0.1, 0.0); glVertex3f(x1,   bottom_platform_height, z);
            glEnd();
            glNormal3f(-norm.x, -norm.y, -norm.z); //not exactly correct, but it's so close that no one will notice
            glBegin(GL_POLYGON);
            glVertex3f(0.05,     0.09-top_platform_height, z);
            glVertex3f(0.05,     0.09-top_platform_height, z+inc);
            glVertex3f(x2-0.015, bottom_platform_height,   z+inc);
            glVertex3f(x1-0.015, bottom_platform_height,   z);
            glEnd();
            glNormal3f(0, 0,-1);
            glBegin(GL_POLYGON);
            glVertex3f(0.05,     0.09,                     z);
            glVertex3f(x1,       bottom_platform_height,   z);
            glVertex3f(x1-0.015, bottom_platform_height,   z);
            glVertex3f(0.05,     0.09-top_platform_height, z);
            glEnd();
            glNormal3f(0, 0, 1);
            glBegin(GL_POLYGON);
            glVertex3f(0.05,     0.09,                     z+inc);
            glVertex3f(x2,       bottom_platform_height,   z+inc);
            glVertex3f(x2-0.015, bottom_platform_height,   z+inc);
            glVertex3f(0.05,     0.09-top_platform_height, z+inc);
            glEnd();
        }
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}

void railroadGravel()
{
    float y_values[] = {0.0, 0.05, 0.1, 0.12, 0.12, 0.1, 0.05, 0.0};

    int size_of_y_values = sizeof(y_values)/sizeof(float);
    int size = size_of_y_values-1;

    SetColor(0.7, 0.7, 0.7);
    for (int i=0; i<size; i++)
    {
        glNormal3f(0, 1, 0); //temporary
        float z0 = (float)i/size;
        float z1 = (float)(i+1)/size;
        glBegin(GL_POLYGON);
        glTexCoord2f(1.0, z1); glVertex3f( 0.5, y_values[i+1], z1-0.5);
        glTexCoord2f(0.0, z1); glVertex3f(-0.5, y_values[i+1], z1-0.5);
        glTexCoord2f(0.0, z0); glVertex3f(-0.5, y_values[i],   z0-0.5);
        glTexCoord2f(1.0, z0); glVertex3f( 0.5, y_values[i],   z0-0.5);
        glEnd();
    }
}

void railroadTies(int ties, float tie_width_radius, float tie_length_radius, float tie_y)
{
    
    SetColor(0.5, 0.3, 0.2);
    for (int i=0; i<ties; i++)
    {
        float x = (i+0.5)/ties-0.5;
        glNormal3f(0, 1, 0);
        glBegin(GL_POLYGON);
        glTexCoord2f(0.2, 1.0); glVertex3f(x-tie_length_radius, tie_y, tie_width_radius);
        glTexCoord2f(0.0, 0.4); glVertex3f(x+tie_length_radius, tie_y, tie_width_radius);
        glTexCoord2f(0.0, 0.4); glVertex3f(x+tie_length_radius, tie_y,-tie_width_radius);
        glTexCoord2f(0.2, 1.0); glVertex3f(x-tie_length_radius, tie_y,-tie_width_radius);
        glEnd();
    }
}

void railroadIbeams(float beam_separation_radius, float beam_edges_radius, float beam_edges_height, float beam_middle_radius, float beam_middle_height, float tie_y)
{
    glUniform1i(useTexture, 0);

    for (int i=0; i<2; i++)
    {
        float z = i ? -beam_separation_radius : beam_separation_radius;
        SetColor(0.7, 0.7, 0.7);

        //upper edge, top face
        glNormal3f(0, 1, 0);
        glBegin(GL_POLYGON);
        glVertex3f(-0.5, tie_y+beam_edges_height*2+beam_middle_height, z+beam_edges_radius);
        glVertex3f( 0.5, tie_y+beam_edges_height*2+beam_middle_height, z+beam_edges_radius);
        glVertex3f( 0.5, tie_y+beam_edges_height*2+beam_middle_height, z-beam_edges_radius);
        glVertex3f(-0.5, tie_y+beam_edges_height*2+beam_middle_height, z-beam_edges_radius);
        glEnd();

        //upper edge, side faces
        glNormal3f(0, 0, 1);
        glBegin(GL_POLYGON);
        glVertex3f( 0.5, tie_y+beam_edges_height*2+beam_middle_height, z+beam_edges_radius);
        glVertex3f(-0.5, tie_y+beam_edges_height*2+beam_middle_height, z+beam_edges_radius);
        glVertex3f(-0.5, tie_y+beam_edges_height  +beam_middle_height, z+beam_edges_radius);
        glVertex3f( 0.5, tie_y+beam_edges_height  +beam_middle_height, z+beam_edges_radius);
        glEnd();
        glNormal3f(0, 0,-1);
        glBegin(GL_POLYGON);
        glVertex3f( 0.5, tie_y+beam_edges_height*2+beam_middle_height, z-beam_edges_radius);
        glVertex3f(-0.5, tie_y+beam_edges_height*2+beam_middle_height, z-beam_edges_radius);
        glVertex3f(-0.5, tie_y+beam_edges_height  +beam_middle_height, z-beam_edges_radius);
        glVertex3f( 0.5, tie_y+beam_edges_height  +beam_middle_height, z-beam_edges_radius);
        glEnd();

        //lower edge, top face
        glNormal3f(0, 1, 0);
        glBegin(GL_POLYGON);
        glVertex3f(-0.5, tie_y+beam_edges_height, z+beam_edges_radius);
        glVertex3f( 0.5, tie_y+beam_edges_height, z+beam_edges_radius);
        glVertex3f( 0.5, tie_y+beam_edges_height, z-beam_edges_radius);
        glVertex3f(-0.5, tie_y+beam_edges_height, z-beam_edges_radius);
        glEnd();

        //lower edge, side faces
        glNormal3f(0, 0, 1);
        glBegin(GL_POLYGON);
        glVertex3f( 0.5, tie_y+beam_edges_height, z+beam_edges_radius);
        glVertex3f(-0.5, tie_y+beam_edges_height, z+beam_edges_radius);
        glVertex3f(-0.5, tie_y,                   z+beam_edges_radius);
        glVertex3f( 0.5, tie_y,                   z+beam_edges_radius);
        glEnd();
        glNormal3f(0, 0,-1);
        glBegin(GL_POLYGON);
        glVertex3f( 0.5, tie_y+beam_edges_height, z-beam_edges_radius);
        glVertex3f(-0.5, tie_y+beam_edges_height, z-beam_edges_radius);
        glVertex3f(-0.5, tie_y,                   z-beam_edges_radius);
        glVertex3f( 0.5, tie_y,                   z-beam_edges_radius);
        glEnd();

        //middle section, side faces
        SetColor(0.6, 0.6, 0.6);
        glNormal3f(0, 0, 1);
        glBegin(GL_POLYGON);
        glVertex3f( 0.5, tie_y+beam_edges_height+beam_middle_height, z+beam_middle_radius);
        glVertex3f(-0.5, tie_y+beam_edges_height+beam_middle_height, z+beam_middle_radius);
        glVertex3f(-0.5, tie_y+beam_edges_height,                    z+beam_middle_radius);
        glVertex3f( 0.5, tie_y+beam_edges_height,                    z+beam_middle_radius);
        glEnd();
        glNormal3f(0, 0,-1);
        glBegin(GL_POLYGON);
        glVertex3f( 0.5, tie_y+beam_edges_height+beam_middle_height, z-beam_middle_radius);
        glVertex3f(-0.5, tie_y+beam_edges_height+beam_middle_height, z-beam_middle_radius);
        glVertex3f(-0.5, tie_y+beam_edges_height,                    z-beam_middle_radius);
        glVertex3f( 0.5, tie_y+beam_edges_height,                    z-beam_middle_radius);
        glEnd();
    }
    
    glUniform1i(useTexture, 1);
}

void railroad(const float* color, int texture_gravel, int texture_wood)
{
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, color);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
    glDisable(GL_BLEND);
    glDepthMask(1);

    if (texturing)
    {
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    }

    int ties = 4;
    float tie_width_radius  = 0.3;
    float tie_length_radius = 0.05;
    float tie_y = 0.125;
    float beam_separation_radius = tie_width_radius-0.1;
    float beam_edges_radius  = 0.030;
    float beam_edges_height  = 0.015;
    float beam_middle_radius = 0.010;
    float beam_middle_height = 0.030;

    glBindTexture(GL_TEXTURE_2D, texture_gravel);
    railroadGravel();

    glBindTexture(GL_TEXTURE_2D, texture_wood);
    railroadTies(ties, tie_width_radius, tie_length_radius, tie_y);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    railroadIbeams(beam_separation_radius, beam_edges_radius, beam_edges_height, beam_middle_radius, beam_middle_height, tie_y);

    glDisable(GL_TEXTURE_2D);
}

void smogParticles(float* position)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //update all old particle positions
    for (int i = 0; i < particles_size; i++) 
    {
        float* position_cyl = toCylindrical(particles[i*particles_attributes+0], particles[i*particles_attributes+1], particles[i*particles_attributes+2]);
        float random = (float)rand() / RAND_MAX;
        position_cyl[0] -= 0.002*random;
        switch (i % 4)
        {
            case 0:
                position_cyl[1] -= 0.00015*random;
                break;
            case 1:
                position_cyl[1] += 0.00015*random;
                break;
            case 2:
                position_cyl[2] -= 0.00015*random;
                break;
            case 3:
                position_cyl[2] += 0.00015*random;
                break;
        }

        float* position_xyz = toCartesian(position_cyl[0], position_cyl[1], position_cyl[2]);

        particles[i*particles_attributes+0] = position_xyz[0];
        particles[i*particles_attributes+1] = position_xyz[1];
        particles[i*particles_attributes+2] = position_xyz[2];
        particles[i*particles_attributes+3]--;
        if (particles[i*particles_attributes+3] < 0) particles[i*particles_attributes+3] = 0;
        particles[i*particles_attributes+4] = 0.3; //size
    }

    //create 1 new particle
    create_particle = (create_particle + 1) % particles_size;
    particles[create_particle*particles_attributes+0] = position[0]; //x
    particles[create_particle*particles_attributes+1] = position[1]; //y
    particles[create_particle*particles_attributes+2] = position[2]; //z
    particles[create_particle*particles_attributes+3] = particles_size; 

    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_POINT_SPRITE);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particles), particles, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    GLsizei stride = particles_attributes*sizeof(float);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, stride, (void*)(4 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glUseProgram(shader[2]);
    glDrawArrays(GL_POINTS, 0, particles_size);
    glUseProgram(shader[1]);

    glDisable(GL_BLEND);
}

void train(float wheel_speed)
{
    glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, shiny);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  white_vec);
    glUniform1i(useTexture, 0);

    vec3 train_gray  = {0.2, 0.2, 0.2};
    vec3 train_red   = {0.8, 0.1, 0.1};
    vec3 train_brown = {0.6, 0.5, 0.3};
    vec3 train_gold  = {0.9, 0.7, 0.0};
    vec3 train_lightgray  = {0.7, 0.7, 0.7};
    float train_length = 0.5;
    float train_radius = 0.07;

    glPushMatrix();
        glRotatef(-90, 0, 0, 1);
        glTranslatef(0.0, -0.2, 0.0);

        //black front of the body
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(-1.0, 0.0);
        glPushMatrix();
            glRotatef(180, 1, 0, 0);
            circle(train_gray.x, train_gray.y, train_gray.z, train_radius, 50, 0);
        glPopMatrix();
        glDisable(GL_POLYGON_OFFSET_FILL);
        prism(train_gray.x, train_gray.y, train_gray.z, train_length, train_radius, 0.0, 50, white_vec, 0.0, 0.0, 0.0, 0, 0);
        glPushMatrix();
            glTranslatef(0.0, train_length, 0.0);
            circle(train_gray.x, train_gray.y, train_gray.z, train_radius, 50, 0);
            float y_increment = 0.005;
            float r_decrement = 0.01;
            for (int i=0; i<4; i++)
            {
                train_radius -= r_decrement;
                prism(train_gray.x, train_gray.y, train_gray.z, y_increment, train_radius, 0.0, 20, white_vec, 0.0, 0.0, 0.0, 0, 0);
                glTranslatef(0.0, y_increment, 0.0);
                circle(train_gray.x, train_gray.y, train_gray.z, train_radius, 50, 0);
            }
        glPopMatrix();
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

        //black smokestack
        glPushMatrix();
            glTranslatef(0.0, train_length-0.04, 0.0);
            glRotatef(90, 0, 0, 1);
            prism(train_gray.x, train_gray.y, train_gray.z, 0.1, 0.02, 0.0, 40, white_vec, 0.0, 0.0, 0.0, 0, 0);
            glRotatef(180, 0, 0, 1);
            glTranslatef(0.0, -0.2, 0.0);
            spike(train_gray.x, train_gray.y, train_gray.z, 0.2, 0.04, 40, 0);
            glTranslatef(0.0, -0.025, 0.0);
            prism(train_gray.x, train_gray.y, train_gray.z, 0.025, 0.04, 0.005, 40, white_vec, 0.0, 0.0, 0.0, 0, 0);
            glTranslatef(0.0, 0.010, 0.0);
            circle(0.0, 0.0, 0.0, 0.04, 20, 0);
        glPopMatrix();
        glDisable(GL_CULL_FACE);

        //red wheels
        float axel_length = 0.154;
        float wheel_thickness = 0.015;
        float wheel_radius = 0.065;
        float wheel_distance = 0.15;
        float tire_thickness = 0.012;
        float wheel_small_radius = 0.03;
        float wheel_small_speed = wheel_speed*(wheel_small_radius/wheel_radius);
        glPushMatrix();
            glTranslatef(0.08, 0.05, (axel_length-wheel_thickness)/2);
            wheelLarge(train_red.x, train_red.y, train_red.z, wheel_thickness, wheel_radius, tire_thickness, 40, 16, wheel_speed, white_vec, 0);
            glPushMatrix();
                glRotatef(-90, 1, 0, 0);
                prism(train_gray.x, train_gray.y, train_gray.z, axel_length, 0.01, 0.0, 20, white_vec, 0, 0, 0, 0, 0);
            glPopMatrix();
            glTranslatef(0.0, wheel_distance, 0.0);
            wheelLarge(train_red.x, train_red.y, train_red.z, wheel_thickness, wheel_radius, tire_thickness, 40, 16, wheel_speed, white_vec, 0);
            glPushMatrix();
                glRotatef(-90, 1, 0, 0);
                prism(train_gray.x, train_gray.y, train_gray.z, axel_length, 0.01, 0.0, 20, white_vec, 0, 0, 0, 0, 0);
            glPopMatrix();
            glPushMatrix();
                glTranslatef(wheel_radius-wheel_small_radius, 0.18, 0.0);
                wheelSmall(train_red.x, train_red.y, train_red.z, wheel_thickness, wheel_small_radius, 20, wheel_small_speed, white_vec, 0);
                glPushMatrix();
                    glRotatef(-90, 1, 0, 0);
                    prism(train_gray.x, train_gray.y, train_gray.z, axel_length, 0.01, 0.0, 20, white_vec, 0, 0, 0, 0, 0);
                glPopMatrix();
                glTranslatef(-0.015, wheel_distance, 0.0);
                wheelSmall(train_red.x, train_red.y, train_red.z, wheel_thickness, wheel_small_radius, 20, wheel_small_speed, white_vec, 0);
                glPushMatrix();
                    glRotatef(-90, 1, 0, 0);
                    prism(train_gray.x, train_gray.y, train_gray.z, axel_length, 0.01, 0.0, 20, white_vec, 0, 0, 0, 0, 0);
                glPopMatrix();
            glPopMatrix();
        glPopMatrix();
        glPushMatrix();
            glTranslatef(0.08, 0.05, (-axel_length-wheel_thickness)/2);
            wheelLarge(train_red.x, train_red.y, train_red.z, wheel_thickness, wheel_radius, tire_thickness, 20, 16, wheel_speed, white_vec, 0);
            glTranslatef(0.0, wheel_distance, 0.0);
            wheelLarge(train_red.x, train_red.y, train_red.z, wheel_thickness, wheel_radius, tire_thickness, 20, 16, wheel_speed, white_vec, 0);
            glPushMatrix();
                glTranslatef(wheel_radius-wheel_small_radius, 0.18, wheel_thickness);
                glRotatef(180, 1, 0, 0);
                wheelSmall(train_red.x, train_red.y, train_red.z, wheel_thickness, wheel_small_radius, 20,-wheel_small_speed, white_vec, 0);
                glTranslatef(-0.015,-wheel_distance, 0.0);
                wheelSmall(train_red.x, train_red.y, train_red.z, wheel_thickness, wheel_small_radius, 20,-wheel_small_speed, white_vec, 0);
            glPopMatrix();
        glPopMatrix();

        //undercarriage
        glPushMatrix();
            glTranslatef(0.045, train_length/2, 0.0);
            glScalef(0.03, train_length/2, train_radius*0.8);
            cube(train_gray.x, train_gray.y, train_gray.z, 1.0, 0, 0);
        glPopMatrix();

        //axel grabbers
        float distance_along_axel = (axel_length-wheel_thickness)/3;
        float grabber_thickness = 0.01;
        float eccentricity = 0.6;
        float grabber_radius = 0.03;
        glPushMatrix();
            glTranslatef(train_radius/2+wheel_radius-wheel_small_radius, 0.05, distance_along_axel);
            glRotatef(-90, 1, 0, 0);
            glScalef(1.0, 1.0, eccentricity);
            glPushMatrix();
                prism(train_gray.x, train_gray.y, train_gray.z, grabber_thickness, grabber_radius, grabber_radius, 20, white_vec, 0, 0, 0, 0, 0);
                glTranslatef(0.0, distance_along_axel*2-grabber_thickness, 0.0);
                prism(train_gray.x, train_gray.y, train_gray.z, grabber_thickness, grabber_radius, grabber_radius, 20, white_vec, 0, 0, 0, 0, 0);
            glPopMatrix();
            glTranslatef(0.0, 0.0, wheel_distance/eccentricity);
            glPushMatrix();
                prism(train_gray.x, train_gray.y, train_gray.z, grabber_thickness, grabber_radius, grabber_radius, 20, white_vec, 0, 0, 0, 0, 0);
                glTranslatef(0.0, distance_along_axel*2-grabber_thickness, 0.0);
                prism(train_gray.x, train_gray.y, train_gray.z, grabber_thickness, grabber_radius, grabber_radius, 20, white_vec, 0, 0, 0, 0, 0);
            glPopMatrix();
        glPopMatrix();
        
        glPushMatrix();
            glTranslatef(train_radius/2+wheel_radius+0.003, 0.08+wheel_distance*2, distance_along_axel);
            glRotatef(-90, 1, 0, 0);
            glScalef(1.0, 1.0, eccentricity);
            glPushMatrix();
                prism(train_gray.x, train_gray.y, train_gray.z, grabber_thickness, grabber_radius, grabber_radius, 20, white_vec, 0, 0, 0, 0, 0);
                glPushMatrix();
                    glTranslatef(-grabber_radius/2, grabber_thickness/2, 0.0);
                    glScalef(eccentricity, grabber_thickness/grabber_radius/2, 1.0);
                    cube(train_gray.x, train_gray.y, train_gray.z, grabber_radius, 0, 0);
                glPopMatrix();
                glTranslatef(0.0, distance_along_axel*2-grabber_thickness, 0.0);
                prism(train_gray.x, train_gray.y, train_gray.z, grabber_thickness, grabber_radius, grabber_radius, 20, white_vec, 0, 0, 0, 0, 0);
                glPushMatrix();
                    glTranslatef(-grabber_radius/2, grabber_thickness/2, 0.0);
                    glScalef(eccentricity, grabber_thickness/grabber_radius/2, 1.0);
                    cube(train_gray.x, train_gray.y, train_gray.z, grabber_radius, 0, 0);
                glPopMatrix();
            glPopMatrix();
            glTranslatef(-0.015, 0.0, wheel_distance/eccentricity);
            glPushMatrix();
                prism(train_gray.x, train_gray.y, train_gray.z, grabber_thickness, grabber_radius, grabber_radius, 20, white_vec, 0, 0, 0, 0, 0);
                glPushMatrix();
                    glTranslatef(-grabber_radius/2, grabber_thickness/2, 0.0);
                    glScalef(eccentricity, grabber_thickness/grabber_radius/2, 1.0);
                    cube(train_gray.x, train_gray.y, train_gray.z, grabber_radius, 0, 0);
                glPopMatrix();
                glTranslatef(0.0, distance_along_axel*2-grabber_thickness, 0.0);
                prism(train_gray.x, train_gray.y, train_gray.z, grabber_thickness, grabber_radius, grabber_radius, 20, white_vec, 0, 0, 0, 0, 0);
                glPushMatrix();
                    glTranslatef(-grabber_radius/2, grabber_thickness/2, 0.0);
                    glScalef(eccentricity, grabber_thickness/grabber_radius/2, 1.0);
                    cube(train_gray.x, train_gray.y, train_gray.z, grabber_radius, 0, 0);
                glPopMatrix();
            glPopMatrix();
        glPopMatrix();

        //wheel connector things
        float connector_thickness = 0.004;
        float joint_thickness = 0.005;
        float conductor_r = wheel_radius/2;
        glPushMatrix();
            glTranslatef(Sin(wheel_speed)*conductor_r, Cos(wheel_speed)*conductor_r, 0.0);
            glTranslatef(0.08, 0.05+wheel_distance/2, (axel_length+wheel_thickness)/2+joint_thickness);
            glPushMatrix();
                glScalef(0.008, wheel_distance/2, connector_thickness);
                cube(train_lightgray.x, train_lightgray.y, train_lightgray.z, 1.0, 0, 0);
            glPopMatrix();
            glPushMatrix();
                glTranslatef(0.0,-wheel_distance/2, 0.0);
                glScalef(2.0, 2.5, 1.0);
                cube(train_lightgray.x, train_lightgray.y, train_lightgray.z, joint_thickness, 0, 0);
            glPopMatrix();
            glPushMatrix();
                glTranslatef(0.0, wheel_distance/2, 0.0);
                glScalef(2.0, 2.5, 1.0);
                cube(train_lightgray.x, train_lightgray.y, train_lightgray.z, joint_thickness, 0, 0);
            glPopMatrix();
        glPopMatrix();

        glPushMatrix();
            glTranslatef(Sin(wheel_speed)*conductor_r, Cos(wheel_speed)*conductor_r, 0.0);
            glTranslatef(0.08, 0.05+wheel_distance/2,-(axel_length+wheel_thickness)/2-joint_thickness);
            glPushMatrix();
                glScalef(0.008, wheel_distance/2, connector_thickness);
                cube(train_lightgray.x, train_lightgray.y, train_lightgray.z, 1.0, 0, 0);
            glPopMatrix();
            glPushMatrix();
                glTranslatef(0.0,-wheel_distance/2, 0.0);
                glScalef(2.0, 2.5, 1.0);
                cube(train_lightgray.x, train_lightgray.y, train_lightgray.z, joint_thickness, 0, 0);
            glPopMatrix();
            glPushMatrix();
                glTranslatef(0.0, wheel_distance/2, 0.0);
                glScalef(2.0, 2.5, 1.0);
                cube(train_lightgray.x, train_lightgray.y, train_lightgray.z, joint_thickness, 0, 0);
            glPopMatrix();
        glPopMatrix();

        //cowcatcher
        glPushMatrix();
            glTranslatef(0.125, train_length+0.045, 0.0);
            glRotatef(4, 0, 0, 1);
            glRotatef(90, 0, 0, 1);
            glScalef(0.9, 0.9, 0.9);
            cowcatcher(train_red.x, train_red.y, train_red.z, white_vec, 0);
        glPopMatrix();

        //body-cowcatcher connectors
        glPushMatrix();
            glTranslatef(0.0, train_length-0.02, 0.073);
            glPushMatrix();
                glRotatef(-35, 0, 0, 1);
                sphere(train_gray.x, train_gray.y, train_gray.z, 0.003, 10, 0);
                prism(train_gray.x, train_gray.y, train_gray.z, 0.08, 0.003, 0.0, 10, white_vec, 0.0, 0.0, 0.0, 0, 0);
            glPopMatrix();
            glTranslatef(0.0, 0.0,-0.012);
            cube(train_gray.x, train_gray.y, train_gray.z, 0.01, 0, 0);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(0.0, train_length-0.02,-0.073);
            glPushMatrix();
                glRotatef(-35, 0, 0, 1);
                sphere(train_gray.x, train_gray.y, train_gray.z, 0.003, 10, 0);
                prism(train_gray.x, train_gray.y, train_gray.z, 0.08, 0.003, 0.0, 10, white_vec, 0.0, 0.0, 0.0, 0, 0);
            glPopMatrix();
            glTranslatef(0.0, 0.0, 0.012);
            cube(train_gray.x, train_gray.y, train_gray.z, 0.01, 0, 0);
        glPopMatrix();

        //gold bands
        float cabin_size = 0.2;
        float band_thickness = 0.006;
        float band_distance = 0.07;
        glPushMatrix();
            prism(train_gold.x, train_gold.y, train_gold.z, band_thickness, train_radius+0.0001, 0.0, 50, white_vec, 0.0, 0.0, 0.0, 0, 0);
            glTranslatef(0.0, 0.6*cabin_size+band_thickness, 0.0);
            prism(train_gold.x, train_gold.y, train_gold.z, band_thickness, train_radius+0.0001, 0.0, 50, white_vec, 0.0, 0.0, 0.0, 0, 0);
            for (int i = 0; i < 4; i++)
            {
                glTranslatef(0.0, band_distance, 0.0);
                prism(train_gold.x, train_gold.y, train_gold.z, band_thickness, train_radius+0.0001, 0.0, 50, white_vec, 0.0, 0.0, 0.0, 0, 0);
            }
        glPopMatrix();

        //gold railing
        float railing_pole_height = 0.09;
        float railing_pole_radius = 0.004;
        float railing_lower_length = 0.37;
        float railing_upper_length = 0.28;
        float railing_upper_z = cabin_size*0.21;
        for (int side = 0; side < 2; side++)
        {
            glPushMatrix();
            glPushAttrib(GL_POLYGON_BIT);
                if (side) 
                {
                    glScalef(1.0, 1.0, -1.0);
                    glFrontFace(GL_CW);
                }
                glPushMatrix();
                    glTranslatef(0.0, railing_pole_radius, cabin_size*0.9/2);
                    glRotatef(-90, 1, 0, 0);
                    prism(train_gold.x, train_gold.y, train_gold.z, 0.02, railing_pole_radius, 0.0, 10, white_vec, 0, 0, 0, 0, 0);
                    glTranslatef(0.0, 0.0, cabin_size*0.6+band_thickness-railing_pole_radius/2);
                    prism(train_gold.x, train_gold.y, train_gold.z, 0.02, railing_pole_radius, 0.0, 10, white_vec, 0, 0, 0, 0, 0);
                glPopMatrix();
                glPushMatrix();
                    glTranslatef(0.0, railing_pole_radius, cabin_size*0.9/2);
                    sphere(train_gold.x, train_gold.y, train_gold.z, railing_pole_radius, 10, 0);
                    prism(train_gold.x, train_gold.y, train_gold.z, railing_lower_length, railing_pole_radius, 0.0, 10, white_vec, 0, 0, 0, 0, 0);
                    glTranslatef(0.0, railing_lower_length, 0.0);
                    sphere(train_gold.x, train_gold.y, train_gold.z, railing_pole_radius, 10, 0);
                    glRotatef(-90, 1, 0, 0);
                    prism(train_gold.x, train_gold.y, train_gold.z, 0.05, railing_pole_radius, 0.0, 10, white_vec, 0, 0, 0, 0, 0);
                glPopMatrix();
                glPushMatrix();
                    glTranslatef(-0.08, 0.6*cabin_size+band_thickness, railing_upper_z);
                    prism(train_gold.x, train_gold.y, train_gold.z, railing_upper_length, railing_pole_radius, 0.0, 10, white_vec, 0, 0, 0, 0, 0);
                    glTranslatef(0.0, railing_upper_length, 0.0);
                    sphere(train_gold.x, train_gold.y, train_gold.z, railing_pole_radius, 10, 0);
                glPopMatrix();
                glPushMatrix();
                    glTranslatef(0.0, 0.129-band_distance/2, railing_upper_z);
                    for (int i = 0; i < 4; i++)
                    {
                        glTranslatef(0.0, band_distance, 0.0);
                        glPushMatrix();
                            glRotatef(90, 0, 0, 1);
                            prism(train_gold.x, train_gold.y, train_gold.z, railing_pole_height, railing_pole_radius, 0.0, 10, white_vec, 0, 0, 0, 0, 0);
                            glTranslatef(0.0, railing_pole_height, 0.0);
                            spike(train_gold.x, train_gold.y, train_gold.z, railing_pole_height/10, railing_pole_radius, 10, 0);
                        glPopMatrix();
                    }
                glPopMatrix();
            glPopAttrib();
            glPopMatrix();
        }
        //wooden cabin
        glPushMatrix();
            glTranslatef(-0.12, band_thickness, 0.0);
            glScalef(0.6, 0.6, 0.9);
            cabin(train_brown.x, train_brown.y, train_brown.z, cabin_size, white_vec, 0, 1);
            cabin(train_brown.x, train_brown.y, train_brown.z, cabin_size, white_vec, 0, 0);
        glPopMatrix();

    glPopMatrix();

    //cleanup
    glDisable(GL_BLEND);
    glDepthMask(1);
    glDisable(GL_TEXTURE_2D);
    glUniform1i(useTexture, 1);
}

void habitat(float red, float green, float blue, float inner_red, float inner_green, float inner_blue, int opaque)
{
    glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, shiny);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  white_vec);

    float inc = 360.0/habitat_sides;

    //grass
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
    SetColor(red, green, blue);
    int rows = 8;
    if (opaque)
    {
        glPushMatrix();
        for (int i=0; i<rows; i++)
        {
            //edge rows are gray, inner rows are all grass
            if (i == 0 || i == rows-1)
            {
                glUniform1i(useTexture, 0);
                prism(red, green, blue, habitat_height/rows, habitat_radius*1.1, habitat_radius/10, habitat_sides, green_vec, red, green, blue, 0, 0);
                glUniform1i(useTexture, 1);
            }
            else
            {
                prism(red, green, blue, habitat_height/rows, habitat_radius*1.1, habitat_radius/10, habitat_sides, green_vec, inner_red, inner_green, inner_blue, textures[2], 0);
                //railroad
                if (i == 1 || i == rows-2)
                {
                    glPushMatrix();
                        glTranslatef(0.0, habitat_height/rows/2, 0.0);
                        float railroad_scale = 0.0525;
                        for (int j=0; j<habitat_sides; j++)
                        {
                            glRotatef(360/habitat_sides, 0, 1, 0);
                            glPushMatrix();
                                glTranslatef(habitat_radius, 0.0, 0.0);
                                glRotatef(90, 1, 0, 0);
                                glRotatef(90, 0, 0, 1);
                                glScalef(railroad_scale, railroad_scale, railroad_scale);
                                railroad(white_vec, textures[7], textures[3]);
                            glPopMatrix();
                        }

                        //train
                        float train_scale = 0.14;
                        if (i == 1)
                        {
                            glPushMatrix();
                                glRotatef(-roll/18, 0, 1, 0);
                                glTranslatef(habitat_radius*0.94, 0.0, 0.0);
                                glRotatef(90, 1, 0, 0);
                                glRotatef(90, 0, 0, 1);
                                glScalef(train_scale, train_scale, train_scale);
                                train(roll*2);
                            glPopMatrix();
                            
                            //draw smog particles
                            /*glUseProgram(shader[2]);
                            float* position = toCartesian(habitat_radius*0.88, roll/18+5, 0.0);
                            smogParticles(position);
                            glUseProgram(shader[1]);*/
                        }
                        else
                        {
                            glPushMatrix();
                                glTranslatef(habitat_radius*0.94, 0.0, 0.0);
                                glRotatef(-90, 1, 0, 0);
                                glRotatef(90, 0, 0, 1);
                                glScalef(train_scale, train_scale, train_scale);
                                train(0);
                            glPopMatrix();
                        }
                    glPopMatrix();
                }
            }
            glTranslatef(0, habitat_height/rows, 0);
        }
        glPopMatrix();

        //steel connecting rods
        float rod_width = 2*inc;
        SetColor(red, green, blue);
        float rod_side = 0.0;
        glUniform1i(useTexture, 0);
        for (int y = 0; y < 2; y++)
        {
            //outer parts
            glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
            glPushMatrix();
            glBegin(GL_POLYGON);
            glNormal3f(0, y ? 1 : -1, 0);
            for (int i = 0; i < habitat_sides; i++)
            {
                float theta = (i+0.5)*inc;
                if ( (fabsf(180 - theta) < rod_width) || (fabsf(0 - theta) < rod_width) || (fabsf(360 - theta) < rod_width) )
                {
                    if (rod_side < 90) {rod_side = theta;}
                    glVertex3f(Cos(theta)*habitat_radius, habitat_height*y, Sin(theta)*habitat_radius);
                }
            }
            glEnd();
            glPopMatrix();
            
            //inner parts
            glPushMatrix();
            glBegin(GL_POLYGON);
            glNormal3f(0, y ? -1 : 1, 0);
            for (int i = 0; i < habitat_sides; i++)
            {
                float theta = (i+0.5)*inc;
                if ( (fabsf(180 - theta) < rod_width) || (fabsf(0 - theta) < rod_width) || (fabsf(360 - theta) < rod_width) )
                {
                    glVertex3f(Cos(theta)*habitat_radius, y ? (habitat_height * (rows-1)/rows) : (habitat_height * 1/rows), Sin(theta)*habitat_radius);
                }
            }
            glEnd();
            glPopMatrix();
            
            //sides
            float one =     rod_side;
            float two = 180-rod_side;
            int inc = 180;
            glPushMatrix();
            for (int i = 0; i < 360; i += inc)
            {
                glNormal3f(0, 0, 1);
                glBegin(GL_POLYGON);
                glVertex3f(Cos(one)*habitat_radius, 0,                       Sin(one)*habitat_radius);
                glVertex3f(Cos(one)*habitat_radius, habitat_height * 1/rows, Sin(one)*habitat_radius);
                glVertex3f(Cos(two)*habitat_radius, habitat_height * 1/rows, Sin(two)*habitat_radius);
                glVertex3f(Cos(two)*habitat_radius, 0,                       Sin(two)*habitat_radius);
                glEnd();

                glNormal3f(0, 0,-1);
                glBegin(GL_POLYGON);
                glVertex3f(Cos((one+inc))*habitat_radius, habitat_height * (rows-1)/rows, Sin((one+inc))*habitat_radius);
                glVertex3f(Cos((one+inc))*habitat_radius, habitat_height,                 Sin((one+inc))*habitat_radius);
                glVertex3f(Cos((two+inc))*habitat_radius, habitat_height,                 Sin((two+inc))*habitat_radius);
                glVertex3f(Cos((two+inc))*habitat_radius, habitat_height * (rows-1)/rows, Sin((two+inc))*habitat_radius);
                glEnd();

                glRotatef(inc, 0, 1, 0);
            }
            glPopMatrix();
        }
        glUniform1i(useTexture, 1);

        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
    }

    //windows
    if (!opaque)
    {
        glEnable(GL_BLEND);
        SetColor(0.5, 0.5, 0.5);
        glColor4f(0.5, 0.5, 0.5, 0.8);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(0);
        glEnable(GL_TEXTURE_2D);

        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
        glUniform1i(useTexture, 0);
        for (int y = 0; y < 2;     y++)
        {
            glPushMatrix();
                glBegin(GL_POLYGON);
                glNormal3f(0, y ? 1 : -1, 0);
                for (int i = 0; i < habitat_sides; i++)
                {
                    float theta = (i+0.5)*inc;
                    glVertex3f(Cos(theta)*habitat_radius, (0.99*habitat_height*y)+0.001, Sin(theta)*habitat_radius);
                }
                glEnd();
            glPopMatrix();
        }

        glPushMatrix();
            glTranslatef(0.0, 3*habitat_height/rows/2, 0.0);
            
            //draw smog particles
            glUseProgram(shader[2]);
            float* position = toCartesian(habitat_radius*0.88, roll/18+5, 0.0);
            smogParticles(position);
            glUseProgram(shader[1]);
        glPopMatrix();
    }
    
    glDisable(GL_BLEND);
    glDepthMask(1);
    glDisable(GL_TEXTURE_2D);
    glUniform1i(useTexture, 1);
}

void leaf(float red, float green, float blue)
{
    float greenish_red   = red/3 + 0.00;
    float greenish_green = green + 0.70;
    float greenish_blue  = blue  + 0.10;

    sphere(greenish_red, greenish_green, greenish_blue, 0.012, 12, textures[2]);
}

void branch(float red, float green, float blue, float radius, float height, int remaining_levels)
{
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
    SetColor(red, green, blue);

    glDisable(GL_BLEND);
    glDepthMask(1);

    int sides = 6;
    SetColor(red, green, blue);

    glBindTexture(GL_TEXTURE_2D, textures[8]);
    glPushMatrix();
        float inc = 360.0/sides;
        for (int i = 0; i < sides; i++)
        {
            glRotatef(inc, 0, 1, 0);
            glBegin(GL_QUADS);
            glNormal3f( Sin(inc/2)*radius, 0.0,    Cos(inc/2)*radius);
            glTexCoord2f(1.0, 1.0); 
            glVertex3f( Sin(inc/2)*radius, height, Cos(inc/2)*radius);
            glTexCoord2f(1.0, 0.0); 
            glVertex3f( Sin(inc/2)*radius, 0.0,    Cos(inc/2)*radius);
            glNormal3f(-Sin(inc/2)*radius, 0.0,    Cos(inc/2)*radius);
            glTexCoord2f(0.0, 0.0); 
            glVertex3f(-Sin(inc/2)*radius, 0.0,    Cos(inc/2)*radius);
            glTexCoord2f(0.0, 1.0); 
            glVertex3f(-Sin(inc/2)*radius, height, Cos(inc/2)*radius);
            glEnd();
        }
    glPopMatrix();

    glTranslatef(0.0, height*0.95, 0.0);
    if (remaining_levels > 0)
    {
        int branches = 2;
        for (int i = 0; i < branches; i++)
        {
            glPushMatrix();
                glRotatef(rand() % 360, 0, 1, 0);
                glRotatef(10 + rand() % 50, 1, 0, 0);
                glBindTexture(GL_TEXTURE_2D, textures[3]);
                branch(red, green, blue, radius*0.8, height*0.6, remaining_levels-1);
            glPopMatrix();
        }
    }
    else
    {
        leaf(red, green, blue);
    }
}

void tree(float red, float green, float blue, float radius, float r, float th, float h, float angle, int seed)
{
    int remaining_levels = 2; 
    float height = 0.03;

    float brownish_red   = red/4   + 0.30;
    float brownish_green = green/4 + 0.10;
    float brownish_blue  = blue/4  + 0.10;

    if (texturing)
    {
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glBindTexture(GL_TEXTURE_2D, textures[3]);
    }

    SetColor(brownish_red, brownish_green, brownish_blue);

    srand(seed);
    glPushMatrix();
        glRotatef(th, 0, -1, 0);
        glTranslatef(0, h, 0);
        glRotatef(angle, 1, 0, 0);
        glTranslatef(r - radius*1.06, 0, 0);
        glRotatef(90, 0, 0, 1);
        glPushMatrix();
            branch(brownish_red, brownish_green, brownish_blue, radius, height, remaining_levels);
        glPopMatrix();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
    srand(roll);
}

void displayScene()
{
    switch (debug_mode)
    {
        //display whole scene
        case 0:
            //Disable the player's light for all celestial bodies
            glDisable(GL_LIGHT0);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);

            //Earth
            glPushMatrix();
            glTranslatef(0.0, -5.0, 0.0);
            glRotatef(orbit_habitat, 0, Sin(23.5), Cos(23.5));
            glRotatef(orbit_earth+90,   0, 0, 1);
            sphere(1.0, 1.0, 1.0, 4.0, 60, textures[4]);
            glPopMatrix();

            //Sun
            glDisable(GL_LIGHTING);
            glPushMatrix();
            glRotatef(orbit_habitat, 0, Sin(23.5), Cos(23.5));
            glTranslatef(0.0, sun_distance*Cos(23.5), -sun_distance*Sin(23.5));
            glRotatef(orbit_earth/3, 0, 0, 1); //realistically the sun's rotation around its axis would be orbit_earth/25
            sphere(1.0, 1.0, 1.0, 1.0, 15, textures[5]);
            glPopMatrix();
            glEnable(GL_LIGHTING);
            
            glDisable(GL_CULL_FACE);

            //Stars
            glDisable(GL_LIGHTING);
            glUseProgram(0);
            glPushMatrix();
            glRotatef(orbit_habitat, 0, Sin(23.5), Cos(23.5));
            glRotatef(-23.5,          1, 0, 0);
            sphere(0.3, 0.3, 0.3, 45.0, 20, textures[6]);
            glPopMatrix();
            glEnable(GL_LIGHTING);
            glUseProgram(shader[1]);
            glEnable(GL_LIGHT0);

            //Ships
            ship(0.2,  0.06, 12, 0.82, roll,     yaw+140,    160       );
            ship(0.15, 0.04, 12, 0.78, roll*1.3, yaw*1.3+30, 0         );
            ship(0.05, 0.02, 10, 0.91, roll*1.7, yaw*1.8,    yaw*0.1+80);

            //Habitat
            habitat(0.4, 0.4, 0.4, 0.1, 0.9, 0.1, 1);
            habitat(0.4, 0.4, 0.4, 0.1, 0.9, 0.1, 0);

            //Houses
            for(int i = 0; i < houses_size; i++)
            {
                if (house_radius > 0)
                    house(houses[i][0], houses[i][1], houses[i][2], houses[i][3], houses[i][4], houses[i][5], houses[i][6], houses[i][7]);
            }

            //Trees
            for(int i = 0; i < trees_size; i++)
            {
                if (tree_radius > 0)
                    tree(trees[i][0], trees[i][1], trees[i][2], trees[i][3], trees[i][4], trees[i][5], trees[i][6], trees[i][7], trees[i][8]);
            }
            break;

        //debug mode (display objects one at a time)
        case 1:
            train(roll);
            break;
        case 2:
            habitat(0.4, 0.4, 0.4, 0.1, 0.9, 0.1, 1);
            habitat(0.4, 0.4, 0.4, 0.1, 0.9, 0.1, 0);
            break;
        case 3:
            railroad(white_vec, textures[7], textures[3]);
            //jack(0.8, 0.3, 0.5, 0.5, 0.0, 0.0, 0.0, 0, 0, 0);
            break;
        case 4:
            glUniform1i(useTexture, 0);
            glPushMatrix();
                glScalef(10, 10, 10);
                tree(0.8, 0.3, 0.5, tree_radius, 0.0, 0.0, 0.0, 0, 12943);
            glPopMatrix();
            //spike(0.8, 0.3, 0.5, 0.5, 0.1, debug_sides, 0);
            glUniform1i(useTexture, 1);
            break;
        case 5:
            glUniform1i(useTexture, 0);
            prism(0.8, 0.3, 0.5, 0.5, 0.1, 0.0, debug_sides, white_vec, 0.0, 0.0, 0.0, 0, 0);
            glUniform1i(useTexture, 1);
            break;
        case 6:
            ship(0.3, 0.1, debug_sides, 0.0, 0.0, 0.0, 0.0);
            break;
        case 7:
            house(0.8, 0.3, 0.5, 0.25, 0.0, 0.0, 0.0, 0);
            break;
        default:
            Fatal("\'unique_objects\' is too large");
            break;
    }
}

void setPosition()
{
    if (player_th >= 360) {player_th -= 360;}
    if (player_th <  0  ) {player_th += 360;}

    if (angle_up_down >  max_angle_up_down) {angle_up_down =  max_angle_up_down;}
    if (angle_up_down < -max_angle_up_down) {angle_up_down = -max_angle_up_down;}

    if (player_h > habitat_height - player_head_width) {player_h = habitat_height - player_head_width;}
    if (player_h < 0              + player_head_width) {player_h = 0              + player_head_width;}

    player_x = player_r * (Cos(player_th));
    player_y = player_h;
    player_z = player_r * (Sin(player_th));
}

void orientCamera()
{
    //set variables for the various camera modes
    switch (camera)
    {
        //perspective mode
        case 0:
            {
                double Ex = (double)(-2*world_size*Sin(angle_left_right)*Cos(angle_up_down));
                double Ey = (double)(-2*world_size                      *Sin(angle_up_down));
                double Ez = (double)(+2*world_size*Cos(angle_left_right)*Cos(angle_up_down));

                gluLookAt(Ex,Ey,Ez, 0,0,0, 0,Cos(angle_up_down),0);
                break;
            }
        
        //first person mode
        case 1:
            //angle the look vector up/down
            look_x = -Sin(angle_up_down);
            look_z =  Cos(angle_up_down);
            
            //angle the look vector left/right
            look_y = look_z * Sin(angle_left_right);
            look_z = look_z * Cos(angle_left_right);
            
            //angle the look vector around
            float look_x_old = look_x;
            float look_z_old = look_z;
            look_x = look_x_old * Cos(player_th) - look_z_old * Sin(player_th);
            look_z = look_x_old * Sin(player_th) + look_z_old * Cos(player_th);

            //make the look vector shorter
            look_x *= reach;
            look_y *= reach;
            look_z *= reach;

            //move the look vector to the player
            look_x += player_x;
            look_y += player_y;
            look_z += player_z;

            //calculate up vector
            up_x = -Cos(player_th);
            up_y = 0;
            up_z = -Sin(player_th);

            gluLookAt(player_x,player_y,player_z, look_x,look_y,look_z, up_x,up_y,up_z);
            break;

        //orthographic mode
        default:
            glRotatef(angle_up_down,   -1.0, 0.0, 0.0);
            glRotatef(angle_left_right, 0.0, 1.0, 0.0);
            break;
    }
}

void doLighting()
{
    //LIGHT 0 (Player)
    //translate intensity to color vectors
    float Ambient0[]  = {ambient,  ambient,  ambient,  1.0};
    float Diffuse0[]  = {diffuse,  diffuse,  diffuse,  1.0};
    float Specular0[] = {specular, specular, specular, 1.0};

    //light position
    float Position0[] = {light_distance*Cos(light_rotation), light_y, light_distance*Sin(light_rotation), 1.0};

    //set the light's position to just above the player's head in first person mode
    if (camera) 
    {
        float* cartesian = toCartesian(habitat_radius - (2*player_head_height), player_th, player_h);
        Position0[0] = cartesian[0];
        Position0[1] = cartesian[1];
        Position0[2] = cartesian[2];
    }

    //draw light position as a point in perspective mode
    if (!camera)
    {
        SetColor(1, 1, 1);
        glPointSize(10.0f);
        glBegin(GL_POINTS);
        glVertex3f(Position0[0], Position0[1], Position0[2]);
        glEnd();
    }

    //enable lighting
    glEnable(GL_LIGHTING);

    //location of viewer for specular calculations
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 0);

    //glColor sets ambient and diffuse color materials
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    //set ambient, diffuse, specular components and position of light 0
    glLightfv(GL_LIGHT0, GL_AMBIENT,  Ambient0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  Diffuse0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, Specular0);
    glLightfv(GL_LIGHT0, GL_POSITION, Position0);


    //LIGHT 1 (Sun)
    if (debug_mode) glDisable(GL_LIGHT1);
    else glEnable(GL_LIGHT1);

    //translate intensity to color vectors
    float Ambient1[]  = {0.0,         0.0,         0.0,           1.0};
    float Diffuse1[]  = {diffuse*1.5, diffuse*1.5, diffuse*1.1,   1.0};
    float Specular1[] = {specular,    specular,    specular*0.75, 1.0};

    //light position
    float light_distance = 10*sun_distance;
    float Position1[] = {light_distance*Sin(-orbit_habitat), light_distance*Cos(-orbit_habitat)*Cos(23.5), light_distance*-Cos(-orbit_habitat)*Sin(23.5)};
    
    //glColor sets ambient and diffuse color materials
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    //set ambient, diffuse, specular components and position of light 0
    glLightfv(GL_LIGHT1, GL_AMBIENT , Ambient1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE , Diffuse1);
    glLightfv(GL_LIGHT1, GL_SPECULAR, Specular1);
    glLightfv(GL_LIGHT1, GL_POSITION, Position1);
}

void lineClipping()
{
    int iterations = 10;

    float x_init = player_x;
    float y_init = player_y;
    float z_init = player_z;
    float x_fini = look_x;
    float y_fini = look_y;
    float z_fini = look_z;
    float x_curr = (x_init + x_fini)/2;
    float y_curr = (y_init + y_fini)/2;
    float z_curr = (z_init + z_fini)/2;

    int is_init_inside = (sqrt(pow(x_init, 2) + pow(z_init, 2)) < habitat_radius) ? 1 : 0;
    int is_curr_inside = (sqrt(pow(x_curr, 2) + pow(z_curr, 2)) < habitat_radius) ? 1 : 0;
    int is_fini_inside = (sqrt(pow(x_fini, 2) + pow(z_fini, 2)) < habitat_radius) ? 1 : 0;

    //if there is no clipping, return -5
    if (is_init_inside == is_fini_inside)
    {
        line_clipping_x = -5;
        line_clipping_y = -5;
        line_clipping_z = -5;
    }
    else
    {
        for(int i = 0; i < iterations; i++)
        {
            if (is_init_inside == is_curr_inside)
            {
                x_init = x_curr;
                y_init = y_curr;
                z_init = z_curr;

                x_curr = (x_init + x_fini)/2;
                y_curr = (y_init + y_fini)/2;
                z_curr = (z_init + z_fini)/2;
            }
            else if (is_fini_inside == is_curr_inside)
            {
                x_fini = x_curr;
                y_fini = y_curr;
                z_fini = z_curr;

                x_curr = (x_init + x_fini)/2;
                y_curr = (y_init + y_fini)/2;
                z_curr = (z_init + z_fini)/2;
            }
            is_init_inside = (sqrt(pow(x_init, 2) + pow(z_init, 2)) < habitat_radius) ? 1 : 0;
            is_curr_inside = (sqrt(pow(x_curr, 2) + pow(z_curr, 2)) < habitat_radius) ? 1 : 0;
            is_fini_inside = (sqrt(pow(x_fini, 2) + pow(z_fini, 2)) < habitat_radius) ? 1 : 0;
        }
        
        line_clipping_x = x_curr;
        line_clipping_y = y_curr;
        line_clipping_z = z_curr;

        if (line_clipping_y > habitat_height || line_clipping_y < 0)
        {
            line_clipping_x = -5;
            line_clipping_y = -5;
            line_clipping_z = -5;
        }
    }
}

void display()
{
    //display setup
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    //set position values in both cylindrical coordinates and cartesian coordinates
    setPosition();

    //set and rotate the perspective of the scene by calculating the look vector and the up vector
    orientCamera();

    //find out what part of the habitat the player is looking at
    lineClipping();
    float* pos = toCylindrical(line_clipping_x, line_clipping_y, line_clipping_z);

    //display a non-lit wireframe where the player is looking at, clamped to within the habitat
    float diag = sqrt(1 + fabsf(Sinf(angle_left_right*2)));
    if (pos[2] <                  house_radius*diag) {pos[2] =                  house_radius*diag;}
    if (pos[2] > habitat_height - house_radius*diag) {pos[2] = habitat_height - house_radius*diag;}
    if (line_clipping_x != -5) {houseWireframe(house_radius, pos[0], pos[1], pos[2], -angle_left_right);}

    //draw lighting
    if (lighting)  doLighting();
    else glDisable(GL_LIGHTING);

    //draw scene
    glUseProgram(shader[1]);
    displayScene();
    glUseProgram(0);

    //draw axes
    glDisable(GL_LIGHTING);
    if (axes_on) {axes(look_x, look_y, look_z, up_x, up_y, up_z);}

    //draw text on HUD
    if (hud_text_on) {hudText();}

    //draw crosshair
    hud();

    //display cleanup
    ErrCheck("display");
    glFlush();
    glutSwapBuffers();
}

void keyboardFunctionality()
{
    //esc menu
    if (keys[27])
    {
        keys[27] = 0;
        if (esc_menu)
        {
            esc_menu = 0;
            glutSetCursor(GLUT_CURSOR_NONE);

            //reset cursor to the center of the screen, but ignore that movement
            ignore_next_look = 1;
            glutWarpPointer(window_width/2, window_height/2);
        }
        else
        {
            esc_menu = 1;
            glutSetCursor(GLUT_CURSOR_INHERIT);
            glutPostRedisplay();
        }
    }
    if (esc_menu) {return;}

    //adjust camera angle
    if (special_keys[GLUT_KEY_RIGHT])
    {
        angle_left_right -= 6*mouse_sensitivity_x;
        if (angle_left_right < 0) {angle_left_right += 360;}
    }
    if (special_keys[GLUT_KEY_LEFT])
    {
        angle_left_right += 6*mouse_sensitivity_x;
        if (angle_left_right >= 360) {angle_left_right -= 360;}
    }
    if (special_keys[GLUT_KEY_UP])
    {
        angle_up_down += 6*mouse_sensitivity_y;
        if (angle_up_down > max_angle_up_down) {angle_up_down = max_angle_up_down;}
    }
    if (special_keys[GLUT_KEY_DOWN])
    {
        angle_up_down -= 6*mouse_sensitivity_y;
        if (angle_up_down < -max_angle_up_down) {angle_up_down = -max_angle_up_down;}
    }

    //adjust world size
    if (special_keys[GLUT_KEY_PAGE_UP])
    {
        world_size *= 0.97;
        if (world_size < 1.0)   {world_size = 1.0;}
    }
    if (special_keys[GLUT_KEY_PAGE_DOWN])
    {
        world_size *= 1.03;
        if (world_size > 22.0) {world_size = 22.0;}
    }
  
    //toggle fullscreen
    if (special_keys[GLUT_KEY_F11])
    {
        special_keys[GLUT_KEY_F11] = 0;
        fullscreen_press_timestamp = glutGet(GLUT_ELAPSED_TIME)/1000.0;

        fullscreen = 1 - fullscreen;
        if (fullscreen == 1)
        {
            window_width_non_fullscreen  = window_width;
            window_height_non_fullscreen = window_height;
            glutFullScreen();
        }
        else
        {
            glutReshapeWindow(window_width_non_fullscreen, window_height_non_fullscreen);
        }
    }

    //camera mode
    if (keys['m'])
    {
        keys['m'] = 0;
        camera = (camera + 1) % 2;
    }
     
    //first person player position
    if (keys['w'])
    {
        float diag = (keys['a'] ^ keys['d']) ? 1/sqrt(2) : 1;
        player_x -= diag * player_speed * Cos(angle_left_right) * Sin(player_th);
        player_z += diag * player_speed * Cos(angle_left_right) * Cos(player_th);
        player_y += diag * player_speed * Sin(angle_left_right);

        float* position = toCylindrical(player_x, player_y, player_z);
        player_r  = habitat_radius - player_head_height;
        player_th = position[1];
        player_h  = position[2];
    }
    if (keys['s'])
    {
        float diag = (keys['a'] ^ keys['d']) ? 1/sqrt(2) : 1;
        player_x += diag * player_speed * Cos(angle_left_right) * Sin(player_th);
        player_z -= diag * player_speed * Cos(angle_left_right) * Cos(player_th);
        player_y -= diag * player_speed * Sin(angle_left_right);

        float* position = toCylindrical(player_x, player_y, player_z);
        player_r  = habitat_radius - player_head_height;
        player_th = position[1];
        player_h  = position[2];
    }
    if (keys['a'])
    {
        float diag = (keys['w'] ^ keys['s']) ? 1/sqrt(2) : 1;
        player_x += diag * player_speed * Sin(angle_left_right) * Sin(player_th);
        player_z -= diag * player_speed * Sin(angle_left_right) * Cos(player_th);
        player_y += diag * player_speed * Cos(angle_left_right);

        float* position = toCylindrical(player_x, player_y, player_z);
        player_r  = habitat_radius - player_head_height;
        player_th = position[1];
        player_h  = position[2];
    }
    if (keys['d'])
    {
        float diag = (keys['w'] ^ keys['s']) ? 1/sqrt(2) : 1;
        player_x -= diag * player_speed * Sin(angle_left_right) * Sin(player_th);
        player_z += diag * player_speed * Sin(angle_left_right) * Cos(player_th);
        player_y -= diag * player_speed * Cos(angle_left_right);

        float* position = toCylindrical(player_x, player_y, player_z);
        player_r  = habitat_radius - player_head_height;
        player_th = position[1];
        player_h  = position[2];
    }

    //debug mode
    if (keys['r'])
    {
        keys['r'] = 0;
        debug_mode = (debug_mode + 1) % (unique_objects + 1);
    }

    //toggle axes
    if (keys['q'])
    {
        keys['q'] = 0;
        axes_on = 1 - axes_on;
    }
    //toggle hud text
    if (keys['e'])
    {
        keys['e'] = 0;
        hud_text_on = 1 - hud_text_on;
    }

    //toggle texturing
    if (keys['t'])
    {
        keys['t'] = 0;
        texturing = 1 - texturing;
    }

    //toggle lighting
    if (keys['l'])
    {
        keys['l'] = 0;
        lighting = 1 - lighting;
    }
        
    //light movement
    if (keys['k'])
    {
        keys['k'] = 0;
        light_movement = 1 - light_movement;
    }
   
    //light distance
    if (keys['j'])
    {
        light_distance -= 0.01;
        if (light_distance < 0) {light_distance = 0;}
    }
    if (keys['J'])
    {
        light_distance += 0.01;
        if (light_distance > 3) {light_distance = 3;}
    }
    
    //light height
    if (keys['h'])
    {
        light_y -= 0.015;
        if (light_y < -3) {light_y = -3;}
    }
    if (keys['H'])
    {
        light_y += 0.015;
        if (light_y > 3) {light_y = 3;}
    }

    //side count of prisms/cones
    if (keys['c'])
    {
        keys['c'] = 0;
        debug_sides -= 1;
    }
    if (keys['C'])
    {
        keys['C'] = 0;
        debug_sides += 1;
    }

    //fov
    if (keys['-'] || keys['_'])
    {
        fov--;
        if (fov <= 1) {fov = 1;}
    }
    if (keys['+'] || keys['='])
    {
        fov++;
        if (fov >= 179) {fov = 179;}
    }

    //generic switch
    if (keys['1'])
    {
        keys['1'] = 0;
        switch_1 = 1 - switch_1;
    }

    //specular level
    if (keys['z'])
    {
        specular -= 0.005;
        if (specular < 0.0) {specular = 0.0;}
    }
    if (keys['Z'])
    {
        specular += 0.005;
        if (specular > 1.0) {specular = 1.0;}
    }

    //shininess level
    if (keys['x'])
    {
        shiny = (shiny > 1) ? shiny*0.97 : shiny-0.02;
        if (shiny < 0.0)   {shiny = 0.0;}
    }
    if (keys['X'])
    {
        shiny = (shiny > 1) ? shiny*1.03 : shiny+0.02;
        if (shiny > 128.0) {shiny = 128.0;}
    }

    Projection();
    glutPostRedisplay();
}

void special(int key, int x, int y)
{
    //I don't care about mouse_x and mouse_y
    (void)x;
    (void)y;

    special_keys[key] = 1;

    Projection();
    glutPostRedisplay();
}

void specialUp(int key, int x, int y)
{
    //I don't care about mouse_x and mouse_y
    (void)x;
    (void)y;

    special_keys[key] = 0;

    Projection();
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
    //I don't care about mouse_x and mouse_y
    (void)x;
    (void)y;

    keys[key] = 1;

    Projection();
    glutPostRedisplay();
}

void keyboardUp(unsigned char key, int x, int y)
{
    //I don't care about mouse_x and mouse_y
    (void)x;
    (void)y;

    keys[key] = 0;

    //if a lowercase letter is unpressed, unpress the capital variant too, and vice versa
    if (key >= 'a' && key <= 'z') {keys[key-32] = 0;}
    if (key >= 'A' && key <= 'Z') {keys[key+32] = 0;}

    //same for the equals/plus key
    if (key == '=') {keys['+'] = 0;}
    if (key == '+') {keys['='] = 0;}

    //same for the minus/underscore key
    if (key == '-') {keys['_'] = 0;}
    if (key == '_') {keys['-'] = 0;}

    Projection();
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
    //I don't care about mouse_x and mouse_y
    (void)x;
    (void)y;
    
    if (esc_menu) {return;}

    //place house
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) 
    {
        if (camera)
        {
            if (house_count < houses_size)
            {
                float* pos = toCylindrical(line_clipping_x, line_clipping_y, line_clipping_z);
                float diag = sqrt(1 + fabsf(Sinf(angle_left_right*2)));
                if (pos[2] <                  house_radius*diag) {pos[2] =                  house_radius*diag;}
                if (pos[2] > habitat_height - house_radius*diag) {pos[2] = habitat_height - house_radius*diag;}
                if (line_clipping_x != -5) 
                {
                    int red =   (rand() % 600) + 200;
                    int green = (rand() % 600) + 200;
                    int blue =  (rand() % 600) + 200;

                    houses[house_count][0] = red / 1000.0; 
                    houses[house_count][1] = green / 1000.0; 
                    houses[house_count][2] = blue / 1000.0;
                    houses[house_count][3] = house_radius;
                    houses[house_count][4] = pos[0];
                    houses[house_count][5] = pos[1];
                    houses[house_count][6] = pos[2]; 
                    houses[house_count][7] = -angle_left_right;
                }
                house_count++;
            }
        }
    }
}

void look(int x, int y)
{
    if (esc_menu) {return;}

    //reset cursor to the center of the screen, but ignore that movement
    if (ignore_next_look) {ignore_next_look = 0; return;}
    ignore_next_look = 1;
    glutWarpPointer(window_width/2, window_height/2);

    //move camera only if the mouse movement was at a reasonable speed and if the fullscreen button wasn't just pressed
    float dx = window_width /2 - x;
    float dy = window_height/2 - y;
    if (fabsf(dx) < window_width/3 && fabsf(dy) < window_height/3)
    {
        if (glutGet(GLUT_ELAPSED_TIME)/1000.0 > fullscreen_press_timestamp + fullscreen_immunity)
        {
            angle_left_right += dx * mouse_sensitivity_x;
            angle_up_down    += dy * mouse_sensitivity_y;

            if (angle_left_right <  0  ) {angle_left_right += 360;}
            if (angle_left_right >= 360) {angle_left_right -= 360;}
        }
    }
}

void reshape(int width, int height)
{
    aspect_ratio = (height>0) ? (double)width/height : 1;
    glViewport(0,0, RES*width,RES*height);
    window_width  = width;
    window_height = height;
    Projection();
}

void idle(void)
{
    //get time since this function was last called
    double old_time = time;
    time = glutGet(GLUT_ELAPSED_TIME)/1000.0;
    double delta_time = time - old_time;

    //call keyboardFunctionality once every 60th of a second
    if (time >= last_keyboard_functionality + 1.0/60)
    {
        last_keyboard_functionality = time;
        keyboardFunctionality();
    }

    //ensure window starts at the top left
    if (time < 1.5) {glutReshapeWindow(window_width, window_height);}

    //don't do anything else if the pause menu is open
    if (esc_menu) {return;}

    //increment values each timestep for orbit_habitat, yaw, and roll
    orbit_habitat -= game_speed*delta_time;
    orbit_earth   += game_speed*delta_time/6; //realistically the earth's rotation around its axis would be orbit_habitat/16 in low earth orbit
    yaw   += 30*delta_time;
    roll  += 60*delta_time;
    
    //move the light around
    if (light_movement) {light_rotation += 90*delta_time;}

    glutPostRedisplay();
}

char* ReadText(char *file)
{
   char* buffer;
   //  Open file
   FILE* f = fopen(file, "rt");
   if (!f) Fatal("Cannot open text file %s\n", file);
   //  Seek to end to determine size, then rewind
   fseek(f, 0, SEEK_END);
   int n = ftell(f);
   rewind(f);
   //  Allocate memory for the whole file
   buffer = (char*)malloc(n+1);
   if (!buffer) Fatal("Cannot allocate %d bytes for text file %s\n", n+1, file);
   //  Snarf the file
   if (fread(buffer, n, 1, f)!=1) Fatal("Cannot read %d bytes for text file %s\n", n, file);
   buffer[n] = 0;
   //  Close and return
   fclose(f);
   return buffer;
}

void PrintShaderLog(int shader, char* file)
{
   int len=0;
   glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
   if (len>1)
   {
      int n=0;
      char* buffer = (char *)malloc(len);
      if (!buffer) Fatal("Cannot allocate %d bytes of text for shader log\n", len);
      glGetShaderInfoLog(shader, len, &n, buffer);
      fprintf(stderr, "%s:\n%s\n", file, buffer);
      free(buffer);
   }
   glGetShaderiv(shader, GL_COMPILE_STATUS, &len);
   if (!len) Fatal("Error compiling %s\n", file);
}

void PrintProgramLog(int prog)
{
   int len=0;
   glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
   if (len>1)
   {
      int n=0;
      char* buffer = (char *)malloc(len);
      if (!buffer) Fatal("Cannot allocate %d bytes of text for program log\n", len);
      glGetProgramInfoLog(prog, len, &n, buffer);
      fprintf(stderr, "%s\n", buffer);
   }
   glGetProgramiv(prog, GL_LINK_STATUS, &len);
   if (!len) Fatal("Error linking program\n");
}

int CreateShader(GLenum type, char* file)
{
   //  Create the shader
   int shader = glCreateShader(type);
   //  Load source code from file
   char* source = ReadText(file);
   glShaderSource(shader, 1, (const char**)&source, NULL);
   free(source);
   //  Compile the shader
   fprintf(stderr, "Compile %s\n", file);
   glCompileShader(shader);
   //  Check for errors
   PrintShaderLog(shader, file);
   //  Return name
   return shader;
}

int CreateShaderProg(char* VertFile, char* FragFile)
{
    //  Create program
    int prog = glCreateProgram();
    //  Create and compile vertex shader
    int vert = CreateShader(GL_VERTEX_SHADER, VertFile);
    //  Create and compile fragment shader
    int frag = CreateShader(GL_FRAGMENT_SHADER, FragFile);
    //  Attach vertex shader
    glAttachShader(prog, vert);
    //  Attach fragment shader
    glAttachShader(prog, frag);
    //  Link program
    glLinkProgram(prog);
    //  Check for errors
    PrintProgramLog(prog);

    //  Return name
    return prog;
}

int main(int argc, char *argv[])
{
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_ALPHA);
    glutInitWindowSize(window_width, window_height);
    glutCreateWindow("Zachary McGuire");
    glutFullScreen();

    #ifdef USEGLEW
        if (glewInit()!=GLEW_OK) Fatal("Error initializing GLEW\n");
    #endif

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(special);
    glutSpecialUpFunc(specialUp);
    glutMouseFunc(mouse);
    glutPassiveMotionFunc(look);
    glutMotionFunc(look);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glutSetCursor(GLUT_CURSOR_NONE);
    
    textures[0] = LoadTexBMP("textures/wall.bmp");
    textures[1] = LoadTexBMP("textures/bricks.bmp");
    textures[2] = LoadTexBMP("textures/grass.bmp");
    textures[3] = LoadTexBMP("textures/door.bmp");
    textures[4] = LoadTexBMP("textures/world.bmp");
    textures[5] = LoadTexBMP("textures/sun.bmp");
    textures[6] = LoadTexBMP("textures/stars.bmp");
    textures[7] = LoadTexBMP("textures/gravel.bmp");
    textures[8] = LoadTexBMP("textures/bark.bmp");

    shader[1] = CreateShaderProg("pixtex.vert", "pixtex.frag");
    shader[2] = CreateShaderProg("smog.vert", "smog.frag");

    for(int i = 0; i < habitat_sides; i++)
    {
        grass_random_x[i] = rand() % texture_portions;
        grass_random_y[i] = rand() % texture_portions;
    }

glEnable(GL_CULL_FACE);        // Turn on culling
glCullFace(GL_BACK);           // Cull back faces (default)
glFrontFace(GL_CCW);   

    ErrCheck("init");
    glutMainLoop();

    return 0;
}