#include <iostream>
#include <iomanip>
#include "MPRNG.h"
#include "q3tallyVotes.h"

using namespace std;

static MPRNG r;


TallyVotes::TallyVotes( unsigned int group, Printer &printer) :
  isVoting(false),
  generation(0),
  group(group),
  printer(&printer),
  pcount(0),
  scount(0),
  count(0) {}

TallyVotes::Tour TallyVotes::vote(unsigned int id, TallyVotes::Tour ballot) {
  int myGen = generation;
  // Prevent barging
  if (isVoting) {
    printer->print(id, Voter::States::Barging);
    // In case if it randomly wakes up
    while (myGen == generation) {
      wait();
    }
  }

  // Vote for picture or statue
  if (ballot == TallyVotes::Tour::Picture) {
    pcount++;
  } else {
    scount++;
  }
  printer->print(id, Voter::States::Vote, ballot);

  if (pcount + scount < group) {
    // If not enough people, wait
    printer->print(id, Voter::States::Block, pcount + scount);
    wait();
    printer->print(id, Voter::States::Unblock, group - count - 1);
  } else {
    // Prevent barging, wake everyone up
    isVoting = true;
    signalAll();
    printer->print(id, Voter::States::Complete);
  }

  count++;
  bool pictureGreater = pcount > scount;
  // Last person to leave resets counters, disables flag and wakes up all voters
  // waiting.
  if (count == group) {
    isVoting = false;
    generation++;
    signalAll();
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
