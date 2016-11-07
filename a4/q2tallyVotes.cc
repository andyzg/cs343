#include <iostream>
#include "MPRNG2.h"

#if defined( IMPLTYPE_BAR )
#include "uBarrier.h"
#elif defined( IMPLTYPE_SEM )
#include "uSemaphore.h"
#endif

using namespace std;


static MPRNG r;

_Cormonitor Printer;

#if defined( IMPLTYPE_MC )            // mutex/condition solution
// includes for this kind of vote-tallier
class TallyVotes {
  // private declarations for this kind of vote-tallier
  uOwnerLock lock;
  uCondLock bargeLock;
  uCondLock voteLock;

  uCondLock saveLock;
  bool isVoting = false;

  unsigned int count = 0;

#elif defined( IMPLTYPE_SEM )         // semaphore solution
// includes for this kind of vote-tallier
class TallyVotes {
  // private declarations for this kind of vote-tallier
  uSemaphore bargeSem;
  uSemaphore voteSem;
  uSemaphore saveSem;
  uSemaphore endSem;
  unsigned int count = 0;

  bool isVoting = false;

#elif defined( IMPLTYPE_BAR )         // barrier solution
// includes for this kind of vote-tallier
_Cormonitor TallyVotes : public uBarrier {
    // private declarations for this kind of vote-tallier
#else
    #error unsupported voter type
#endif
    // common declarations
    unsigned int group;
    Printer* printer;
    unsigned int pcount;
    unsigned int scount;
  public:                             // common interface
    TallyVotes( unsigned int group, Printer &printer );
    enum Tour { Picture, Statue };
    Tour vote( unsigned int id, Tour ballot );

#if defined( IMPLTYPE_BAR )
    virtual void block();
    virtual void last();
#endif
};

#if defined( IMPLTYPE_MC )
TallyVotes::TallyVotes( unsigned int group, Printer &printer ) : group(group), printer(&printer), pcount(0), scount(0) {}

TallyVotes::Tour TallyVotes::vote(unsigned int id, TallyVotes::Tour ballot) {
  lock.acquire();
  if (isVoting && pcount + scount >= group) {
    bargeLock.wait(lock);
  }

  if (ballot == TallyVotes::Tour::Picture) {
    pcount += 1;
  } else {
    scount += 1;
  }

  if (pcount + scount < group) {
    isVoting = true;
    voteLock.wait(lock);
  }

  bool pictureGreater = pcount > scount;
  count++;
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
  saveSem.P();
  pictureGreater = pcount > scount;
  count++;

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
  if (ballot == TallyVotes::Tour::Picture) {
    pcount += 1;
  } else {
    scount += 1;
  }
  block();
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

_Task Voter {
    int id;
    TallyVotes* tv;
    Printer* printer;
    void main();
  public:
    enum States {
      Start = 'S',
      Vote = 'V',
      Block = 'B',
      Unblock = 'U',
      Barging = 'b',
      Complete = 'C',
      Finished = 'F'
    };
    Voter( unsigned int id, TallyVotes &voteTallier, Printer &printer );
    Voter() = delete;
};


Voter::Voter( unsigned int id, TallyVotes &voteTallier, Printer &printer ) :
      id(id), tv(&voteTallier), printer(&printer) {}


void Voter::main() {
  yield(r(0, 19));
  // cout << "Starting voter " << id << endl;
  yield(1);
  TallyVotes::Tour result = r(0,1) == 0 ?
    tv->vote(id, TallyVotes::Tour::Picture) :
    tv->vote(id, TallyVotes::Tour::Statue);
  yield(1);
  cout << "Finished with " << result << endl;
}

_Cormonitor Printer {      // chose one of the two kinds of type constructor
    int voters;
    void main() {}
  public:
    Printer( unsigned int voters ) : voters(voters) {}
    void print( unsigned int id, Voter::States state ) {}
    void print( unsigned int id, Voter::States state, TallyVotes::Tour vote ) {}
    void print( unsigned int id, Voter::States state, unsigned int numBlocked ) {}
};

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
