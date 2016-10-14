#include <cstdlib>                    // atoi
#include <iostream>
using namespace std;

// volatile prevents dead-code removal
void do_work( int C1, int C2, int C3, int L1, int L2, volatile int L3 ) {
    for ( int i = 0; i < L1; i += 1 ) {
#ifndef NOOUPUT
        cout << "S1 i:" << i << endl;
#endif
        for ( int j = 0; j < L2; j += 1 ) {
#ifndef NOOUPUT
            cout << "S2 i:" << i << " j:" << j << endl;
#endif
            for ( int k = 0; k < L3; k += 1 ) {
#ifndef NOOUPUT
                cout << "S3 i:" << i << " j:" << j << " k:" << k << " : ";
#endif
      if ( C1 ) goto EXIT1;
#ifndef NOOUPUT
                cout << "S4 i:" << i << " j:" << j << " k:" << k << " : ";
#endif
          if ( C2 ) goto EXIT2;
#ifndef NOOUPUT
                cout << "S5 i:" << i << " j:" << j << " k:" << k << " : ";
#endif
              if ( C3 ) goto EXIT3;
#ifndef NOOUPUT
                cout << "S6 i:" << i << " j:" << j << " k:" << k << " : ";
#endif
            } // for
          EXIT3:;
#ifndef NOOUPUT
            cout << "S7 i:" << i << " j:" << j << endl;
#endif
        } // for
      EXIT2:;
#ifndef NOOUPUT
        cout << "S8 i:" << i << endl;
#endif
    } // for
  EXIT1:;
} // do_work

int main( int argc, char *argv[] ) {
    int times = 1, L1 = 10, L2 = 10, L3 = 10;
    switch ( argc ) {
      case 5:
        L3 = atoi( argv[4] );
        L2 = atoi( argv[3] );
        L1 = atoi( argv[2] );
        times = atoi( argv[1] );
        break;
      default:
        cerr << "Usage: " << argv[0] << " times L1 L2 L3" << endl;
        exit( EXIT_FAILURE );
    } // switch

    for ( int i = 0; i < times; i += 1 ) {
        for ( int C1 = 0; C1 < 2; C1 += 1 ) {
            for ( int C2 = 0; C2 < 2; C2 += 1 ) {
                for ( int C3 = 0; C3 < 2; C3 += 1 ) {
                    do_work( C1, C2, C3, L1, L2, L3 );
#ifndef NOOUPUT
                    cout << endl;
#endif
                } // for
            } // for
        } // for
    } // for
} // main
