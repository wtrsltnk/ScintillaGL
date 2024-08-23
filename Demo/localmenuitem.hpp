#ifndef LOCALMENUITEM_HPP
#define LOCALMENUITEM_HPP

#include <functional>
#include <string>
#include <vector>

class LocalMenuItem
{
public:
    LocalMenuItem(const std::string &n) : name(n) {}
    LocalMenuItem(const std::string &n, int d) : name(n), subMenuDirection(d) {}
    LocalMenuItem(const std::string &n, bool e) : name(n), enabled(e) {}
    LocalMenuItem(const std::string &n, bool e, int d) : name(n), enabled(e), subMenuDirection(d) {}
    LocalMenuItem(const std::string &n, const std::vector<LocalMenuItem> &s) : name(n), subMenu(s) {}

    std::string name;
    bool enabled = true;
    int subMenuDirection = 0;
    std::vector<LocalMenuItem> subMenu;
    std::function<void()> action = []() {};
};

#endif // LOCALMENUITEM_HPP
