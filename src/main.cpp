#include <iostream>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include <mine/camera.hpp>
#include <mine/enums.hpp>
#include <mine/pipeline.hpp>
#include <mine/plot.hpp>
#include <mine/gui.hpp>

constexpr int INITIAL_SCREEN_WIDTH = 960;
constexpr int INITIAL_SCREEN_HEIGHT = 720;

constexpr float INITIAL_RADIUS = 13.0f;
constexpr float INITIAL_THETA = 45.0f;
constexpr float INITIAL_PHI = 65.0f;

SDL_Window *g_window{};
SDL_DisplayMode g_display_mode{};

GLuint g_VAO{};
GLuint g_VBO{};
GLuint g_IBO{};

mine::graphics_pipeline g_graphics_pipeline{};
mine::compute_pipeline g_compute_pipeline{};
mine::plot g_plot{};
mine::camera g_camera{};
mine::gui g_gui{};

bool g_running = true;
bool g_size_change = false;
bool g_scene_change = true;
bool g_screen_change = false;

double g_refresh_time{};

void setup() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Error: Failed to initialize SDL video subsytem\nSDL Error: " << SDL_GetError() << std::endl;
        exit(1);
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    g_window = SDL_CreateWindow(
        "MA_385_Project",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        INITIAL_SCREEN_WIDTH,
        INITIAL_SCREEN_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );
    if (!g_window) {
        std::cerr << "Error: Failed to create window\nSDL Error: " << SDL_GetError() << std::endl;
        exit(1);
    }

    SDL_GLContext gl_context = SDL_GL_CreateContext(g_window);
    if (!gl_context) {
        std::cerr << "Error: Failed to create OpenGL context\nSDL Error: " << SDL_GetError() << std::endl;
        exit(1);
    }

    if(!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
        std::cerr << "Error: Failed to initialize glad" << std::endl;
        exit(1);
    }

    std::cout << "Vender: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glViewport(0, 0, INITIAL_SCREEN_WIDTH, INITIAL_SCREEN_HEIGHT);

    g_gui.set_version(g_window, gl_context);

    g_camera.set_screen((float)INITIAL_SCREEN_WIDTH, (float)INITIAL_SCREEN_HEIGHT);
    g_camera.set_data(INITIAL_RADIUS, INITIAL_THETA, INITIAL_PHI);
    g_camera.set_center(mine::INITIAL_BOUNDS[0]);

    g_graphics_pipeline.set_program("./shaders/vertex.glsl", "./shaders/fragment.glsl", "u_view_matrix");

    if (!SDL_GetCurrentDisplayMode(SDL_GetWindowDisplayIndex(g_window), &g_display_mode)) {
        g_refresh_time = 1.0 / g_display_mode.refresh_rate;
    }
}

bool update_function(const std::string& function, int index) {
    if (!g_compute_pipeline.set_program("./shaders/compute.glsl", function, index)) {
        return false;
    }

    GLuint input_buffer;
    glGenBuffers(1, &input_buffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, input_buffer);
    glBufferData(
        GL_SHADER_STORAGE_BUFFER,
        g_plot.functions[index].size() * sizeof(mine::vertex),
        g_plot.functions[index].data(),
        GL_STATIC_READ
    );
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, input_buffer);

    GLuint output_buffer;
    glGenBuffers(1, &output_buffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, output_buffer);
    glBufferData(
        GL_SHADER_STORAGE_BUFFER,
        g_plot.functions[index].size() * sizeof(mine::vertex),
        nullptr,
        GL_DYNAMIC_COPY
    );
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, output_buffer);

    glUseProgram(g_compute_pipeline.get_program());
    glUniform1f(glGetUniformLocation(g_compute_pipeline.get_program(), "i"), index);
    glDispatchCompute(g_plot.functions[index].size(), 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    glUseProgram(0);

    float* results = (float*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);

    for (std::vector<mine::vertex>::size_type j = 0; j < (mine::X_RECTS + 1) * (mine::Z_RECTS + 1); ++j) {
        g_plot.vertices[g_plot.base_vertice_count + index * ((mine::X_RECTS + 1) * (mine::Z_RECTS + 1)) + j].x = results[j * 6];
        g_plot.vertices[g_plot.base_vertice_count + index * ((mine::X_RECTS + 1) * (mine::Z_RECTS + 1)) + j].y = results[j * 6 + 1];
        g_plot.vertices[g_plot.base_vertice_count + index * ((mine::X_RECTS + 1) * (mine::Z_RECTS + 1)) + j].z = results[j * 6 + 2];
        g_plot.vertices[g_plot.base_vertice_count + index * ((mine::X_RECTS + 1) * (mine::Z_RECTS + 1)) + j].r = results[j * 6 + 3];
        g_plot.vertices[g_plot.base_vertice_count + index * ((mine::X_RECTS + 1) * (mine::Z_RECTS + 1)) + j].g = results[j * 6 + 4];
        g_plot.vertices[g_plot.base_vertice_count + index * ((mine::X_RECTS + 1) * (mine::Z_RECTS + 1)) + j].b = results[j * 6 + 5];
    }

    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glDeleteBuffers(1, &input_buffer);
    glDeleteBuffers(1, &output_buffer);

    return true;
}

void vertex_specification() {
    g_plot.set_vertices();

    update_function(mine::INITIAL_FUNCTIONS[0], 0);

    glGenVertexArrays(1, &g_VAO);
    glBindVertexArray(g_VAO);

    glGenBuffers(1, &g_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, g_VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        g_plot.vertices.size() * sizeof(mine::vertex),
        g_plot.vertices.data(),
        GL_STATIC_DRAW
    );

    glGenBuffers(1, &g_IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_IBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        g_plot.indices.size() * sizeof(GLuint),
        g_plot.indices.data(),
        GL_STATIC_DRAW
    );

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(mine::vertex),
        (GLvoid*)0
    );

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(mine::vertex),
        (GLvoid*)(sizeof(GLfloat) * 3)
    );
}

void input() {
    static float mouse_x = 0.0f;
    static float mouse_y = 0.0f;
    static bool left_down = false;
    static float width_speed = 2.0f;
    static float height_speed = 2.0f;
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);
        switch (event.type) {
            case SDL_QUIT:
                g_running = false;
                break;
            case SDL_MOUSEWHEEL:
                if (ImGui::GetIO().WantCaptureMouse) {
                    break;
                }
                g_camera.zoom(event.wheel.y > 0, event.wheel.y < 0);
                g_scene_change = true;
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (ImGui::GetIO().WantCaptureMouse) {
                    break;
                }
                switch (event.button.button) {
                    case SDL_BUTTON_LEFT:
                        mouse_x = event.button.x;
                        mouse_y = event.button.y;
                        left_down = true;
                        break;
                    default:
                        break;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                switch (event.button.button) {
                    case SDL_BUTTON_LEFT:
                        left_down = false;
                        break;
                    default:
                        break;
                }
                break;
            case SDL_MOUSEMOTION:
                if (left_down) {
                    g_camera.update_angles(
                        (event.button.x - mouse_x) / width_speed, 
                        (mouse_y - event.button.y) / height_speed
                    );
                    mouse_x = event.button.x;
                    mouse_y = event.button.y;
                    g_scene_change = true;
                }
                break;
            case SDL_KEYDOWN:
                if (ImGui::GetIO().WantCaptureKeyboard) {
                    break;
                }
                switch (event.key.keysym.sym) {
                    case SDLK_x:
                        g_camera.set_data(INITIAL_RADIUS, INITIAL_THETA, INITIAL_PHI);
                        g_scene_change = true;
                        break;
                    default:
                        break;
                }
                break;
            case SDL_WINDOWEVENT:
                switch (event.window.event) {
                    case SDL_WINDOWEVENT_RESIZED:
                        g_camera.set_screen(event.window.data1, event.window.data2);
                        glViewport(0, 0, g_camera.screen_width, g_camera.screen_height);
                        width_speed = 2.0f * expf((g_camera.screen_width - INITIAL_SCREEN_WIDTH) / 1000.0f);
                        height_speed = 2.0f * expf((g_camera.screen_height - INITIAL_SCREEN_HEIGHT) / 1000.0f);
                        if (!SDL_GetCurrentDisplayMode(SDL_GetWindowDisplayIndex(g_window), &g_display_mode)) {
                            g_refresh_time = 1.0 / g_display_mode.refresh_rate;
                        }
                        g_screen_change = true;
                        break;
                    case SDL_WINDOWEVENT_MOVED:
                        if (!SDL_GetCurrentDisplayMode(SDL_GetWindowDisplayIndex(g_window), &g_display_mode)) {
                            g_refresh_time = 1.0 / g_display_mode.refresh_rate;
                        }
                        break;
                    default:
                        break;
                }
            default:
                break;
        }
    }
}

void update_GUI() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Edit Scene");

    static ImVec2 frame_size = {-1, -1};
    static ImVec2 frame_position = {-1, -1};

    if (
        frame_position.x != ImGui::GetWindowPos().x || frame_position.y != ImGui::GetWindowPos().y || 
        frame_size.x != ImGui::GetWindowSize().x || frame_size.y != ImGui::GetWindowSize().y
    ) {
        frame_position = ImGui::GetWindowPos();
        frame_size = ImGui::GetWindowSize();
        g_screen_change = true;
    }

    ImGui::Text("Functions");

    static int count = 1;
    static std::array<char[256], 8> input_strings {mine::INITIAL_FUNCTIONS};
    static std::array<int, 6> axes{mine::INITIAL_AXES};
    static std::array<std::array<int, 4>, 8> bounds{mine::INITIAL_BOUNDS};
    float half_space = (ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(" <= x <= ").x) * 0.5f;
    half_space = (half_space < 0) ? 0 : half_space;

    static auto domain_axes = [&](const char* str, int neg) {
        ImGui::SetNextItemWidth(half_space);
        ImGui::InputInt(("##intInput" + std::to_string(neg)).c_str(), &axes[neg], 0, 0, ImGuiInputTextFlags_None);
        ImGui::SameLine();
        ImGui::Text(str);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::InputInt(("##intInput" + std::to_string(neg + 1)).c_str(), &axes[neg + 1], 0, 0, ImGuiInputTextFlags_None);
    };

    static auto domain_functions = [&](const char* str, int func, int neg) {
        ImGui::SetNextItemWidth(half_space);
        ImGui::InputInt(("##intInput" + std::to_string(func) + std::to_string(neg)).c_str(), &bounds[func][neg], 0, 0, ImGuiInputTextFlags_None);
        ImGui::SameLine();
        ImGui::Text(str);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::InputInt(("##intInput" + std::to_string(func) + std::to_string(neg + 1)).c_str(), &bounds[func][neg + 1], 0, 0, ImGuiInputTextFlags_None);
    };

    for (int i = 0; i < count; ++i) {
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::InputText(mine::ids[2 * i], input_strings[i], sizeof(input_strings[i]));
        domain_functions("<= x <=", i, mine::NEG_X_BOUND);
        domain_functions("<= y <=", i, mine::NEG_Z_BOUND);
        if (ImGui::Button(mine::ids[2 * i + 1])) {
            g_plot.update_bounds(i, bounds[i]);
            g_camera.set_center(g_plot.bounds[i]);
            if (!update_function(input_strings[i], i)) {
                strcpy(input_strings[i], g_compute_pipeline.get_function(i));
            }
            g_scene_change = true;
        }
    }

    if (count < 8 && ImGui::Button("+")) {
        g_plot.add_function();
        g_plot.update_bounds(count, bounds[count]);
        update_function(input_strings[count], count);
        count++;
        g_size_change = true;
    }
    if (count < 8 && count > 0) {
        ImGui::SameLine();
    }
    if (count > 0 && ImGui::Button("-")) {
        g_plot.remove_function();
        count--;
        g_size_change = true;
    }

    domain_axes("<= X <=", 0);
    domain_axes("<= Y <=", 2);
    domain_axes("<= Z <=", 4);

    if (ImGui::Button("Set Bounds")) {
        g_plot.update_axes(axes);
        for (int i = 0; i < count; ++i) {
            update_function(input_strings[i], i);
        }
        for (int i = 0; i < count; ++i) {
            bounds[i] = g_plot.bounds[i];
        }
        g_size_change = true;
    }

    ImGui::End();
}

void reallocate_buffers() {
    glBufferData(
        GL_ARRAY_BUFFER, 
        g_plot.vertices.size() * sizeof(mine::vertex),
        g_plot.vertices.data(), 
        GL_STATIC_DRAW
    );
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        g_plot.indices.size() * sizeof(GLuint),
        g_plot.indices.data(),
        GL_STATIC_DRAW
    );
}

void predraw() {
    glUseProgram(g_graphics_pipeline.get_program());
}

void update_view() {
    glUniformMatrix4fv(g_graphics_pipeline.get_view_matrix_location(), 1, GL_FALSE, &g_camera.view[0][0]);
}

void draw() {
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glDrawElements(GL_LINES, g_plot.base_vertice_count, GL_UNSIGNED_INT, nullptr);
    glDrawElements(GL_TRIANGLES, g_plot.indices.size() - g_plot.base_vertice_count, GL_UNSIGNED_INT, (void*)(g_plot.base_vertice_count * sizeof(GLuint)));
}

void postdraw() {
    glUseProgram(0);
}

void loop() {
    Uint64 prev_counter = SDL_GetPerformanceCounter();
    Uint64 curr_counter{};
    double elapsed_time{};
    double frame_elapsed_time{};
    int frame_count{};
    int FPS;

    while (g_running) {
        input();
        if (frame_elapsed_time >= g_refresh_time) {
            update_GUI();
            
            if (g_size_change) {
                reallocate_buffers();
                predraw();
                update_view();
                draw();
                postdraw();
                g_size_change = false;
                g_scene_change = false;
                g_screen_change = false;
            } else if (g_scene_change) {
                predraw();
                update_view();
                draw();
                postdraw();
                g_scene_change = false;
                g_screen_change = false;
            } else if (g_screen_change) {
                predraw();
                draw();
                postdraw();
                g_screen_change = false;
            }

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            SDL_GL_SwapWindow(g_window);

            frame_count++;
            frame_elapsed_time = 0.0;
        }

        curr_counter = SDL_GetPerformanceCounter();
        double frame_time = (double)(curr_counter - prev_counter) / SDL_GetPerformanceFrequency();
        elapsed_time += frame_time;
        frame_elapsed_time += frame_time;

        if (elapsed_time >= 1.0) {
            FPS = frame_count;
            std::string title = std::to_string(FPS) + " FPS";
            SDL_SetWindowTitle(g_window, title.c_str());

            frame_count = 0;
            elapsed_time = 0.0;
        }

        prev_counter = curr_counter;
    }
}

void cleanup() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyWindow(g_window);

    glDeleteBuffers(1, &g_VBO);
    glDeleteBuffers(1, &g_IBO);
    glDeleteVertexArrays(1, &g_VAO);

    glDeleteProgram(g_graphics_pipeline.get_program());
    glDeleteProgram(g_compute_pipeline.get_program());

    SDL_Quit();
}

int main() {
    setup();

    vertex_specification();

    loop();

    cleanup();

    return 0;
}