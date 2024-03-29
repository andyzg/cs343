#include <map>
#include <iostream>

using namespace std;

class Potato {
    // YOU ADD MEMBERS HERE
    unsigned int maxTicks;
  public:
    _Event Explode {};
    Potato( unsigned int maxTicks = 10 ) {
      this->reset(maxTicks);
    }
    void reset( unsigned int maxTicks = 10 );
    void countdown();
};

_Coroutine Umpire;

_Coroutine Player {
    void main();
    Potato* potato;
    Umpire* umpire;
    unsigned int id;
  public:
    typedef std::map<int, Player*> PlayerList; // container type of your choice
  private:
    const Player::PlayerList *players;
  public:
    Player( Umpire &umpire, unsigned int Id, Player::PlayerList &players )  {
      this->potato = NULL;
      this->umpire = &umpire;
      this->id = Id;
      this->players = &players;
    }
    void toss( Potato &potato );
};

_Coroutine Umpire {
    void main();
    Player::PlayerList *players;
    unsigned int current;
  public:
    Umpire( Player::PlayerList &players ) {
      Umpire::players = &players;
    }
    void set( unsigned int player );
};
