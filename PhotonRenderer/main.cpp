#include "Engine/Engine.hpp"

extern "C" {
__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}

int main() {
  photon::Engine engine;
  engine.Initialize("SimpleScene");
  engine.Run();
  return 0;
}