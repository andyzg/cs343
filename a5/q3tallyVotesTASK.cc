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

void TallyVotes::main() {
  while (true) {
    count = 0;
    pcount = 0;
    scount = 0;
    bool endLoop = false;
    for (int i = 0; i < group; i++) {
      _Accept(vote) {
      } or _Accept(~TallyVotes) {
        endLoop = true;
        break;
      };
    }
    if (endLoop) {
      break;
    }

    for (int i = 0; i < group; i++) {
      voters.signal();
      groupForming.wait();
    }
  }
}

TallyVotes::Tour TallyVotes::vote(unsigned int id, TallyVotes::Tour ballot) {
  if (ballot == TallyVotes::Tour::Picture) {
    pcount++;
  } else {
    scount++;
  }
  printer->print(id, Voter::States::Vote, ballot);
  printer->print(id, Voter::States::Block, pcount + scount);
  voters.wait();
  printer->print(id, Voter::States::Unblock, group - count - 1);
  count++;

  printer->print(id, Voter::States::Complete);
  groupForming.signal();
  return pcount > scount ? TallyVotes::Tour::Picture : TallyVotes::Tour::Statue;
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
  TallyVotes* tv = new TallyVotes(g, printer);
  Voter* voters[v];

  // Start voting
  for (int i = 0; i < v; i++) {
    voters[i] = new Voter(i, *tv, printer);
  }

  for (int i = 0; i < v; i++) {
    delete voters[i];
  }
  delete tv;
}
