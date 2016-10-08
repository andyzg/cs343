#include <iostream>
#include <vector>
#include "prng.h"
#include "q2.h"

using namespace std;

static PRNG ran;
static unsigned int START = 25; // Number of players will never exceed 20

void Potato::reset( unsigned int maxTicks ) {
  this->maxTicks = ran( 1, maxTicks );
}

void Potato::countdown() {}

void Umpire::main() {
  Potato potato(10);

  int first = ran(0, this->players->size());
  Player::PlayerList::const_iterator jt = this->players->begin();
  for (int i = 0; i < first; i++) { jt++; }
  cout << "lol Omg" << endl;
  jt->second->toss(potato);

  while (this->players->size() != 1) {
    suspend();

    // Remove the player that was just eliminated
    this->players->erase(this->current);

    cout << this->current << " was eliminated" << endl;
    // Get a random player again
    Player::PlayerList::const_iterator it = this->players->begin();
    int index = ran(0, this->players->size());
    for (int i = 0; i < index; i++) { it++; }

    // Toss the potato
    potato.reset(10);
    it->second->toss(potato);
  }
}

void Umpire::set( unsigned int player ) {
  this->current = player;
  resume();
}

void Player::main() {
  cout << "Start some shit" << endl;
  while (true) {
    cout << "Start some shit" << endl;
    try {
      this->potato->countdown();

      // If succeeds, continue.
      // Get player at a random index.
      int index = ran(0, this->players->size());
      Player::PlayerList::const_iterator it = this->players->begin();
      for (int i = 0; i < index; i++) {
        it++;
      }

      // Toss it to a random player
      it->second->toss(*this->potato);

    } catch (Potato::Explode e) {
      this->umpire->set(this->id);
    }
    suspend();
  }
}

void Player::toss( Potato &potato ) {
  cout << "Omg " << &potato << endl;
  this->potato = &potato;
  cout << "FUCK" << endl;
  resume();
}

void uMain::main() {
  if (argc < 2) {
    cout << "Usage: " << argv[0] << " number-of-players (2 <= and <20) [ random-seed (>= 0) ]" << endl;
    return;
  }

  int numPlayers = stoi(argv[1]);
  int s = getpid(); // TODO: Make random

  Player::PlayerList players;
  Umpire u(players);

  for (int i = 0; i < numPlayers; i++) {
    Player p( u, i, players );
    Potato potato(10);
    p.toss(potato);
    players[i] = &p;
  }
  ran.seed(s);

  // u.set(START);
}
