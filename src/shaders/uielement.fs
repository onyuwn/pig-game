#version 330

out vec4 fragColor;
uniform vec4 inColor;
in vec2 texCoords;
uniform sampler2D uiTex;
uniform float dHoverTime;
uniform float animTime;

void main() {
    if(inColor.a > 0) {
        vec4 color = inColor;
        float colorMult = 1.0 - pow(distance(texCoords, vec2(texCoords.x, 1.0)), 4.0);
        color *= vec4(colorMult, colorMult, colorMult, 1.0);

        float hoverColor = (dHoverTime/animTime);

        if(hoverColor < 1.0) {
            color += vec4(hoverColor * .3,hoverColor * .3,hoverColor * .3,0.0);
        } else {
            color += vec4(1 * .3,1 * .3,1 * .3,0.0);
        }

        if(dHoverTime > 0) {
            float borderThicknessX = 4.0 / 200;  
            float borderThicknessY = 4.0 / 50;  

            if(texCoords.x < borderThicknessX || texCoords.x > 1.0 - borderThicknessX) {
                float c = sin(texCoords.x * 50+ texCoords.y * 50 + dHoverTime*5);
                color = vec4(0.0, 1.0 - c, 0.0, 1.0 - c);
                //color = vec4(0.0,1.0,0.0,cos(5 * (dHoverTime + texCoords.x)));
            }

            if(texCoords.y < borderThicknessY || texCoords.y > 1.0 - borderThicknessY) {
                float c = sin(texCoords.x * 50 + texCoords.y * 50 + dHoverTime*5);
                color = vec4(0.0, 1.0 - c, 0.0, 1.0 - c);
            }   
        }

        fragColor = color;
    } else {
        vec4 diffuse = texture(uiTex, texCoords);
        fragColor = diffuse;
    }
}