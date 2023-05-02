#version 330 core  

layout (location = 0) in vec3 aPos;  
layout (location = 1) in vec2 aTexCoord;  
layout (location = 2) in vec3 aColor;  
layout (location = 3) in vec3 aNormal;  

uniform mat4 Matrix = mat4(1);  

out vec2 texCoord;  
out vec3 color;  
out vec3 normal;  

void main()  
{  
	texCoord = aTexCoord;  
	color = aColor;  
	normal = aNormal;  
	gl_Position = Matrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);  
}