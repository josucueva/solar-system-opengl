#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include <string>

using namespace std;

class Texture {
public:
  unsigned int ID;
  string type;
  string path;

  Texture(const char *texturePath);
  void bind(unsigned int unit = 0) const;
  void unbind() const;
  static unsigned int loadFromFile(const char *path);
};

#endif
