#include <ps2/window.hpp>
#include <ps2/retro_target.hpp>
#include <ps2/model.hpp>

#include <vector>

int main(void) {
  window win("Tundra", 1280, 960);
  retro_target ren(640, 480);
  
  // --- vertex data: one triangle, NDC coords (-1..1) ---
  const std::vector<float> vertices{
    0.0f,  0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
  };

  model m(vertices, "test.vert", "test.frag");
  
  // --- main loop ---
  while (!win.should_close()) {
    win.poll_events();
    if (win.is_key_down(Key::Escape)) break;

    ren.begin_scene(0.1, 0.1, 0.12, 1.0);

    m.render();

    ren.composite(1280, 960);
    win.present();
  }
  
  return 0;
}