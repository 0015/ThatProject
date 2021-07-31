#include "FileIO.h"
#include <Update.h>

class Updater {
private:
  void performUpdate(Stream& updateSource, size_t updateSize);

public:
  bool updateFromFS(FileIO** fileIO);
};
