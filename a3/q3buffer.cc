#include <iostream>
#include <unistd.h>

#include "q3buffer.h"
#include "MPRNG.h"

using namespace std;

template<typename T>
BoundedBuffer<T>::BoundedBuffer( const unsigned int size ) : size(size), signalFlag(false) {}

template<typename T>
void BoundedBuffer<T>::insert( T elem ) {
#ifdef BUSY
  owner.acquire();
  while (buffer.size() == size) {
    insertLock.wait(owner);
  }
  buffer.push(elem);
  removeLock.signal();
  owner.release();
#endif

#ifdef NOBUSY
  owner.acquire();
  if (signalFlag) {
    taskLock.wait(owner);
  }
  if (buffer.size() == size) {
    insertLock.wait(owner);
  }
  buffer.push(elem);
  taskLock.signal();
  signalFlag = true;
  removeLock.signal();
  owner.release();
  signalFlag = false;
#endif
}

template<typename T>
T BoundedBuffer<T>::remove() {
#ifdef NOBUSY
  owner.acquire();
  if (signalFlag) {
    taskLock.wait(owner);
  }
  if (buffer.size() == 0) {
    removeLock.wait(owner);
  }
  T val = buffer.front();
  if (!buffer.size() == 0) {
    buffer.pop();
  } else {
    val = 0;
  }
  taskLock.signal();
  signalFlag = true;
  insertLock.signal();
  owner.release();
  signalFlag = false;
  return val;
#endif

#ifdef BUSY
  owner.acquire();
  while (buffer.size() == 0) {
    removeLock.wait(owner);
  }
  T val = buffer.front();
  buffer.pop();
  signalFlag = true;
  insertLock.signal();
  owner.release();
  signalFlag = false;
  return val;
#endif
}

Producer::Producer( BoundedBuffer<int> &buffer,
                    const int produce,
                    const int delay ) :
    buffer(&buffer), produce(produce), delay(delay) {
}

Consumer::Consumer( BoundedBuffer<int> &buffer,
                    const int delay,
                    const int sentinel,
                    int &sum ) :
    buffer(&buffer), sentinel(sentinel), delay(delay), sum(&sum) {
}

void Producer::main() {
  for (int i = 1; i <= produce; i++) {
    yield(MPRNG()(delay-1));
    buffer->insert(i);
  }
}

void Consumer::main() {
  while (true) {
    yield(MPRNG()(delay-1));
    int next = buffer->remove();
    if (next == sentinel)  {
      break;
    }

    *sum += next;
  }
}

void uMain::main() {
  int cons = 5, prods = 3, produce = 10, bufferSize = 10, delay;
  switch ( argc ) {
    case 6:
      delay = atoi( argv[5] );
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
    delay = cons + prods;
  }

  int sum = 0;
  BoundedBuffer<int> buffer(bufferSize);
  Producer* producer[prods];
  Consumer* consumer[cons];
  {
    for (int i = 0; i < prods; i++) {
      producer[i] = new Producer(buffer, produce, delay);
    }
    for (int i = 0; i < cons; i++) {
      consumer[i] = new Consumer(buffer, delay, SENTINEL, sum);
    }
  }

  for (int i = 0; i < prods; i++) {
    delete producer[i];
  }

  for (int i = 0; i < cons; i++) {
    buffer.insert(-1);
  }

  for (int i = 0; i < cons; i++) {
    delete consumer[i];
  }
  cout << "total: " << sum << endl;
}
