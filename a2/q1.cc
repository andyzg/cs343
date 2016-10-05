#include <iostream>


using namespace std;

template<typename T> _Coroutine Binsertsort {
    const T Sentinel;            // value denoting end of set
    T value;                     // communication: value being passed down/up the tree
    T pivot;                     // property: current value of the node
    void main() { // YOU WRITE THIS ROUTINE
      while (value == Sentinel) {
        suspend();
      }
      pivot = value;
      suspend();

      Binsertsort<int> left(Sentinel);
      Binsertsort<int> right(Sentinel);
      while (true) {
        if (value == Sentinel) {
          suspend();
          left.sort(Sentinel);
          value = left.retrieve();
          // cout << "And then " << value << " from " << pivot << endl;
          if (value != Sentinel) {
            cout << "left" << endl;
            while (value != Sentinel) {
              suspend();
              value = left.retrieve();
            }
          }

          value = pivot;
          cout << "And then " << value << " from " << pivot << endl;
          suspend();

          right.sort(Sentinel);
          value = right.retrieve();
          cout << "And then .." << value << " from " << pivot << endl;
          if (value != Sentinel) {
            cout << "right" << endl;
            while (value != Sentinel) {
              suspend();
              value = right.retrieve();
            }
          }
        } else if (value < pivot) {
          left.sort(value);
        } else if (value >= pivot) {
          right.sort(value);
        }
        suspend();
      }
    }
  public:
    Binsertsort( T Sentinel ) : Sentinel( Sentinel ) {}
    void sort( T value ) {       // value to be sorted
        Binsertsort::value = value;
        resume();
    }
    T retrieve() {               // retrieve sorted value
        resume();
        return value;
    }
};

void uMain::main() {
  Binsertsort<int> bs(SENTINEL);
  bs.sort(2);
  bs.sort(3);
  bs.sort(1);
  bs.sort(SENTINEL);
  int current = NULL;
  while (current != SENTINEL) {
    current = bs.retrieve();
    cout << "OMG" << current << endl;
  }
}
