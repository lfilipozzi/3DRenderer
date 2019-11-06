#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <QPoint>

/// Manage the keyboard and mouse inputs.
/**
 * This class make a distinction between keyboard keys and mouse buttons. The 
 * state of the buttons and keys can be obtained from the public methods. The 
 * registration of the input (key and button press and release) is managed by 
 * this class. The private static method update() is used to update the inputs.
 */
class InputManager {
public:
    /**
     * @brief Possible key/button states.
     * InputInvalid indicates an invalid input.
     * InputRegistered: A new input has been registered.
     * InputTrigerred: After registering the input, its state is change to 
     * InputTrigerred. This allows to call functions when an input is triggered 
     *      (short) contrary to pressed (long). InputPressed: After trigering  
     *      the input, it is pressed. This allows to call function when an input 
     *      must be pressed.
     * InputUnregistered: The input is no longer pressed.
     * InputReleased: The input has been unregistered.
     */
    enum InputState {
        InputInvalid,
        InputRegistered,
        InputUnregistered,
        InputTriggered,
        InputPressed,
        InputReleased
    };

    /**
     * Return the state of the key (e.g., trigerred, pressed, ...)
     * @param key The key.
     * @return The state of the key.
     */
    static InputState keyState(Qt::Key key);

    /**
     * Check if a key has been trigerred.
     * @param key The key.
     * @return Boolean.
     */
    static bool isKeyTriggered(Qt::Key key);

    /**
     * Check if a key has been pressed.
     * @param key The key.
     * @return Boolean.
     */
    static bool isKeyPressed(Qt::Key key);

    /**
     * Check if a key has been released.
     * @param key The key.
     * @return Boolean.
     */
    static bool isKeyReleased(Qt::Key key);

    /**
     * Return the state of the button (e.g., trigerred, pressed, ...)
     * @param key The button.
     * @return The state of the button.
     */
    static InputState buttonState(Qt::MouseButton button);

    /**
     * Check if a button has been trigerred.
     * @param key The button.
     * @return Boolean.
     */
    static bool isButtonTriggered(Qt::MouseButton button);

    /**
     * Check if a button has been pressed.
     * @param key The button.
     * @return Boolean.
     */
    static bool isButtonPressed(Qt::MouseButton button);

    /**
     * Check if a button has been released.
     * @param key The button.
     * @return Boolean.
     */
    static bool isButtonReleased(Qt::MouseButton button);

    /**
     * Give the cursor position on the screen.
     * @return The cursor position.
     */
    static QPoint mousePosition();

    /**
     * Return the distance traveled by the mouse between two frames.
     * @return The difference of position.
     */
    static QPoint mouseDelta();

    /**
     * Get the wheel scroll and reset it to zero.
     * @return The wheel scroll.
     */
    static int getWheelScroll();

private:
    /**
     * @brief Update the state of all the inputs (button, keys, and mouse
     * position and displacement).
     */
    static void update();

    /**
     * @brief Register a key press. Check the container, if the key is not in 
     * the container, the key is added to the container with state 
     * InputRegistered. The input can now be processed.
     * @param key The key.
     */
    static void registerKeyPress(int key);

    /**
     * @brief Register a key release. Find the key in the container and update 
     * its state to InputUnregistered.
     * @param key The key
     */
    static void registerKeyRelease(int key);

    /**
     * @brief Register a mouse press. Check the container, if the mouse button 
     * is not in the container, the button is added to the container with state 
     * InputRegistered. The input can now be processed.
     * @param button The button.
     */
    static void registerMousePress(Qt::MouseButton button);

    /**
     * @brief Register a mouse release. Find the button in the container and
     * update its state to InputUnregistered.
     * @param button The button.
     */
    static void registerMouseRelease(Qt::MouseButton button);

    /**
     * @brief Register the mouse scroll wheel.
     * @param deltaScroll The difference of the mouse scroll wheel.
     */
    static void registerWheelScroll(int deltaScroll);

    /**
     * @brief Empty the containers which will discard all the inputs.
     */
    static void reset();

    /*
     * Give access to the OpenGLWindow to the class's private methods to 
     * register key and button press and release.
     */
    friend class OpenGLWindow;
};

inline bool InputManager::isKeyTriggered(Qt::Key key) {
    return keyState(key) == InputTriggered;
}

inline bool InputManager::isKeyPressed(Qt::Key key) {
    return keyState(key) == InputPressed;
}

inline bool InputManager::isKeyReleased(Qt::Key key) {
    return keyState(key) == InputReleased;
}

inline bool InputManager::isButtonTriggered(Qt::MouseButton button) {
    return buttonState(button) == InputTriggered;
}

inline bool InputManager::isButtonPressed(Qt::MouseButton button) {
    return buttonState(button) == InputPressed;
}

inline bool InputManager::isButtonReleased(Qt::MouseButton button) {
    return buttonState(button) == InputReleased;
}

#endif // INPUTMANAGER_H
