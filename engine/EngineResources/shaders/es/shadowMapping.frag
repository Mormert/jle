#version 300 es

precision highp float;

out vec4 FragColor;

in vec4 glPos;

void main()
{
    FragColor = vec4(1.0);
    // Not doing anything in the frag shader is the equivalent to:
    // gl_FragDepth =  gl_FragCoord.z;
}