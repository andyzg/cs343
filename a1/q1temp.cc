#include <iostream>
#include <cstdlib>
#include <unistd.h>        // getpid
#include <setjmp.h>

using namespace std;

#ifdef NOOUTPUT
#define print( x )
#else
#define print( x ) x
#endif

struct EnvNode {
  jmp_buf env;
  EnvNode* next;
};

static EnvNode* env;

struct E {};

long int freq = 5;

long int Ackermann( long int m, long int n ) {
  if ( m == 0 ) {
    if ( random() % freq == 0 ) {
      longjmp(env->env, 1); // GOTO 52
    }
    return n + 1;
  } else if ( n == 0 ) { // m == 0, n == 0
    if ( random() % freq == 0 ) {
      longjmp(env->env, 1); // GOTO 52
    }

    EnvNode* newEnv = new EnvNode();
    newEnv->next = env;
    env = newEnv;
    int val = setjmp(newEnv->env);
    if (val) {
			print( cout << "E1 " << m << " " << n << endl );
      env = env->next;
      longjmp(env->env, 1);
    }
    return Ackermann( m - 1, 1 );

  } else {

    EnvNode* newEnv = new EnvNode();
    newEnv->next = env;
    env = newEnv;
    int val2 = setjmp(env->env);
    if (val2) {
      print( cout << "E2 " << m << " " << n << endl );
      env = env->next;
      longjmp(env->env, 1);
    }

    long int firstAckermann = Ackermann( m, n - 1 );
    env = newEnv;
    return Ackermann( m - 1,  firstAckermann );

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

  env = new EnvNode();
  env->next = NULL;
  int val = setjmp(env->env);
  if (val) {
    print( cout << "E3" << endl );
    return 0;
  }

  cout << Ackermann( m, n ) << endl;
}
