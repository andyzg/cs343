#include <iostream>
#include <cstdlib>
#include <unistd.h>        // getpid

// Use stack instead of passing as routine
using namespace std;

class HandlerFunctor;
void f( int &i , HandlerFunctor* functor );

class HandlerFunctor {
  public:
    HandlerFunctor() {}
    virtual void operator() (int &i) {}
};

class FHandlerFunctor : public HandlerFunctor {
  public:
    void operator() (int &i) {
      cout << "f handler " << i << endl;
      i -= 1;
      if ( 0 < i ) {
        HandlerFunctor* functor = new FHandlerFunctor();
        f(i, functor);
      }
    }
};

class RootFunctor : public HandlerFunctor {
  public:
    void operator() (int &i) {
      cout << "root " << i << endl;
    }
};

void f( int &i, HandlerFunctor* functor ) {
    cout << "f " << i << endl;
    if ( rand() % 5 == 0 ) {
      (*functor)( i );
      return;
    }
    i -= 1;
    if ( 0 < i ) {
      HandlerFunctor* newFunctor = new FHandlerFunctor();
      f( i, newFunctor );
    }
}

int main( int argc, const char *argv[] ) {
    int times = 25, seed = getpid();
    switch ( argc ) {
      case 3: seed = atoi( argv[2] );                   // allow repeatable experiment
      case 2: times = atoi( argv[1] );                  // control recursion depth
      case 1: break;                                    // defaults
      default: cerr << "Usage: " << argv[0] << " times seed" << endl; exit( EXIT_FAILURE );
    }
    srand( seed );                                      // fixed or random seed
    HandlerFunctor* functor = new RootFunctor();
    f( times , functor );
}
