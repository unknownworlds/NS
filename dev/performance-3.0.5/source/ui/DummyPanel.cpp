#include "ui/DummyPanel.h"

void DummyPanel::paint()
{
    gHUD.ComponentJustPainted(this);
}

void DummyPanel::paintBackground()
{
}