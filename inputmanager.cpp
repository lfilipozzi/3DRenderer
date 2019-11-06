#include "inputmanager.h"
#include <QCursor>
#include <vector>

template <typename T>
/**
 * @brief Structure representing an input and its state by a pair.
 */
struct InputInstance : std::pair<T, InputManager::InputState> {
    typedef std::pair<T, InputManager::InputState> base_class;
    inline InputInstance(T value) : base_class(value, InputManager::InputInvalid) {}
    inline InputInstance(T value, InputManager::InputState state) : base_class(value, state) {}
    // Redefine operator== to give the expected result when using std::find
    inline bool operator==(const InputInstance &rhs) const {
        return this->first == rhs.first;
    }
};

/**
 * @brief Pair containing a key and its InputManager::InputState.
 */
typedef InputInstance<Qt::Key> KeyInstance;

/**
 * @brief Pair containing a mouse button and ist InputManager::InputState.
 */
typedef InputInstance<Qt::MouseButton> ButtonInstance;

/**
 * @brief Vector of KeyInstance (i.e. pair containing a key and its InputManager::InputState).
 */
typedef std::vector<KeyInstance> KeyContainer;

/**
 * @brief Vector of ButtonInstance (i.e. pair containing a button and its InputManager::InputState).
 */
typedef std::vector<ButtonInstance> ButtonContainer;


// Globals
/**
 * @brief Static variable of the key inputs container.
 */
static KeyContainer sg_keyInstances;

/**
 * @brief Static variable of the mouse button container.
 */
static ButtonContainer sg_buttonInstances;

/**
 * @brief Static variable of the cursor current position.
 */
static QPoint sg_mouseCurrPosition;

/**
 * @brief Static variable of the cursor previous position.
 */
static QPoint sg_mousePrevPosition;

/**
 * @brief Static variable of the cursor displacement between the current frame and the previous frame.
 */
static QPoint sg_mouseDelta;

/**
 * @brief The difference of the mouse scroll wheel.
 */
static int sg_mouseScrollDelta;


/**
 * This inline function makes it easier to use the containers from the STL without increasing
 * the overhead by calling functions.
 * @brief Return the position of a key in the container if it exists.
 * @param value The key.
 * @return The position.
 */
static inline KeyContainer::iterator FindKey(Qt::Key value) {
    return std::find(sg_keyInstances.begin(), sg_keyInstances.end(), value);
}

/**
 * This inline function makes it easier to use the containers from the STL without increasing
 * the overhead by calling functions.
 * @brief Return the position of a button in the container if it exists.
 * @param value The button.
 * @return The position.
 */
static inline ButtonContainer::iterator FindButton(Qt::MouseButton value) {
    return std::find(sg_buttonInstances.begin(), sg_buttonInstances.end(), value);
}

template <typename TPair>
/**
 * Update the state of a key/button. The sequence is:
 * registered -> triggerred -> pressed and unregistered -> released.
 * @param instance
 */
static inline void UpdateStates(TPair &instance) {
    switch (instance.second) {
    case InputManager::InputRegistered:
        instance.second = InputManager::InputTriggered;
        break;
    case InputManager::InputTriggered:
        instance.second = InputManager::InputPressed;
        break;
    case InputManager::InputUnregistered:
        instance.second = InputManager::InputReleased;
        break;
    default:
        break;
    }
}

template <typename TPair>
/**
 * Check if a key has been released.
 * @param instance
 * @return
 */
static inline bool CheckReleased(const TPair &instance) {
    return instance.second == InputManager::InputReleased;
}

template <typename Container>
/**
 * @brief Update a container (of keys or mouse buttons pressed), remove released inputs
 * from the containers.
 * @param container
 */
static inline void Update(Container &container) {
    typedef typename Container::iterator Iter;
    typedef typename Container::value_type TPair;

    // Remove old data
    Iter remove = std::remove_if(container.begin(), container.end(), &CheckReleased<TPair>);
    container.erase(remove, container.end());

    // Update existing data
    std::for_each(container.begin(), container.end(), &UpdateStates<TPair>);
}

/**
 * @brief Return the state of a key.
 * @param key The key.
 * @return The state.
 */
InputManager::InputState InputManager::keyState(Qt::Key key) {
    KeyContainer::iterator it = FindKey(key);
    return (it != sg_keyInstances.end()) ? it->second : InputInvalid;
}

/**
 * @brief Return the state of a button.
 * @param button The button.
 * @return The state.
 */
InputManager::InputState InputManager::buttonState(Qt::MouseButton button) {
    ButtonContainer::iterator it = FindButton(button);
    return (it != sg_buttonInstances.end()) ? it->second : InputInvalid;
}

/**
 * @brief Return the cursor position
 * @return The cursor position.
 */
QPoint InputManager::mousePosition() {
    return QCursor::pos();
}

/**
 * @brief Return the displacement of the mouse cursor between two frames.
 * @return The displacement of the cursor.
 */
QPoint InputManager::mouseDelta() {
    return sg_mouseDelta;
}

void InputManager::update() {
    // Update Mouse Delta
    sg_mousePrevPosition = sg_mouseCurrPosition;
    sg_mouseCurrPosition = QCursor::pos();
    sg_mouseDelta = sg_mouseCurrPosition - sg_mousePrevPosition;

    // Update KeyState values
    Update(sg_buttonInstances);
    Update(sg_keyInstances);
}

void InputManager::registerKeyPress(int key) {
    KeyContainer::iterator it = FindKey(static_cast<Qt::Key>(key));
    if (it == sg_keyInstances.end()) {
        sg_keyInstances.push_back(KeyInstance(static_cast<Qt::Key>(key), InputRegistered));
    }
}

void InputManager::registerKeyRelease(int key) {
    KeyContainer::iterator it = FindKey(static_cast<Qt::Key>(key));
    if (it != sg_keyInstances.end())
    {
        it->second = InputUnregistered;
    }
}

void InputManager::registerMousePress(Qt::MouseButton button)
{
    ButtonContainer::iterator it = FindButton(button);
    if (it == sg_buttonInstances.end()) {
        sg_buttonInstances.push_back(ButtonInstance(button, InputRegistered));
    }
}

void InputManager::registerMouseRelease(Qt::MouseButton button)
{
    ButtonContainer::iterator it = FindButton(button);
    if (it != sg_buttonInstances.end()) {
        it->second = InputUnregistered;
    }
}

void InputManager::registerWheelScroll(int deltaScroll) {
    sg_mouseScrollDelta = deltaScroll;
}

int InputManager::getWheelScroll() {
    int temp = sg_mouseScrollDelta;
    sg_mouseScrollDelta = 0;
    return temp;
}

void InputManager::reset() {
    sg_keyInstances.clear();
    sg_buttonInstances.clear();
}










