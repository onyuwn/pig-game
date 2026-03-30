#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in float shellHeight;
in vec3 fragPos;
uniform float shellCount;
uniform float shellLength;
uniform int shellIndex;
uniform float shellAttenuation;
uniform float density;
uniform float noiseMin;
uniform float noiseMax; 
uniform sampler2D texture_diffuse1;
uniform vec3 shellColor;

uniform vec3 lightPos;  
uniform vec3 lightColor;

float hash(vec2 x) {
    // hash by Inigo Quilez, Integer Hash - III, 2017
    uvec2 q = uvec2(x * 8192.0);
    q = 1103515245u * ((q >> 1u) ^ q.yx);
    uint n = 1103515245u * (q.x ^ (q.y >> 3u));
    return float(n) * (1.0 / float(0xffffffffu));
}

void main() {
    vec3 ambient = 1.0 * lightColor;
    vec4 leafColor = texture(texture_diffuse1, TexCoords);
    leafColor += vec4(0.0, .25, 0.0, 1.0);
    if(shellIndex <= shellCount / 2.0) {
        FragColor = vec4(shellColor * leafColor.xyz, 1.0);
    } else {
        float d = distance(vec2(fragPos.x, fragPos.z), vec2(lightPos.x, lightPos.z));
        // if(d > 75.0) {
        //     return;
        // }
        vec3 norm = vec3(0.0, 1.0, 0.0);
        vec3 lightDir = normalize(lightPos - fragPos); 
        float diff = max(dot(norm, lightDir), 0.0);
        vec2 newUv = TexCoords * density;
        vec2 _uv = fract(TexCoords * density) - 0.5;
        float rand = mix(noiseMin, noiseMax, hash(floor(TexCoords * density)));
        if (noiseMin != 0.0 && noiseMax != 0.0 && rand < .75 && shellIndex > 0) discard;
        // FragColor = vec4(
        //     vec3(leafColor.xyz * (1.0 - (1.0 / shellHeight) + .5) * shellColor),
        //     leafColor.a
        // );
        FragColor = vec4(
            vec3(((1.5 - (pow(shellHeight - .25, shellAttenuation))) * shellColor * leafColor.xyz)),
            leafColor.a
        );
    }
}