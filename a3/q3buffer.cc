#include <iostream>

#include "q3buffer.h"
#include "MPRNG.h"

using namespace std;

template<typename T>
BoundedBuffer<T>::BoundedBuffer( const unsigned int size ) : size(size) {
}

template<typename T>
void BoundedBuffer<T>::insert( T elem ) {
}

template<typename T>
T BoundedBuffer<T>::remove() {
  T val = buffer.back();
  buffer.pop_back();
  return val;
}

Producer::Producer( BoundedBuffer<int> &buffer,
                    const int produce,
                    const int delay ) :
    buffer(buffer), produce(produce), delay(delay) {
}

Consumer::Consumer( BoundedBuffer<int> &buffer,
                    const int delay,
                    const int sentinel,
                    int &sum ) :
    buffer(buffer), sentinel(sentinel), delay(delay), sum(sum) {
}

void Producer::main() {
  for (int i = 0; i <= produce; i++) {
    yield(MPRNG()(delay-1));
    buffer.insert(i);
  }
}

void Consumer::main() {
  while (true) {
    yield(MPRNG()(delay-1));
    int next = buffer.remove();
    if (next == sentinel)  {
      break;
    }

    sum += next;
  }
}

void uMain::main() {
  int cons = 5, prods = 3, produce = 10, bufferSize = 10, delays;
  switch ( argc ) {
    case 6:
      delays = atoi( argv[5] );
    case 5:
      bufferSize = atoi( argv[4] );
    case 4:
      produce = atoi( argv[3] );
    case 3:
      prods = atoi( argv[2] );
    case 2:
      cons = atoi( argv[1] );
      break;
  } // switch
  if (argc != 6) {
    delays = cons + prods;
  }

  cout << "Hello World!" << endl;
}
