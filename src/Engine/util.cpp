#include "util.hpp"

#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>

#include <SDL3/SDL.h>

namespace fs = std::filesystem;

static std::optional<fs::path> resolve_resource(const std::string &filename) {
  const char *base = SDL_GetBasePath();
  if (!base) {
    SDL_Log("SDL_GetBasePath failed: %s", SDL_GetError());
    return std::nullopt;
  }

  fs::path base_path(base);

  fs::path full_path = base_path / "res" / filename;
  return full_path;
}


std::optional<std::string> load_resource_txt(const std::string &path) {
  if (auto fp = resolve_resource(path)) {
    std::ifstream file(*fp);
    if (!file) return std::nullopt;

    std::stringstream buffer;
    buffer << file.rdbuf();

    std::string contents = buffer.str();
    return contents;
  }

  return std::nullopt;
}
