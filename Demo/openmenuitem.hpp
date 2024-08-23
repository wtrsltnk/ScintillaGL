#ifndef OPENMENUITEM_HPP
#define OPENMENUITEM_HPP

#include "localmenuitem.hpp"

#include "Platform.h"

class OpenMenuItem
{
public:
    OpenMenuItem(const LocalMenuItem *m, const PRectangle &r) : menuItem(m), rect(r) {}

    const LocalMenuItem *menuItem = nullptr;
    PRectangle rect;
};

#endif // OPENMENUITEM_HPP
