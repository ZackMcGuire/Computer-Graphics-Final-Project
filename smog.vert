#version 120

attribute vec3 pos;
attribute float life;
attribute float size;

void main() 
{
    gl_Position = gl_ModelViewProjectionMatrix * vec4(pos, 1.0);
    gl_PointSize = size*life;
}