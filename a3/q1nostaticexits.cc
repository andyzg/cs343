#include <cstdlib>                    // atoi
#include <iostream>
using namespace std;

// volatile prevents dead-code removal
void do_work1( int C1, int C2, int C3, int L1, int L2, volatile int L3 ) {
    bool l1Flag = true;
    bool l2Flag = true;
    bool l3Flag = true;

    int i = 0;
    int j = 0;
    int k = 0;
    while ( l1Flag ) {
        l2Flag = true;
        j = 0;
        while ( l2Flag ) {
            l3Flag = true;
            k = 0;
            while ( l3Flag ) {
#ifdef NOOUTPUT
                cout << "S1 i:" << i << endl;
#endif
#ifdef NOOUTPUT
                cout << "S2 i:" << i << " j:" << j << endl;
#endif
#ifdef NOOUTPUT
                cout << "S3 i:" << i << " j:" << j << " k:" << k << " : ";
#endif
                if ( !C1 ) {
#ifdef NOOUTPUT
                    cout << "S4 i:" << i << " j:" << j << " k:" << k << " : ";
#endif
                    if ( !C2 ) {
#ifdef NOOUTPUT
                        cout << "S5 i:" << i << " j:" << j << " k:" << k << " : ";
#endif
                        if ( !C3 ) {
#ifdef NOOUTPUT
                            cout << "S6 i:" << i << " j:" << j << " k:" << k << " : ";
#endif
                        } else {
                          l3Flag = false;
                        }
#ifdef NOOUTPUT
                        cout << "S7 i:" << i << " j:" << j << endl;
#endif
                    } else {
                        l2Flag = false;
                    }
#ifdef NOOUTPUT
                    cout << "S8 i:" << i << endl;
#endif
                } else {
                    l1Flag = false;
                }
                if ( ++k == L3 ) {
                  l3Flag = false;
                }
            }
            if ( ++j == L2 ) {
              l2Flag = false;
            }
        }

        if ( ++i == L1 ) {
          l1Flag = false;
        }
    }
}

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
                    cout << endl;
                } // for
            } // for
        } // for
    } // for
} // main
