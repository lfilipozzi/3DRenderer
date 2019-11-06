#ifndef PLAYERBUTTON_H
#define PLAYERBUTTON_H

#include <QPushButton>

/// Button to control the animation
/**
 * Personalized QPushButton
 */
class PlayerButton : public QPushButton
{
public:
    PlayerButton(QString iconName, QIcon fallbackIcon, 
                 QWidget *parent = nullptr);
};

#endif // PLAYERBUTTON_H
