#include <iostream>
#include <iomanip>
#include "MPRNG2.h"
#include "q2tallyVotes.h"

using namespace std;

static MPRNG r;


#if defined( IMPLTYPE_MC )
TallyVotes::TallyVotes( unsigned int group, Printer &printer ) : group(group), printer(&printer), pcount(0), scount(0) {}

TallyVotes::Tour TallyVotes::vote(unsigned int id, TallyVotes::Tour ballot) {
  lock.acquire();
  if (isVoting && pcount + scount >= group) {
    bargeLock.wait(lock);
  }
  printer->print(id, Voter::States::Vote, ballot);

  if (ballot == TallyVotes::Tour::Picture) {
    pcount += 1;
  } else {
    scount += 1;
  }

  if (pcount + scount < group) {
    printer->print(id, Voter::States::Block, pcount + scount);
    isVoting = true;
    voteLock.wait(lock);
    printer->print(id, Voter::States::Unblock, group - count - 1);
  }

  count++;
  printer->print(id, Voter::States::Complete);
  bool pictureGreater = pcount > scount;
  voteLock.signal();
  if (count < group) {
    saveLock.wait(lock);
  } else {
    pcount = 0;
    scount = 0;
    count = 0;
    saveLock.broadcast();
  }
  lock.release();

  isVoting = false;
  if (!bargeLock.empty()) {
    bargeLock.signal();
  }
  return pictureGreater ? TallyVotes::Tour::Picture : TallyVotes::Tour::Statue;
}

#elif defined( IMPLTYPE_SEM )
TallyVotes::TallyVotes( unsigned int group, Printer &printer) : bargeSem(0), voteSem(1), saveSem(0), endSem(0), group(group), printer(&printer), pcount(0), scount(0) {}

TallyVotes::Tour TallyVotes::vote(unsigned int id, TallyVotes::Tour ballot) {
  if (pcount + scount >= group && isVoting) {
    bargeSem.P();
  }

  voteSem.P();
  isVoting = true;
  printer->print(id, Voter::States::Vote, ballot);
  if (ballot == TallyVotes::Tour::Picture) {
    pcount += 1;
  } else {
    scount += 1;
  }

  bool unlocker = false;
  if (pcount + scount == group) {
    saveSem.V();
    unlocker = true;
  }

  voteSem.V();

  bool pictureGreater;
  if (!unlocker) {
    printer->print(id, Voter::States::Block, pcount + scount);
  }
  saveSem.P();
  count++;
  if (!unlocker) {
    printer->print(id, Voter::States::Unblock, group - count - 1);
  }
  printer->print(id, Voter::States::Complete);
  pictureGreater = pcount > scount;

  if (!unlocker) {
    saveSem.V();
  }

  if (count == group) {
    count = 0;
    pcount = 0;
    scount = 0;
    isVoting = false;

    for (unsigned int i = 0; i < group && bargeSem.counter() < 0; i++) { bargeSem.V(); }
  }

  return pictureGreater ? TallyVotes::Tour::Picture : TallyVotes::Tour::Statue;
}

#elif defined( IMPLTYPE_BAR )
TallyVotes::TallyVotes( unsigned int group, Printer &printer) : uBarrier(group), group(group), printer(&printer), pcount(0), scount(0) { }

TallyVotes::Tour TallyVotes::vote(unsigned int id, TallyVotes::Tour ballot) {
  printer->print(id, Voter::States::Vote, ballot);
  if (ballot == TallyVotes::Tour::Picture) {
    pcount += 1;
  } else {
    scount += 1;
  }

  bool isBlocked = false;
  if (pcount + scoun < total()) {
    isBlocked = true;
    printer->print(id, Voter::States::Block, pcount + scount);
  }
  block();
  count++;
  if (!isBlocked) {
    printer->print(id, Voter::States::Unblock, group - count);
  }
  printer->print(id, Voter::States::Complete);
  if (scount > pcount) {
    return TallyVotes::Statue;
  } else {
    return TallyVotes::Picture;
  }
}

void TallyVotes::block() {
  uBarrier::block();
}

void TallyVotes::last() {}

#else
  #error unsupported voter type
#endif


Voter::Voter( unsigned int id, TallyVotes &voteTallier, Printer &printer ) :
      id(id), tv(&voteTallier), printer(&printer) {}


void Voter::main() {
  yield(r(0, 19));
  printer->print(id, Voter::States::Start);
  yield(1);
  TallyVotes::Tour result = r(0,1) == 0 ?
    tv->vote(id, TallyVotes::Tour::Picture) :
    tv->vote(id, TallyVotes::Tour::Statue);
  yield(1);
  printer->print(id, Voter::States::Finished, result);
}

Printer::Printer( unsigned int voters ) : voters(voters) {
  states = new Voter::States[voters];
  dirty = new bool[voters];
  votes = new TallyVotes::Tour[voters];
  results = new TallyVotes::Tour[voters];
  numBlocked = 0;
  numUnblocked = 0;

  for (unsigned int i = 0; i < voters; i++) { dirty[i] = false; }

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
  cout << endl << "=================" << endl;
  cout << "All tours started" << endl;

  delete states;
  delete dirty;
  delete votes;
  delete results;
}

void Printer::main() {}

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
      if (states[i] == Voter::States::Vote) {
        cout << "V " << left << setw(6) << setfill(' ') << (this->votes[i] == TallyVotes::Tour::Picture ? 'p' : 's');
      } else if (states[i] == Voter::States::Block) {
        cout << "B " << left << setw(6) << setfill(' ') << this->numBlocked;
      } else if (states[i] == Voter::States::Unblock) {
        cout << "U " << left << setw(6) << setfill(' ') << this->numUnblocked;
      } else {
        cout << left << setw(8) << setfill(' ') << printState(states[i]);
      }
    } else {
      cout << left << setw(8) << setfill(' ') << "";
    }
  }

  cout << endl;
  for (unsigned int i = 0; i < voters; i++) {
    dirty[i] = false;
  }
  this->isDirty = false;
}

void Printer::flushFinish(unsigned int id) {
  for (unsigned int i = 0; i < voters; i++) {
    if (i == id) {
      cout << "F " << left << setw(6) << setfill(' ') << (this->results[i] == TallyVotes::Tour::Picture ? 'p' : 's');
    } else {
      cout << left << setw(8) << setfill(' ') << "...";
    }
  }
  cout << endl;
  for (unsigned int i = 0; i < voters; i++) {
    dirty[i] = false;
  }
  this->isDirty = false;
}

void Printer::print( unsigned int id, Voter::States state ) {
  this->id = id;
  if (this->states[id] != -1 && this->dirty[id]) { flush(id); }
  this->states[id] = state;
  this->dirty[id] = true;
  this->isDirty = true;
}

void Printer::print( unsigned int id, Voter::States state, TallyVotes::Tour vote ) {
  this->id = id;
  if (this->states[id] != -1 && this->dirty[id]) {
    flush(id);
  } else if (state == Voter::States::Finished && isDirty) {
    flush(id);
  }

  this->states[id] = state;
  if (state == Voter::States::Finished) {
    results[id] = vote;
    flushFinish(id);
  } else if (state == Voter::States::Vote) {
    votes[id] = vote;
    this->dirty[id] = true;
  }
  this->isDirty = true;
}

void Printer::print( unsigned int id, Voter::States state, unsigned int numBlocked ) {
  this->id = id;
  if (this->states[id] != -1 && this->dirty[id]) { flush(id); }
  if (state == Voter::States::Block) {
    this->numBlocked = numBlocked;
  } else {
    this->numUnblocked = numBlocked;
  }
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

  r.set_seed(seed);
  Printer printer(v);
  TallyVotes tv(g, printer);
  Voter* voters[v];
  for (int i = 0; i < v; i++) {
    voters[i] = new Voter(i, tv, printer);
  }

  for (int i = 0; i < v; i++) {
    delete voters[i];
  }
}
