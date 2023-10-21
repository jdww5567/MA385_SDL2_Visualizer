#version 460 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 0) buffer InputData {
    float data[];
};

layout(std430, binding = 1) buffer OutputData {
    float result[]; // Use an array of float
};

void main() {
    uint idx = gl_GlobalInvocationID.x;
    float x = data[idx * 6];
    float z = data[idx * 6 + 2];
    float y = 
    float r = abs(sin(y / 2.0)) / 1.2;
    float g = abs(sin(y / 2.0 + 3.1415926535 / 3)) / 1.2;
    float b = abs(sin(y / 2.0 + (2 * 3.1415926535) / 3)) / 1.2;

    result[idx * 6] = x;
    result[idx * 6 + 1] = y;
    result[idx * 6 + 2] = z;
    result[idx * 6 + 3] = r;
    result[idx * 6 + 4] = g;
    result[idx * 6 + 5] = b;
}
