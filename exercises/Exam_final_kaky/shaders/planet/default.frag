#version 330 core  

uniform uint Mode = 4u;  

in vec2 texCoord;  
in vec3 color;  
in vec3 normal;  
in vec3 worldPos;

out vec4 FragColor;  

void main()  
{  
    switch (Mode)  
    {  
    default:  
    case 0u:  
        FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);  
        break;  
    case 1u:  
        FragColor = vec4(fract(texCoord), 0.0f, 1.0f);  
        break;  
    case 2u:  
        FragColor = vec4(color, 1.0f);  
        break;  
    case 3u:  
        FragColor = vec4(normalize(normal), 1.0f);  
        break;  
    case 4u:  
        FragColor = vec4(color * max(dot(normalize(normal), normalize(worldPos)), 0.01f), 1.0f);  
        break;  
    }  
}