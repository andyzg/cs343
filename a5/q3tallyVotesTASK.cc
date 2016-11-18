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
      // Accept votes or a destructor call to end the voting.
      _Accept(~TallyVotes) {
        // All voters are part of a group now, end.
        endLoop = true;
        break;
      } or _Accept(vote) {
        // Perform a vote, and then print out.
        if (tourType== TallyVotes::Tour::Picture) {
          pcount++;
        } else {
          scount++;
        }
        printer->print(id, Voter::States::Vote, tourType);
        printer->print(id, Voter::States::Block, pcount + scount);
      };
    }

    // Done voting
    if (endLoop) {
      break;
    }

    printer->print(id, Voter::States::Complete);
    for (int i = 0; i < group; i++) {
      count++;
      // Give execution to voter, and then return execution here.
      voters.signalBlock();
    }
  }
}

TallyVotes::Tour TallyVotes::vote(unsigned int id, TallyVotes::Tour ballot) {
  // Transfer these variables back to void main() functions
  tourType = ballot;
  this->id = id;
  voters.wait();

  // Unblocked, return the final result
  printer->print(id, Voter::States::Unblock, group - count);
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
