#ifndef __AUTUMN_STATE_HPP__
#define __AUTUMN_STATE_HPP__
#include <utility>

namespace Autumn {
class State {
private:
  bool clicked;
  int x;
  int y;

  bool left;
  bool right;
  bool up;
  bool down;

public:
  State()
      : clicked(false), x(-1), y(-1), left(false), right(false), up(false),
        down(false) {}
  void click(int x, int y) {
    clicked = true;
    this->x = x;
    this->y = y;
  }

  void pushUp() { up = true; }
  void pushDown() { down = true; }
  void pushLeft() { left = true; }
  void pushRight() { right = true; }

  bool getClicked() { return clicked; }
  int getX() { return x; }
  int getY() { return y; }

  bool getLeft() { return left; }
  bool getRight() { return right; }
  bool getUp() { return up; }
  bool getDown() { return down; }

  void reset() {
    clicked = false;
    x = -1;
    y = -1;
    left = false;
    right = false;
    up = false;
    down = false;
  }
};
} // namespace Autumn

#endif
