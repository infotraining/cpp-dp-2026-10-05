#ifndef COMPOSITE_HPP_
#define COMPOSITE_HPP_

#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <string>

class Component
{
private:
    std::string name_;

public:
    explicit Component(std::string name)
        : name_(std::move(name))
    {
    }
    
    virtual void display(int depth) = 0;
    virtual ~Component() = default;

    const std::string& name() const { return name_; }
};

using ComponentPtr = std::shared_ptr<Component>;

class Composite : public Component
{
private:
    std::list<ComponentPtr> children;

public:
    explicit Composite(std::string name)
        : Component(std::move(name))
    {
    }

    void add(ComponentPtr c)
    {
        children.push_back(c);
    }

    void remove(ComponentPtr c)
    {
        children.remove(c);
    }

    void display(int depth) override
    {
        std::cout << std::string(depth, '-') << name() << std::endl;

        for (const auto& child : children)
            child->display(depth + 2);
    }
};

// "Leaf"
class Leaf : public Component
{
public:
    explicit Leaf(std::string name)
        : Component(std::move(name))
    {
    }

    void display(int depth) override
    {
        std::cout << std::string(depth, '-') << name() << std::endl;
    }
};

#endif /*COMPOSITE_HPP_*/
