#pragma once

#include "window.h"
#include <GLFW/glfw3.h>
#include "key_codes.h"
#include "mouse_codes.h"
#include "assert.h"

namespace Engine {

struct MousePosition
{
    double x;
    double y;
};

class Input
{
public:
    static void init(Window& window);

    static bool isKeyDown(Key key);
    static bool isKeyReleased(Key key);

    static bool isMouseDown(Mouse button);
    static bool isMouseReleased(Mouse button);

    static MousePosition getMousePosition();

    static void disableCursor();
    static void enableCursor();

private:
    static GLFWwindow* m_window;
};

}