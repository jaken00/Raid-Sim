#include <vector>

#include "../characters/CharacterClass.h"
#include "../characters/character.h"

class gamestate {
private:
public:
    gamestate(Character playerCharacter);
    ~gamestate();
};

gamestate::gamestate(Character playerCharacter) {}

gamestate::~gamestate() {}
