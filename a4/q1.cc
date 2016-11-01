#include <vector>
#include <memory>                // unique_ptr
#include <cstdlib>               // atoi
using namespace std;

int tasks = 1, times = 10000000;

_Task Worker {
    enum { size = 100 };
    void main() {
        for ( int i = 0; i < times; i += 1 ) {
#if defined( IMPLKIND_DARRAY )
            unique_ptr<volatile int []> arr( new volatile int[size] );
            for ( int i = 0; i < size; i += 1 ) arr[i] = i;
#elif defined( IMPLKIND_VECTOR1 )
            vector<int> arr( size );
            for ( int i = 0; i < size; i += 1 ) arr.at(i) = i;
#elif defined( IMPLKIND_VECTOR2 )
            vector<int> arr;
            for ( int i = 0; i < size; i += 1 ) arr.push_back(i);
#else // STACK ARRAY
            volatile int arr[size] __attribute__ (( unused )); // prevent unused warning
            for ( int i = 0; i < size; i += 1 ) arr[i] = i;
#endif
        }
    }
};

void uMain::main() {
    switch ( argc ) {
      case 3:
         times = atoi( argv[2] );
      case 2:
         tasks = atoi( argv[1] );
    }
    uProcessor p[tasks - 1];
    Worker workers[tasks];
}
