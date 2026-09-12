#version 410 core

in  vec3 vNormal;
in  vec3 vWorldPos;
out vec4 fragColour;

//uniform vec3 uCameraPos;

uniform int shaded; 
uniform vec3  BASE_COLOUR = vec3(0.68f, 0.68f, 0.88f); 
const vec3  LIGHT_DIR  = normalize(vec3(0.4, 1.0, 0.6));
const float AMBIENT    = 0.25;

void main() {
    vec3  n        = normalize(vNormal);
    float diffuse  = max(dot(n, LIGHT_DIR), 0.0);
    float light    = AMBIENT + (1.0 - AMBIENT) * diffuse;
    vec3 c = shaded == 1 ? light * BASE_COLOUR : BASE_COLOUR; 
    fragColour = vec4(c, 1.0);
}
