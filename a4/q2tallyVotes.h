#if defined( IMPLTYPE_BAR )
#include "uBarrier.h"
#elif defined( IMPLTYPE_SEM )
#include "uSemaphore.h"
#endif

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

_Cormonitor Printer {      // chose one of the two kinds of type constructor
    unsigned int voters;
    Voter::States* states;
    bool* dirty;
    bool isDirty;

    TallyVotes::Tour* votes;
    TallyVotes::Tour* results;

    // State to be passed
    unsigned id;
    unsigned int numBlocked;
    unsigned int numUnblocked;

    void main();
    void flush(unsigned int id);
    void flushFinish(unsigned int id);
  public:
    Printer( unsigned int voters );
    ~Printer();
    void print( unsigned int id, Voter::States state );
    void print( unsigned int id, Voter::States state, TallyVotes::Tour vote );
    void print( unsigned int id, Voter::States state, unsigned int numBlocked );
};
