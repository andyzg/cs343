#include <cstdlib>                    // atoi
#include <iostream>
using namespace std;

#define NOOUTPUT

// volatile prevents dead-code removal
void do_work( int C1, int C2, int C3, int L1, int L2, volatile int L3 ) {
#ifndef NOOUTPUT
  cout << L1 << " " << L2 << " " << L3 << " - " << C1 << " " << C2 << " " << C3 << endl;
#endif
  for ( int i = 0; i < L1; i += 1 ) {
#ifndef NOOUTPUT
    cout << "S1 i:" << i << endl;
#endif
    for ( int j = 0; j < L2; j += 1 ) {
#ifndef NOOUTPUT
      cout << "S2 i:" << i << " j:" << j << endl;
#endif
      for ( int k = 0; k < L3; k += 1 ) {
#ifndef NOOUTPUT
        cout << "S3 i:" << i << " j:" << j << " k:" << k << " : ";
#endif
        if ( C1 ) goto EXIT1;
#ifndef NOOUTPUT
        cout << "S4 i:" << i << " j:" << j << " k:" << k << " : ";
#endif
        if ( C2 ) goto EXIT2;
#ifndef NOOUTPUT
        cout << "S5 i:" << i << " j:" << j << " k:" << k << " : ";
#endif
        if ( C3 ) goto EXIT3;
#ifndef NOOUTPUT
        cout << "S6 i:" << i << " j:" << j << " k:" << k << " : ";
#endif
      } // for
      EXIT3:;
#ifndef NOOUTPUT
      cout << "S7 i:" << i << " j:" << j << endl;
#endif
    } // for
    EXIT2:;
#ifndef NOOUTPUT
    cout << "S8 i:" << i << endl;
#endif
  } // for
  EXIT1:;
} // do_work

// volatile prevents dead-code removal
void do_work3( int C1, int C2, int C3, int L1, int L2, volatile int L3 ) {
#ifndef NOOUTPUT
  cout << L1 << " " << L2 << " " << L3 << " - " << C1 << " " << C2 << " " << C3 << endl;
#endif
  int i = 0;
  int j = 0;
  int k = 0;

  int level = 4;
  int J1 = 0;
  int J2 = 0;
  int J3 = 0;

  J1 = 0;
  while ( (J1 | i) < L1 ) {
#ifndef NOOUTPUT
    cout << "S1 i:" << i << endl;
#endif
    J2 = 0;
    j = 0;
    while ( (J2 | j) < L2 ) {
#ifndef NOOUTPUT
      cout << "S2 i:" << i << " j:" << j << endl;
#endif
      J3 = 0;
      k = 0;
      while ( (J3 | k) < L3 ) {
        level = 4;
#ifndef NOOUTPUT
        cout << "S3 i:" << i << " j:" << j << " k:" << k << " : ";
#endif
        if ( C1 ) {
          level = 1;
          J1 = L1;
          J2 = L2;
          J3 = L3;
          // goto EXIT1;
        } else {
#ifndef NOOUTPUT
          cout << "S4 i:" << i << " j:" << j << " k:" << k << " : ";
#endif
          if ( C2 ) {
            level = 2;
            J2 = L2;
            J3 = L3;
            // goto EXIT2;
          } else {
#ifndef NOOUTPUT
            cout << "S5 i:" << i << " j:" << j << " k:" << k << " : ";
#endif
            if ( C3 ) {
              level = 3;
              J3 = L3;
              // goto EXIT3;
            } else {
#ifndef NOOUTPUT
              cout << "S6 i:" << i << " j:" << j << " k:" << k << " : ";
#endif
              k++;
            }
          }
        }
      }
      if (level >= 3) { // Disable for 1 and 2
        // EXIT3:;
#ifndef NOOUTPUT
        cout << "S7 i:" << i << " j:" << j << endl;
#endif
        j++;
      }
    }
    if (level >= 2) { // Disable for 1
      // EXIT2:;
#ifndef NOOUTPUT
      cout << "S8 i:" << i << endl;
#endif
      i++;
    }
  }
  // EXIT1:;
}

// volatile prevents dead-code removal
void do_work2( int C1, int C2, int C3, int L1, int L2, volatile int L3 ) {
#ifndef NOOUTPUT
  cout << L1 << " " << L2 << " " << L3 << " - " << C1 << " " << C2 << " " << C3 << endl;
#endif
  int flag1 = L1 == 0 ? 0 : 1;
  int flag2 = L2 == 0 ? 0 : 1;
  int flag3 = L3 == 0 ? 0 : 1;
  int exitNum = 0;

  int i = 0;
  int j = 0;
  int k = 0;

  while (flag1) {
    flag2 = 1;
#ifndef NOOUTPUT
    cout << "S1 i:" << i << endl;
#endif
    while (flag2) {
      flag3 = 1;
#ifndef NOOUTPUT
      cout << "S2 i:" << i << " j:" << j << endl;
#endif
      while (flag3) {
        exitNum = 0;
#ifndef NOOUTPUT
        cout << "S3 i:" << i << " j:" << j << " k:" << k << " : ";
#endif

        if ( !C1 ) {
#ifndef NOOUTPUT
          cout << "S4 i:" << i << " j:" << j << " k:" << k << " : ";
#endif

          if ( !C2 ) {
#ifndef NOOUTPUT
            cout << "S5 i:" << i << " j:" << j << " k:" << k << " : ";
#endif

            if ( !C3 ) {
#ifndef NOOUTPUT
              cout << "S6 i:" << i << " j:" << j << " k:" << k << " : ";
#endif
            } else {
              flag3 = 0;
              exitNum = 3;
            }

          } else {
            flag2 = 0;
            flag3 = 0;
            exitNum = 2;
          }

        } else {
          flag1 = 0;
          flag2 = 0;
          flag3 = 0;
          exitNum = 1;
        }

        if (++k >= L3-1) {
          flag3 = 0;
        }

      }

      if (exitNum >= 3) {
#ifndef NOOUTPUT
        cout << "S7 i:" << i << " j:" << j << endl;
#endif
        exitNum--;
      }
      if (++j >= L2-1) {
        flag2 = 0;
      }

    }
    if (exitNum >= 2) {
#ifndef NOOUTPUT
      cout << "S8 i:" << i << endl;
#endif
    }

    if (++i >= L1-1) {
      flag1 = 0;
    }
  }
}


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
#ifndef NOOUTPUT
#ifndef NOOUTPUT
                cout << "S1 i:" << i << endl;
#endif
#endif
#ifndef NOOUTPUT
#ifndef NOOUTPUT
                cout << "S2 i:" << i << " j:" << j << endl;
#endif
#endif
#ifndef NOOUTPUT
#ifndef NOOUTPUT
                cout << "S3 i:" << i << " j:" << j << " k:" << k << " : ";
#endif
#endif
                if ( !C1 ) {
#ifndef NOOUTPUT
#ifndef NOOUTPUT
                    cout << "S4 i:" << i << " j:" << j << " k:" << k << " : ";
#endif
#endif
                    if ( !C2 ) {
#ifndef NOOUTPUT
#ifndef NOOUTPUT
                        cout << "S5 i:" << i << " j:" << j << " k:" << k << " : ";
#endif
#endif
                        if ( !C3 ) {
#ifndef NOOUTPUT
#ifndef NOOUTPUT
                            cout << "S6 i:" << i << " j:" << j << " k:" << k << " : ";
#endif
#endif
                        } else {
                          l3Flag = false;
                        }
#ifndef NOOUTPUT
#ifndef NOOUTPUT
                        cout << "S7 i:" << i << " j:" << j << endl;
#endif
#endif
                    } else {
                        l2Flag = false;
                    }
#ifndef NOOUTPUT
#ifndef NOOUTPUT
                    cout << "S8 i:" << i << endl;
#endif
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
                    do_work3( C1, C2, C3, L1, L2, L3 );
                    cout << endl;
                } // for
            } // for
        } // for
    } // for
} // main
