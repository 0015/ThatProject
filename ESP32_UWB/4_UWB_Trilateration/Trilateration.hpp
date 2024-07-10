#include <stdio.h>

class Anchor {
public:
  float x;
  float y;
  float distance;

  Anchor(float x, float y) {
    this->x = x;
    this->y = y;
    this->distance = 0.0;
  }

  void updateDistance(float distance) {
    this->distance = distance;
  }
};

class Trilateration {
private:
  Anchor *anchor0;
  Anchor *anchor1;
  Anchor *anchor2;

public:
  Trilateration(Anchor *a0, Anchor *a1, Anchor *a2) {
    anchor0 = a0;
    anchor1 = a1;
    anchor2 = a2;
  }

  void calcUserLocation(float &user_x, float &user_y) {
    float A = 2 * (anchor1->x - anchor0->x);
    float B = 2 * (anchor1->y - anchor0->y);
    float C = sq(anchor0->distance) - sq(anchor1->distance) - sq(anchor0->x) + sq(anchor1->x) - sq(anchor0->y) + sq(anchor1->y);
    float D = 2 * (anchor2->x - anchor1->x);
    float E = 2 * (anchor2->y - anchor1->y);
    float F = sq(anchor1->distance) - sq(anchor2->distance) - sq(anchor1->x) + sq(anchor2->x) - sq(anchor1->y) + sq(anchor2->y);

    user_x = ((F * B) - (E * C)) / ((B * D) - (E * A));
    user_y = ((F * A) - (D * C)) / ((A * E) - (D * B));
  }
};
