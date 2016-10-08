#include <iostream>
#include <fstream>
#include <sstream>

#include "q1binsertsort.h"

using namespace std;

template <typename T>
void Binsertsort<T>::main() { // YOU WRITE THIS ROUTINE
  while (value == Sentinel) {
    suspend();
  }
  pivot = value;
  suspend();

  Binsertsort<T> left(Sentinel);
  Binsertsort<T> right(Sentinel);
  while (true) {
    if (value == Sentinel) {
      suspend();
      left.sort(Sentinel);
      value = left.retrieve();
      if (value != Sentinel) {
        while (value != Sentinel) {
          suspend();
          value = left.retrieve();
        }
      }

      value = pivot;
      suspend();

      right.sort(Sentinel);
      value = right.retrieve();
      if (value != Sentinel) {
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

void uMain::main() {
  ifstream file_input;
  if (argc == 2) {
    file_input.open(argv[1]);
  }
  istream& input = argc == 2 ? file_input : cin;


  while (!input.eof()) {
    Binsertsort<int> bs(SENTINEL);
    string buffer;
    getline(input, buffer);

    if (buffer.empty()) {
      continue;
    }
    cout << buffer << endl;
    istringstream iss(buffer);
    int num;
    while (iss >> num) {
       bs.sort(num);
    }
    bs.sort(SENTINEL);

    int current = NULL;
    while (current != SENTINEL) {
      current = bs.retrieve();
      if (current != SENTINEL) {
        cout << current << " ";
      }
    }
    cout << endl << endl;
  }
}
