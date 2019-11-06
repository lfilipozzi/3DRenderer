#include "playerbutton.h"

PlayerButton::PlayerButton(QString iconName, QIcon fallbackIcon, 
                           QWidget *parent) : QPushButton(parent) {
    this->setIcon(QIcon::fromTheme(iconName, fallbackIcon));
    this->setFixedSize(30, 30);
}
