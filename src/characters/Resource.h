#include <string>

class Resource
{
private:
    /* data */
public: //This is like Mana, Energy, Rage etc
    Resource(std::string name, int currentReourceAmount, int maxResourceAmount);
    ~Resource();
};

Resource::Resource(std::string name, int currentReourceAmount, int maxResourceAmount)
{
}

Resource::~Resource()
{
}
