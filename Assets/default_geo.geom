#version 450 core
layout(triangles) in;
layout(line_strip, max_vertices = 6) out;

//uniform mat4 uProjViewMatrix;
//uniform vec3 uWireframeColor;

//out vec4 gsFragColor;

void main() {
    // Emit triangle edges as lines (wireframe)
    for (int i = 0; i < 3; ++i)
    {
        //gsFragColor = vec4(uWireframeColor, 1.0);
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();

        //gsFragColor = vec4(uWireframeColor, 1.0);
        gl_Position = gl_in[(i + 1) % 3].gl_Position;
        EmitVertex();

        EndPrimitive();
    }
}
