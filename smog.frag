#version 120

void main() 
{
    vec2 uv = gl_PointCoord * 2.0 - 1.0;
    if (length(uv) > 1.0) discard;
    gl_FragColor = vec4(0.1, 0.1, 0.1, 1.0-length(uv));
}