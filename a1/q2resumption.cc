#include <iostream>
using namespace std;

_Event H {                                              // uC++ exception type
  public:
    int &i;                                             // pointer to fixup variable at raise
    H( int &i ) : i( i ) {}
};
void f( int &i ) {
    cout << "f " << i << endl;
    if ( rand() % 5 == 0 ) _Resume H( i );              // require correction ?
    try {
        i -= 1;
        if ( 0 < i ) f( i );                            // recursion
    } _CatchResume( H &h ) {
        cout << "f handler " << h.i << endl;
        h.i -= 1;
        f( h.i );
    }
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
        f( times );
    } _CatchResume( H &h ) {
        cout << "root " << h.i << endl;
    }
}
