#include <iostream>
#include <iomanip>
#include "MPRNG.h"
#include "q3tallyVotes.h"

using namespace std;

static MPRNG r;


TallyVotes::TallyVotes( unsigned int group, Printer &printer) :
  group(group),
  printer(&printer),
  pcount(0),
  scount(0),
  count(0) {}

TallyVotes::Tour TallyVotes::vote(unsigned int id, TallyVotes::Tour ballot) {
  // Vote for picture or statue
  if (ballot == TallyVotes::Tour::Picture) {
    pcount++;
  } else {
    scount++;
  }
  printer->print(id, Voter::States::Vote, ballot);

  if (pcount + scount < group) {
    printer->print(id, Voter::States::Block, pcount + scount);
    // Accept a vote call which accepts another vote call G-1 times.
    _Accept(vote);
    printer->print(id, Voter::States::Unblock, group - count - 1);
  } else {
    printer->print(id, Voter::States::Complete);
  }

  count++;
  bool pictureGreater = pcount > scount;
  // When all voters have submitted their results, reset counters for a new
  // group
  if (count == group) {
    pcount = 0;
    scount = 0;
    count = 0;
  }

  return pictureGreater ? TallyVotes::Tour::Picture : TallyVotes::Tour::Statue;
}

void Voter::main() {
  yield(r(0, 19));
  // Start
  printer->print(id, Voter::States::Start);

  yield(1);
  // Request for the voting results for a tour.
  TallyVotes::Tour result = r(0,1) == 0 ?
    tv->vote(id, TallyVotes::Tour::Picture) :
    tv->vote(id, TallyVotes::Tour::Statue);

  yield(1);
  // Finished
  printer->print(id, Voter::States::Finished, result);
}


void uMain::main() {
  int v = 6, g = 3, seed = getpid();
  try {
    switch (argc) {
      case 4:
        for (unsigned int i = 0; i < strlen(argv[3]); i++) {
          if (!isdigit(argv[3][i])) { throw 1; }
        }
        seed = atoi(argv[3]);
      case 3:
        for (unsigned int i = 0; i < strlen(argv[2]); i++) {
          if (!isdigit(argv[2][i])) { throw 1; }
        }
        g = atoi(argv[2]);
      case 2:
        for (unsigned int i = 0; i < strlen(argv[1]); i++) {
          if (!isdigit(argv[1][i])) { throw 1; }
        }
        v = atoi(argv[1]);
      default:
        break;
    }

    if (v % g != 0) {
      throw 1;
    }
  } catch (int e) {
    cout << "Usage: " << argv[0] << "  Voters (> 0 & V mod G = 0, default 6)  Group (> 0 & odd, default 3)  Seed (> 0)" << endl;
    return;
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
