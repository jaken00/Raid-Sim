#include <string>
#include "Resource.h"
#include "enum.h"

class Spec
{
private:
    /* data */
public:
    Spec(std::string name, Resource resouceUsed, AttackRange AttackRange);
    ~Spec();
};

Spec::Spec(std::string name, Resource resouceUsed, AttackRange AttackRange)
{
}

Spec::~Spec()
{
}
