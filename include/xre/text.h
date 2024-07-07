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
  Text(const char* sentence);

  void render();

private:
  // Using the extendes ASCII charset for now, which has 224 characters (skipping control character)
  const int CHAR_COUNT = 224;

  // Steps for going through the texture
  const float X_STEP = 1.0f / 32.0f;
  const float Y_STEP = 1.0f / 7.0f;

  Bitmap m_bitmap;

  void buildMeshesFromSentence(const char* sentence);
  inline text_char_t computeTextureOffsets(int letter);
};
