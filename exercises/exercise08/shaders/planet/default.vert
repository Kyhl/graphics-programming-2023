#version 330 core  

layout (location = 0) in vec3 aPos;  
layout (location = 1) in vec2 aTexCoord;  
layout (location = 2) in vec3 aColor;  
layout (location = 3) in vec3 aNormal;  

uniform mat4 WorldMatrix = mat4(1);  
uniform mat4 ViewProjMatrix;
out vec2 texCoord;  
out vec3 color;  
out vec3 normal;  
out vec3 worldPos;

void main()  
{  
	worldPos = (WorldMatrix * vec4(aPos, 1.0)).xyz;
	texCoord = aTexCoord;  
	color = aColor;  
	normal = (WorldMatrix * vec4(aNormal, 1.0)).xyz;  
	gl_Position = ViewProjMatrix * vec4(worldPos, 1.0);

}