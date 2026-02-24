#include <string>

class Resource
{
private:
    /* data */
public:
    Resource(std::string name, int currentReourceAmount, int maxResourceAmount);
    ~Resource();
};

Resource::Resource(std::string name, int currentReourceAmount, int maxResourceAmount)
{
}

Resource::~Resource()
{
}
