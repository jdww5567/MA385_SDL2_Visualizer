#ifndef MINE_GUI_HPP
#define MINE_GUI_HPP

#include <glad/glad.h>
#include <SDL2/SDL.h>

namespace mine {
class gui {
public:
    gui();

    void set_version(SDL_Window* window, SDL_GLContext context);
private:
};
}

#endif