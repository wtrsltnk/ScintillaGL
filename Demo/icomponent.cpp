
#include "icomponent.hpp"

std::shared_ptr<IComponent> IComponent::componentWithKeyboardFocus = nullptr;

bool IComponent::isHit(
    const glm::vec2 &p)
{
    if (p.x >= _origin.x && p.y >= _origin.y && p.x <= (_origin.x + _width) && p.y <= (_origin.y + _height))
    {
        return true;
    }

    return false;
}

int IComponent::width()
{
    return _width;
}

int IComponent::height()
{
    return _height;
}
