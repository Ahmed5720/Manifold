#version 410 core

in  vec3 vNormal;
in  vec3 vWorldPos;
out vec4 fragColour;

//uniform vec3 uCameraPos;


const vec3  LIGHT_DIR  = normalize(vec3(0.4, 1.0, 0.6));
const vec3  BASE_COLOUR = vec3(0.55, 0.62, 0.72); 
const float AMBIENT    = 0.25;

void main() {
    vec3  n        = normalize(vNormal);
    float diffuse  = max(dot(n, LIGHT_DIR), 0.0);
    float light    = AMBIENT + (1.0 - AMBIENT) * diffuse;
    fragColour = vec4(BASE_COLOUR * light, 1.0);
}
