#version 150
// ^ Change this to version 130 if you have compatibility issues

//This is a vertex shader. While it is called a "shader" due to outdated conventions, this file
//is used to apply matrix transformations to the arrays of vertex data passed to it.
//Since this code is run on your GPU, each vertex is transformed simultaneously.
//If it were run on your CPU, each vertex would have to be processed in a FOR loop, one at a time.
//This simultaneous transformation allows your program to run much faster, especially when rendering
//geometry with millions of vertices.

uniform mat4 u_Model;       // The matrix that defines the transformation of the
                            // object we're rendering. In this assignment,
                            // this will be the result of traversing your scene graph.

uniform mat4 u_ModelInvTr;  // The inverse transpose of the model matrix.
                            // This allows us to transform the object's normals properly
                            // if the object has been non-uniformly scaled.

uniform mat4 u_View;

uniform mat4 u_ViewProj;    // The matrix that defines the camera's transformation.
                            // We've written a static matrix for you to use for HW2,
                            // but in HW3 you'll have to generate one yourself
uniform int u_Time;

uniform vec4 u_Color;       // When drawing the cube instance, we'll set our uniform color to represent different block types.

//uniform sampler2D textureSampler;

in vec4 vs_Pos;             // The array of vertex positions passed to the shader

in vec4 vs_Nor;             // The array of vertex normals passed to the shader

in vec4 vs_Col;             // The array of vertex colors passed to the shader.
in vec4 vs_UV;

out vec4 fs_Pos;
out vec4 fs_Nor;            // The array of normals that has been transformed by u_ModelInvTr. This is implicitly passed to the fragment shader.
out vec4 fs_LightVec;       // The direction in which our virtual light lies, relative to each vertex. This is implicitly passed to the fragment shader.
out vec4 fs_Col;
out vec4 fs_UV;
out float visibility;
out mat4 viewProjInverse;

// The color of each vertex. This is implicitly passed to the fragment shader.

const vec4 lightDir = normalize(vec4(0.5, 1, 0.75, 0));  // The direction of our virtual light, which is used to compute the shading of
                                        // the geometry in the fragment shader.

const float density = 0.007;
const float gradient = 5.0;

void main()
{
    viewProjInverse = inverse(u_ViewProj);
//    vec4 col = textureSampler(vs_UV, vs_Col);
    fs_Pos = vs_Pos;
    fs_Col = vs_Col;
    fs_UV = vs_UV;
    // Pass the vertex colors to the fragment shader for interpolation

    mat3 invTranspose = mat3(u_ModelInvTr);
    fs_Nor = vec4(invTranspose * vec3(vs_Nor), 0);          // Pass the vertex normals to the fragment shader for interpolation.
                                                            // Transform the geometry's normals by the inverse transpose of the
                                                            // model matrix. This is necessary to ensure the normals remain
                                                            // perpendicular to the surface after the surface is transformed by
                                                            // the model matrix.


    vec4 modelposition = u_Model * vs_Pos;   // Temporarily store the transformed vertex positions for use below
    vec4 modelRelativeToCam = u_View * modelposition;
    float distance = length(modelRelativeToCam.xyz);
    visibility = exp(-pow((distance*density), gradient));
    visibility = clamp(visibility, 0.0, 1.0);


    fs_LightVec = (lightDir);  // Compute the direction in which the light source lies

    if (vs_Col.y > 5.0) {
        gl_Position = u_ViewProj * modelposition;
        float offSet = 0;
        if (vs_Col.z > 0.5) {
            offSet = sin(u_Time / 5) * 1.2;
        } else {
            offSet = cos(u_Time / 5) * 1.2;
        }

        gl_Position.y = gl_Position.y + offSet;
    } else {
        gl_Position = u_ViewProj * modelposition;// gl_Position is a built-in variable of OpenGL which is
    }

                                             // used to render the final positions of the geometry's vertices
}