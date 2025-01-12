#ifndef INPUT_H
#define INPUT_H

#include <GLFW/glfw3.h>
#include "key_codes.h"
#include "mouse_codes.h"

struct MousePosition
{
    double x;
    double y;
};

class Input
{
public:
    // Initialize input system
    static void init(GLFWwindow* window);

    static bool isKeyDown(Key::KeyCode key);
    static bool isKeyReleased(Key::KeyCode key);

    static bool isMouseDown(Mouse::MouseCode button);
    static bool isMouseReleased(Mouse::MouseCode button);

    static MousePosition getMousePosition();

    static void disableCursor();
    static void enableCursor();

private:
    static GLFWwindow* m_window;
};

#endif // INPUT_H
