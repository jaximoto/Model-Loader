#version 330 core
out vec4 FragColor;
  
in vec3 ourColor;
in vec2 TexCoord;

uniform struct{
	vec3 ambient, diffuse, specular, emissive;
	float shininess;
	float opacity;
	bool hasDiffuseTex;
} material;
uniform sampler2D texture_diffuse1;


void main()
{
	vec3 diffuseColor = material.hasDiffuseTex 
	? texture(texture_diffuse1, TexCoord).rgb 
	: material.diffuse;

	FragColor = vec4(diffuseColor, material.opacity);
}
