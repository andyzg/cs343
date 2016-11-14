#include <iostream>
#include <iomanip>
#include "MPRNG.h"
#include "q2tallyVotes.h"

using namespace std;

static MPRNG r;


// Constructor TODO
TallyVotes::TallyVotes( unsigned int group, Printer &printer) :
  uBarrier(group),
  group(group),
  printer(&printer),
  pcount(0),
  scount(0) {}

// Vote TODO
TallyVotes::Tour TallyVotes::vote(unsigned int id, TallyVotes::Tour ballot) {
  return TallyVotes::Tour::Picture;
}


void uMain::main() {
  int v = 6, g = 3, seed = getpid();
  switch (argc) {
    case 4:
      seed = atoi(argv[3]);
    case 3:
      g = atoi(argv[2]);
    case 2:
      v = atoi(argv[1]);
    default:
      break;
  }

  // Initialize everything
  r.set_seed(seed);
  Printer printer(v);
  TallyVotes tv(g, printer);
  Voter* voters[v];

  // Start voting
  for (int i = 0; i < v; i++) {
    voters[i] = new Voter(i, tv, printer);
  }

  for (int i = 0; i < v; i++) {
    delete voters[i];
  }
}
