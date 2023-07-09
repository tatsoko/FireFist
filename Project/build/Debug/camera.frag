#version 330 core
in vec2 fragTexCoord;
out vec4 fragColor;
uniform sampler2D textureSampler;
//fragment
void main()
{
    fragColor = texture(textureSampler, fragTexCoord) * 0.5;
}
