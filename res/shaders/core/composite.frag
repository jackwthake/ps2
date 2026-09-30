#version 330 core
in vec2 vUV;
out vec4 FragColor;

uniform sampler2D uScreenTex;
uniform float uColorLevels; // e.g. 16.0

float dither_pattern(vec2 frag_coord) {
  int x = int(mod(frag_coord.x, 4.0));
  int y = int(mod(frag_coord.y, 4.0));
  int index = x + y * 4;
  float bayer[16] = float[](
      0.0, 8.0, 2.0, 10.0,
      12.0, 4.0, 14.0, 6.0,
      3.0, 11.0, 1.0, 9.0,
      15.0, 7.0, 13.0, 5.0
  );
  
  return bayer[index] / 16.0;
}

vec3 quantize(vec3 color, float levels) {
  return floor(color * levels + 0.5) / levels;
}

void main() {
  vec3 color = texture(uScreenTex, vUV).rgb;
  float d = dither_pattern(gl_FragCoord.xy) - 0.5;
  color += d / uColorLevels;
  color = quantize(color, uColorLevels);
  FragColor = vec4(color, 1.0);
}