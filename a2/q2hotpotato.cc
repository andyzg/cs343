#include <iostream>
#include <vector>
#include "PRNG.h"
#include "q2.h"

using namespace std;

static PRNG ran;
static unsigned int START = 25; // Number of players will never exceed 20

void Potato::reset( unsigned int maxTicks ) {
  this->maxTicks = ran( 1, maxTicks );
  cout << "  POTATO goes off after " << this->maxTicks << " toss";
  if (this->maxTicks > 1) {
    cout << "es";
  }
  cout << endl;
}

void Potato::countdown() {
  this->maxTicks -= 1;
  if (maxTicks == 0) {
    throw Potato::Explode();
  }
}

void Umpire::main() {
  // Create potato
  Potato potato(10);

  // Randomly select the first person
  int first = ran(0, this->players->size() - 1);
  Player::PlayerList::const_iterator jt = this->players->begin();
  for (int i = 0; i < first; i++) { jt++; }

  int setNumber = 1;
  cout << "Set " << setNumber << ":\tU";
  (*jt->second).toss(potato);

  // Remove the player that was just eliminated
  this->players->erase(this->current);

  while (this->players->size() != 1) {
    // Toss the potato
    potato.reset(10);

    // Get a random player again
    Player::PlayerList::const_iterator it = this->players->begin();
    int index = ran(0, this->players->size() - 1);
    for (int i = 0; i < index; i++) { it++; }

    setNumber++;
    cout << "Set " << setNumber << ":\tU";
    (*it->second).toss(potato);

    // Remove the player that was just eliminated
    this->players->erase(this->current);
  }

  cout << this->players->begin()->first << " wins the Match!" << endl;
}

void Umpire::set( unsigned int player ) {
  this->current = player;
  resume();
}

void Player::main() {
  while (true) {
    cout << " -> " << this->id;
    try {
      this->potato->countdown();

      // If succeeds, continue.
      // Get player at a random index.
      int index = ran(0, this->players->size() - 1);
      Player::PlayerList::const_iterator it = this->players->begin();
      for (int i = 0; i < index; i++) {
        it++;
      }

      if (it->second == this) {
        it++;
        if (it == this->players->end()) {
          it = this->players->begin();
        }
      }

      // Toss it to a random player
      (*it->second).toss(*potato);

    } catch (Potato::Explode e) {
      cout << " is eliminated" << endl;
      this->umpire->set(this->id);
    }
  }
}

void Player::toss( Potato &potato ) {
  this->potato = &potato;
  resume();
}

void uMain::main() {
  if (argc < 2) {
    cout << "Usage: " << argv[0] << " number-of-players (2 <= and <20) [ random-seed (>= 0) ]" << endl;
    return;
  }

  int numPlayers = stoi(argv[1]);
  int s = getpid();
  if (argc == 3) {
    s = stoi(argv[2]);
  }

  Player::PlayerList players;
  Umpire u(players);

  vector<Player*> vplayers;
  for (int i = 0; i < numPlayers; i++) {
    Player* p = new Player( u, i, players );
    players[i] = p;
    vplayers.push_back(p);
  }
  ran.seed(s);

  cout << numPlayers << " players in the match" << endl;
  u.set(START);

  // Delete every pointer
  for (vector<Player*>::iterator it = vplayers.begin(); it != vplayers.end(); it++) {
    delete (*it);
  }
  vplayers.clear();
}
