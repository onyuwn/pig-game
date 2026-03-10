#version 330
in vec2 texCoords;
uniform float progress;
uniform sampler2D uiTex;
uniform vec2 screenSize;
out vec4 color;

float rand(vec2 st) {
    return fract(sin(dot(st, vec2(36.164927, 44.1028472))) * 64637.7890);
}

void main() {
    vec4 pinkColor = vec4(1, 0, .816, 1.0);
    vec4 diffuse = texture(uiTex, texCoords);
    vec2 st = gl_FragCoord.xy / screenSize;
    vec4 noiseColor = vec4(vec3(rand(floor(st))), 1.0) + vec4(.25,.25,.25,0);
    st *= 10.0 * (progress * 10);
    noiseColor = vec4(rand(floor(st)));
    if(texCoords.y < progress) {
        color = diffuse;
    } else { // todo: noise
        color = pinkColor * vec4(noiseColor.xyz, 1.0);
    }

    color *= vec4(1.0 - pow(distance(texCoords, vec2(0.5)), 3));

    float borderThicknessX = 15.0 / screenSize.x;  
    float borderThicknessY = 15.0 / screenSize.y;  

    if(texCoords.x < borderThicknessX || texCoords.x > 1.0 - borderThicknessX || texCoords.y < borderThicknessY || texCoords.y > 1.0 - borderThicknessY) {
        if(texCoords.y > progress) {
            color = pinkColor + (noiseColor);
        } else {
            color = pinkColor;
        }
    }
}