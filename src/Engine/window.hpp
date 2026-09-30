#pragma once

#include <string>
#include <vector>
#include <array>
#include <variant>
#include <cstdint>
#include <SDL3/SDL.h>

enum class Key : uint16_t {
  Unknown, W, A, S, D, Space, Escape, Up, Down, Left, Right, Count
};

enum class MouseButton : uint8_t { Left, Right, Middle, Count };

struct KeyEvent         { Key key; bool pressed; };
struct MouseButtonEvent { MouseButton button; bool pressed; int x, y; };
struct MouseMoveEvent   { int x, y, dx, dy; };
struct ResizeEvent      { int width, height; };
struct QuitEvent        {};

using Event = std::variant<KeyEvent, MouseButtonEvent, MouseMoveEvent, ResizeEvent, QuitEvent>;

class window {
  SDL_Window *win;
  SDL_GLContext ctx;

  std::vector<Event> event_queue;
  std::array<bool, static_cast<size_t>(Key::Count)> key_state{};
  std::array<bool, static_cast<size_t>(MouseButton::Count)> mouse_state{};
  int mouse_x = 0, mouse_y = 0;
  bool close_requested = false;

  static Key translate_scancode(SDL_Scancode code);
  static MouseButton translate_mouse_button(uint8_t button);

public:
  window(const std::string &title, unsigned width, unsigned height);
  ~window();

  void clear(float r, float g, float b, float a);
  void present();

  void poll_events();
  const std::vector<Event> &events() const { return event_queue; }

  bool is_key_down(Key key) const { return key_state[static_cast<size_t>(key)]; }
  bool is_mouse_down(MouseButton button) const { return mouse_state[static_cast<size_t>(button)]; }
  void mouse_position(int &x, int &y) const { x = mouse_x; y = mouse_y; }

  bool should_close() const { return close_requested; }
};