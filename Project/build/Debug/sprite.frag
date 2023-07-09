#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;
uniform vec3 spriteColor;
//fragment shader
void main()
{

    vec4 tcolor =  texture(image, TexCoords);
    if(tcolor.a < 0.1)
        discard;
    color = vec4(spriteColor, 1.0) * tcolor;
}
