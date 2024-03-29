#include <iostream>
#include <cstdlib>
#include <unistd.h>        // getpid
#include <setjmp.h>
#include <cstring>
using namespace std;

#define NOOUTPUT
#ifdef NOOUTPUT
#define print( x )
#else
#define print( x ) x
#endif

static jmp_buf env;

struct E {};

long int freq = 5;

long int Ackermann( long int m, long int n ) {
  jmp_buf tempenv;
  if ( m == 0 ) {
    if ( random() % freq == 0 ) {
      longjmp(env, 1);
    }
    return n + 1;
  } else if ( n == 0 ) { // m == 0, n == 0
    if ( random() % freq == 0 ) {
      longjmp(env, 1);
    }

    if (setjmp(env)) {
			print( cout << "E1 " << m << " " << n << endl );
    } else {
      return Ackermann( m - 1, 1 );
    }

  } else {

    if (setjmp(env)) {
      print( cout << "E2 " << m << " " << n << endl );

    } else {
      memcpy(tempenv, env, sizeof(env));
      long int firstAckermann = Ackermann( m, n - 1 );
      memcpy(env, tempenv, sizeof(tempenv));
      return Ackermann( m - 1,  firstAckermann );
    }

  } // if
  return 0;  // recover by returning 0
}

int main( int argc, const char *argv[] ) {
  long int Ackermann( long int m, long int n );
  long int m = 4, n = 6, seed = getpid();  // default values

  switch ( argc ) {
    case 5: freq = atoi( argv[4] );
    case 4: seed = atoi( argv[3] );
    case 3: n = atoi( argv[2] );
    case 2: m = atoi( argv[1] );
  } // switch
  srandom( seed );
  cout << m << " " << n << " " << seed << " " << freq << endl;

  if (setjmp(env)) {
    print( cout << "E3" << endl );
  } else {
    cout << Ackermann( m, n ) << endl;
  }
}
