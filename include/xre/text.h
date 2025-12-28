#pragma once

// DirectX includes
#include <DirectXTex/WICTextureLoader11.h>

// XRe includes
#include <xre/structs.h>
#include <xre/utils.h>
#include <xre/renderable.h>

// Other includes
#include <fstream>
#include <vector>

// TODO: move to an approach using a model with multiple meshes making up the text
class Text : public Renderable {
public:
  Text(const char *sentence);

  void render();

private:
  // Using the extendes ASCII charset for now, which has 224 characters (skipping control character)
  const int CHAR_COUNT = 224;

  // Steps for going through the texture
  const float X_STEP = 1.0f / 32.0f;
  const float Y_STEP = 1.0f / 7.0f;

  // Text does not have a bounding box
  inline bool hasBoundingBox() override { return false; };

  void buildMeshesFromSentence(const char *sentence);
  inline text_char_t computeTextureOffsets(int letter);
};
