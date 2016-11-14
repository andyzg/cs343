void TallyVotes::wait() {
    bench.wait();                      // wait until signalled
    while ( rand() % 5 == 0 ) {        // multiple bargers allowed
        _Accept( vote ) {              // accept barging callers
        } _Else {                      // do not wait if no callers
        } // _Accept
    } // while
}

void TallyVotes::signalAll() {
    while ( ! bench.empty() ) bench.signal();\(}\Tab{44}{\)// drain the condition
}

_Cormonitor Printer;

#if defined( IMPLTYPE_EXT )            // external scheduling monitor solution
// includes for this kind of vote-tallier
_Monitor TallyVotes {
    // private declarations for this kind of vote-tallier
#elif defined( IMPLTYPE_INT )          // internal scheduling monitor solution
// includes for this kind of vote-tallier
_Monitor TallyVotes {
    // private declarations for this kind of vote-tallier
#elif defined( IMPLTYPE_INTB )         // internal scheduling monitor solution with barging
// includes for this kind of vote-tallier
_Monitor TallyVotes {
    // private declarations for this kind of vote-tallier
    uCondition bench;                  // only one condition variable (you may change the variable name)
    void wait();                       // barging version of wait
    void signalAll();                  // unblock all waiting tasks
#elif defined( IMPLTYPE_AUTO )         // automatic-signal monitor solution
// includes for this kind of vote-tallier
_Monitor TallyVotes {
    // private declarations for this kind of vote-tallier
#elif defined( IMPLTYPE_TASK )         // internal/external scheduling task solution
_Task TallyVotes {
    // private declarations for this kind of vote-tallier
#else
    #error unsupported voter type
#endif
    // common declarations
  public:                              // common interface
    TallyVotes( unsigned int group, Printer &printer );
    enum Tour { Picture, Statue };
    Tour vote( unsigned int id, Tour ballot );
};

/************** BEGIN VOTER *******/
_Task Voter {
  public:
    enum States { Start = 'S', Vote = 'V', Block = 'B', Unblock = 'U', Barging = 'b',
                   Complete = 'C', Finished = 'F' };
    Voter( unsigned int id, TallyVotes &voteTallier, Printer &printer );
};

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

/************ END VOTER ***********/


/********* BEGIN PRINTER  ***********/
_Cormonitor Printer {       // chose one of the two kinds of type constructor
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
    void print( unsigned int id, Voter::States state );
    void print( unsigned int id, Voter::States state, TallyVotes::Tour vote );
    void print( unsigned int id, Voter::States state, unsigned int numBlocked );
};

Printer::Printer( unsigned int voters ) : voters(voters) {
  // States of each voter
  states = new Voter::States[voters];
  // If the voter has any unflushed changed
  dirty = new bool[voters]; // What the voter voted for
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
    this->isDirty = true;
  }
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

/********* END PRINTER  ***********/
