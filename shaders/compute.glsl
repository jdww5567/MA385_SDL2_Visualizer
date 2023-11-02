#version 460 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 0) buffer InputData {
    float data[];
};

layout(std430, binding = 1) buffer OutputData {
    float result[];
};

void main() {
    uint idx = gl_GlobalInvocationID.x;
    float x = data[idx * 7];
    float y = data[idx * 7 + 2];
    float z = 
    float r = abs(sin(z / 2.0)) / 1.2;
    float g = abs(sin(z / 2.0 + 3.1415926535 / 3)) / 1.2;
    float b = abs(sin(z / 2.0 + (2 * 3.1415926535) / 3)) / 1.2;
    float a = data[idx * 7 + 6];

    
    result[idx * 7] = x;
    result[idx * 7 + 1] = z;
    result[idx * 7 + 2] = y;
    result[idx * 7 + 3] = r;
    result[idx * 7 + 4] = g;
    result[idx * 7 + 5] = b;
    result[idx * 7 + 6] = a;
}
