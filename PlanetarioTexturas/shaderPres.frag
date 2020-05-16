#version 330 core
in vec2 TexCoord;
in vec3 Normal;  
in vec3 FragPos;

out vec4 FragColor; 

uniform vec3 viewPos;
uniform vec3 lightPos; 
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform sampler2D tex;

void main()
{
    // ambient
    float ambientI = 0.3;
    vec3 ambient = ambientI * lightColor;

    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.4);
    vec3 diffuse = diff * lightColor;

	//Speculas
	float specularStrength = 1.0;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 180);
	vec3 specular = specularStrength * spec * lightColor;
            
   	vec3 result = (ambient + diffuse + specular) * vec3(1.0f,1.0f,1.0f);
	//vec3 result = (ambient + diffuse + specular) * texture(tex, TexCoord);

	//FragColor = texture(tex, TexCoord)* vec4(result, 1.0);
	FragColor = vec4(result, 1.0) * texture(tex, TexCoord);
} 
  	

