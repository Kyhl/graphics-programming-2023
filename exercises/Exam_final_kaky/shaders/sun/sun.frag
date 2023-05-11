#version 330 core

in vec3 WorldNormal;
in vec2 TexCoord;

out vec4 FragColor;
uniform uint Mode = 4u;  
uniform vec4 Color;
uniform sampler2D ColorTexture;
uniform vec2 ColorTextureScale;

void main()
{
    switch (Mode)  
    {  
    default:  
    case 0u:  
        FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);  
        break;  
    case 1u:  
        FragColor = vec4(fract(TexCoord), 0.0f, 1.0f);  
        break;  
    case 2u:  
        FragColor = Color * texture(ColorTexture, TexCoord * ColorTextureScale); 
        break;  
    case 3u:  
        FragColor = vec4(normalize(WorldNormal), 1.0f);  
        break;  
    case 4u:  
        FragColor = Color * texture(ColorTexture, TexCoord * ColorTextureScale);
        break;  
    }  
}
