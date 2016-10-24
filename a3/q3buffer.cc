#include <iostream>
#include <unistd.h>

#include "q3buffer.h"
#include "MPRNG.h"

using namespace std;

template<typename T>
BoundedBuffer<T>::BoundedBuffer( const unsigned int size ) : size(size), bargeInsertFlag(false), bargeRemoveFlag(false) {}

#ifdef BUSY
template<typename T>
void BoundedBuffer<T>::insert( T elem ) {
  owner.acquire();
  while (buffer.size() == size) {
    insertLock.wait(owner);
  }
  buffer.push(elem);
  removeLock.signal();
  if (buffer.size() < size) {
    insertLock.signal();
  }
  owner.release();
}

template<typename T>
T BoundedBuffer<T>::remove() {
  owner.acquire();
  while (buffer.size() == 0) {
    removeLock.wait(owner);
  }
  T val = buffer.front();
  buffer.pop();

  insertLock.signal();
  if (buffer.size() > 0) {
    removeLock.signal();
  }
  owner.release();
  return val;
}
#endif

#ifdef NOBUSY
template<typename T>
void BoundedBuffer<T>::insert( T elem ) {
  owner.acquire();
  if (!insertLock.empty() || bargeInsertFlag) {
    bargeInsertLock.wait(owner);
  }
  if (buffer.size() == size) {
    bargeInsertFlag = true;
    insertLock.wait(owner);
    bargeInsertFlag = false;
  }
  buffer.push(elem);

  bargeInsertLock.signal();
  removeLock.signal();
  if (buffer.size() < size) {
    insertLock.signal();
  }
  owner.release();
}

template<typename T>
T BoundedBuffer<T>::remove() {
  owner.acquire();
  if (!removeLock.empty() || bargeRemoveFlag) {
    bargeRemoveLock.wait(owner);
  }
  if (buffer.size() == 0) {
    bargeRemoveFlag = true;
    removeLock.wait(owner);
    bargeRemoveFlag = false;
  }

  T val = buffer.front();
  buffer.pop();

  insertLock.signal();
  if (buffer.size() > 0) {
    removeLock.signal();
  }
  bargeRemoveLock.signal();
  owner.release();

  return val;
}
#endif

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

#ifdef   __U_MULTI__
  uProcessor p[3]   __attribute__   (( unused )); // create 3 kernel thread for a total of 4
#endif //     U MULTI

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
