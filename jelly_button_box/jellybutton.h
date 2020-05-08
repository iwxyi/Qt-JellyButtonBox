#ifndef JELLYBUTTON_H
#define JELLYBUTTON_H

#include "watercirclebutton.h"

class JellyButton : public WaterCircleButton
{
public:
    JellyButton(QIcon ic, QWidget* parent = nullptr);
    JellyButton(QPixmap ic, QWidget* parent = nullptr);
};

#endif // JELLYBUTTON_H
