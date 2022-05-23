typedef struct {
	SDL_Keycode sdl;
	UINT win;
} keycode_conversion;

static const keycode_conversion table[] = {
    {SDLK_F1, VK_F1},
    {SDLK_F2, VK_F2},
    {SDLK_F3, VK_F3},
    {SDLK_F4, VK_F4},
    {SDLK_F5, VK_F5},
    {SDLK_F6, VK_F6},
    {SDLK_F7, VK_F7},
    {SDLK_F8, VK_F8},
    {SDLK_F9, VK_F9},
    {SDLK_F10, VK_F10},
    {SDLK_F11, VK_F11},
    {SDLK_F13, VK_F13},
    {SDLK_F14, VK_F14},
    {SDLK_F15, VK_F15},
    {SDLK_F16, VK_F16},
    {SDLK_F17, VK_F17},
    {SDLK_F18, VK_F18},
    {SDLK_F19, VK_F19},
    {SDLK_F20, VK_F20},
    {SDLK_F21, VK_F21},
    {SDLK_F22, VK_F22},
    {SDLK_F23, VK_F23},
    {SDLK_F24, VK_F24}
};
