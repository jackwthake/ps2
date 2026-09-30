#include <ps2/window.hpp>

#include <glad/glad.h>
#include <cstdio>
#include <cstdlib>

window::window(const std::string &title, unsigned width, unsigned height) {
  SDL_Init(SDL_INIT_VIDEO);

  // Request GL 3.3 core BEFORE creating the window
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  this->win = SDL_CreateWindow(title.c_str(), width, height, SDL_WINDOW_OPENGL);
  this->ctx = SDL_GL_CreateContext(this->win);

  if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
    fprintf(stderr, "failed to load GL\n");
    exit(EXIT_FAILURE);
  }
}


void window::clear(float r, float g, float b, float a) {
  glClearColor(r, g, b, a);
  glClear(GL_COLOR_BUFFER_BIT);
}

void window::present() {
  SDL_GL_SwapWindow(win);
}


Key window::translate_scancode(SDL_Scancode code) {
  switch (code) {
    case SDL_SCANCODE_W:      return Key::W;
    case SDL_SCANCODE_A:      return Key::A;
    case SDL_SCANCODE_S:      return Key::S;
    case SDL_SCANCODE_D:      return Key::D;
    case SDL_SCANCODE_SPACE:  return Key::Space;
    case SDL_SCANCODE_ESCAPE: return Key::Escape;
    case SDL_SCANCODE_UP:     return Key::Up;
    case SDL_SCANCODE_DOWN:   return Key::Down;
    case SDL_SCANCODE_LEFT:   return Key::Left;
    case SDL_SCANCODE_RIGHT:  return Key::Right;
    default:                  return Key::Unknown;
  }
}


MouseButton window::translate_mouse_button(uint8_t button) {
  switch (button) {
    case SDL_BUTTON_LEFT:   return MouseButton::Left;
    case SDL_BUTTON_RIGHT:  return MouseButton::Right;
    case SDL_BUTTON_MIDDLE: return MouseButton::Middle;
    default:                return MouseButton::Left; // no Unknown slot; adjust if needed
  }
}


void window::poll_events() {
  event_queue.clear();

  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    switch (e.type) {
      case SDL_EVENT_QUIT:
        close_requested = true;
        event_queue.push_back(QuitEvent{});
        break;

      case SDL_EVENT_KEY_DOWN:
      case SDL_EVENT_KEY_UP: {
        bool pressed = (e.type == SDL_EVENT_KEY_DOWN);
        Key k = translate_scancode(e.key.scancode);
        if (k != Key::Unknown) key_state[static_cast<size_t>(k)] = pressed;
        event_queue.push_back(KeyEvent{k, pressed});
        break;
      }

      case SDL_EVENT_MOUSE_BUTTON_DOWN:
      case SDL_EVENT_MOUSE_BUTTON_UP: {
        bool pressed = (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN);
        MouseButton b = translate_mouse_button(e.button.button);
        mouse_state[static_cast<size_t>(b)] = pressed;
        event_queue.push_back(MouseButtonEvent{
          b, pressed, static_cast<int>(e.button.x), static_cast<int>(e.button.y)
        });
        break;
      }

      case SDL_EVENT_MOUSE_MOTION:
        mouse_x = static_cast<int>(e.motion.x);
        mouse_y = static_cast<int>(e.motion.y);
        event_queue.push_back(MouseMoveEvent{
          mouse_x, mouse_y,
          static_cast<int>(e.motion.xrel), static_cast<int>(e.motion.yrel)
        });
        break;

      case SDL_EVENT_WINDOW_RESIZED:
        event_queue.push_back(ResizeEvent{e.window.data1, e.window.data2});
        break;

      default:
        break;
    }
  }
}


window::~window() {
  SDL_GL_DestroyContext(ctx);
  SDL_DestroyWindow(win);
  SDL_Quit();
}