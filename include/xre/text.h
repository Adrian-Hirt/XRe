#pragma once

// DirectX includes
#include <d3d11.h>
#include <DirectXTex/WICTextureLoader11.h>

// XRe includes
#include <xre/structs.h>
#include <xre/utils.h>
#include <xre/bitmap.h>

// Other includes
#include <fstream>
#include <vector>

class Text {
public:
  Text();

  void buildMeshesFromSentence(const char* sentence);
  void render();

private:
  text_char_t *font;
  std::vector<Bitmap> bitmaps;
};
