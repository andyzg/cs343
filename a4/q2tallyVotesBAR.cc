#if defined( IMPLTYPE_BAR )

#include <iostream>
#include <iomanip>
#include "MPRNG2.h"
#include "q2tallyVotes.h"

using namespace std;

static MPRNG r;


// Constructor
TallyVotes::TallyVotes( unsigned int group, Printer &printer) :
  uBarrier(group),
  group(group),
  printer(&printer),
  pcount(0),
  scount(0) {}

TallyVotes::Tour TallyVotes::vote(unsigned int id, TallyVotes::Tour ballot) {
  // Vote for tour
  printer->print(id, Voter::States::Vote, ballot);
  if (ballot == TallyVotes::Tour::Picture) {
    pcount += 1;
  } else {
    scount += 1;
  }

  // Keep track of which task is blocked and which isn't
  bool isBlocked = false;
  if (pcount + scount < total()) {
    isBlocked = true;
    printer->print(id, Voter::States::Block, pcount + scount);
  }

  block();
  // Keep track of how many tasks are unblocked
  count++;
  if (!isBlocked) {
    printer->print(id, Voter::States::Unblock, group - count);
  }

  // Store the final result
  printer->print(id, Voter::States::Complete);

  return pictureGreater ? TallyVotes::Tour::Picture : TallyVotes::Tour::Statue;
}

void TallyVotes::block() {
  uBarrier::block();
}

void TallyVotes::last() {
  // Reset all values after barrier has unblocked
  pictureGreater = pcount > scount;
  pcount = 0;
  scount = 0;
  count = 0;
  resume();
}


Voter::Voter( unsigned int id, TallyVotes &voteTallier, Printer &printer ) :
      id(id), tv(&voteTallier), printer(&printer) {}


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

Printer::Printer( unsigned int voters ) : voters(voters) {
  // States of each voter
  states = new Voter::States[voters];
  // If the voter has any unflushed changed
  dirty = new bool[voters];
  // What the voter voted for
  votes = new TallyVotes::Tour[voters];
  // What tour the voter will go on
  results = new TallyVotes::Tour[voters];
  // How many voters are still blocked
  numBlocked = 0;
  // How many voters are stil blocked after being unblocked
  numUnblocked = 0;

  for (unsigned int i = 0; i < voters; i++) { dirty[i] = false; }

  // Initial printing
  for (unsigned int i = 0; i < voters; i++) {
    cout << "Voter" << i;
    cout.width(7);
  }
  cout << endl;
  for (unsigned int i = 0; i < voters; i++) {
    cout << "======= ";
  }
  cout << endl;
}

Printer::~Printer() {
  // Final printing
  cout << endl << "=================" << endl;
  cout << "All tours started" << endl;

  // Free memory
  delete states;
  delete dirty;
  delete votes;
  delete results;
}

void Printer::main() {}

// Helper function to transform state into character
char printState(Voter::States state) {
  switch (state) {
    case Voter::States::Start:
      return 'S';
    case Voter::States::Vote:
      return 'V';
    case Voter::States::Block:
      return 'B';
    case Voter::States::Unblock:
      return 'U';
    case Voter::States::Barging:
      return 'b';
    case Voter::States::Complete:
      return 'C';
    case Voter::States::Finished:
      return 'F';
    default:
      return ' ';
  }
}

void Printer::flush(unsigned int id) {
  for (unsigned int i = 0; i < voters; i++) {
    if (dirty[i]) {
      // Print value if it is dirty
      if (states[i] == Voter::States::Vote) {
        // Print vote
        cout << "V " << left << setw(6) << setfill(' ')
             << (this->votes[i] == TallyVotes::Tour::Picture ? 'p' : 's');

      } else if (states[i] == Voter::States::Block) {
        // Print Blocked
        cout << "B " << left << setw(6) << setfill(' ') << this->numBlocked;

      } else if (states[i] == Voter::States::Unblock) {
        // Print Unblocked
        cout << "U " << left << setw(6) << setfill(' ') << this->numUnblocked;

      } else {
        // Print everything else
        cout << left << setw(8) << setfill(' ') << printState(states[i]);
      }
    } else {
      cout << left << setw(8) << setfill(' ') << "";
    }
  }

  cout << endl;
  // Reset all dirties
  for (unsigned int i = 0; i < voters; i++) {
    dirty[i] = false;
  }
  this->isDirty = false;
}

void Printer::flushFinish(unsigned int id) {
  for (unsigned int i = 0; i < voters; i++) {
    // Print ... and finishes
    if (i == id) {
      cout << "F " << left << setw(6) << setfill(' ') << (this->results[i] == TallyVotes::Tour::Picture ? 'p' : 's');
    } else {
      cout << left << setw(8) << setfill(' ') << "...";
    }
  }
  cout << endl;

  // Reset all dirties
  for (unsigned int i = 0; i < voters; i++) {
    dirty[i] = false;
  }
  this->isDirty = false;
}

void Printer::print( unsigned int id, Voter::States state ) {
  this->id = id;
  if (this->states[id] != -1 && this->dirty[id]) { flush(id); }
  // Store all new information to be flushed next
  this->states[id] = state;
  this->dirty[id] = true;
  this->isDirty = true;
}

void Printer::print( unsigned int id, Voter::States state, TallyVotes::Tour vote ) {
  this->id = id;
  if ((this->states[id] != -1 && this->dirty[id]) ||
      (state == Voter::States::Finished && isDirty)) {
    flush(id);
  }

  this->states[id] = state;
  if (state == Voter::States::Finished) {
    // Finished voting, print finish
    results[id] = vote;
    flushFinish(id);
  } else if (state == Voter::States::Vote) {
    // Store all new information to be flushed next
    votes[id] = vote;
    this->dirty[id] = true;
  }
  this->isDirty = true;
}

void Printer::print( unsigned int id, Voter::States state, unsigned int numBlocked ) {
  this->id = id;
  if (this->states[id] != -1 && this->dirty[id]) { flush(id); }

  // Store how many are blocked
  if (state == Voter::States::Block) {
    this->numBlocked = numBlocked;
  } else {
    this->numUnblocked = numBlocked;
  }
  // Store all new information to be flushed next
  this->states[id] = state;
  this->dirty[id] = true;
  this->isDirty = true;
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
#endif
