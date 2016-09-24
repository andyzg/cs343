#include <iostream>
using namespace std;

void f( int &i );

class HandlerFunctor {
  public:
    HandlerFunctor() {}
    void operator() (H &h) {}
};

class FHandlerFunctor : HandlerFunctor {
  public:
    void operator() (H &h) {
      cout << "f handler " << h.i << endl;
      h.i -= 1;
      f(h.i);
    }
};

class RootFunctor : HandlerFunctor {
  public:
    void operator() (H &h) {
      cout << "root " << h.i << endl;
    }
};

_Event H {                                              // uC++ exception type
  public:
    int &i;                                             // pointer to fixup variable at raise
    H( int &i ) : i( i ) {}
};
void f( int &i , HandlerFunctor functor ) {
    cout << "f " << i << endl;
    if ( rand() % 5 == 0 ) functor( i );                // require correction ?
      i -= 1;
      if ( 0 < i ) f( i, new FHandlerFunctor() );                            // recursion
}
void uMain::main() {
    int times = 25, seed = getpid();
    switch ( argc ) {
      case 3: seed = atoi( argv[2] );                   // allow repeatable experiment
      case 2: times = atoi( argv[1] );                  // control recursion depth
      case 1: break;                                    // defaults
      default: cerr << "Usage: " << argv[0] << " times seed" << endl; exit( EXIT_FAILURE );
    }
    srand( seed );                                      // fixed or random seed
    try {
        f( times , new RootFunctor() );
    } _CatchResume( H &h ) {
        cout << "root " << h.i << endl;
    }
}
