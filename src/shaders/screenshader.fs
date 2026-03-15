#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float time;
uniform vec2 window;

vec3 texsample(const float x, const float y, vec2 fragCoord)
{
    vec2 uv = fragCoord.xy;
	uv = (uv + (uv * vec2(x, y)));
	return texture(screenTexture, uv).xyz;
}

vec3 texfilter(vec2 fragCoord, float offsetStep)
{
    float offset = 1.0 / 500.0; // yuh
    vec3 sum = texsample(-offset, -offset, fragCoord) * 0.
             + texsample(-offset,  0, fragCoord) * -1.
             + texsample(-offset,  offset, fragCoord) * 0.
             + texsample( 0, -offset, fragCoord) * -1.
             + texsample( 0,  0, fragCoord) * 5
             + texsample( 0,  offset, fragCoord) * -1.
             + texsample( offset, -offset, fragCoord) * 0
             + texsample( offset,  0, fragCoord) * -1.
             + texsample( offset,  offset, fragCoord) * 0;
    
	return sum;
}

float rand(vec2 st) {
    return fract(sin(dot(st, vec2(36.164927, 44.1028472))) * 64637.7890);
}

void main()
{ 
    vec2 st = gl_FragCoord.xy / vec2(window.x,window.y);
    st *= 1.0;
    float val = rand(st);

    vec3 cf = texfilter(TexCoords, abs(time));
    FragColor = vec4(cf, 1.0);
}