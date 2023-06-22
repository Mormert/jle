
#define BIAS 0.1

void main()
{
    // Not doing anything in this frag shader is the equivalent to:
    // gl_FragDepth =  gl_FragCoord.z;
    // Note that using the above mentioned method, shadow acne will appear,
    // and needs to be accounted for in the mesh rendering shader.
    // Another way is to account for peter panning in this shadow mapping shader:

    gl_FragDepth = gl_FragCoord.z;
    gl_FragDepth += gl_FrontFacing ? BIAS : 0.0; // Mitigate shadow acne

    // The bias could also be calculated depending on surface normal and light direction
    // float bias = max(0.05 * (1.0 - dot(N, L)), 0.005);
}