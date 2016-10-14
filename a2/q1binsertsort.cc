#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

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
  ofstream file_output;
  if (argc == 1) {
    cout << "Usage: " << argv[0] << " unsorted-file [ sorted-file ]" << endl;
    return;
  }
  if (argc >= 2) {
    file_input.open(argv[1]);
  }
  if (argc == 3) {
    file_output.open(argv[2]);
  }

  istream& input = argc >= 2 ? file_input : cin;
  ostream& output = argc == 3 ? file_output : cout;


  while (!input.eof()) {
    Binsertsort<TYPE> bs(SENTINEL);
    string buffer;
    getline(input, buffer);

    if (buffer.empty()) {
      continue;
    }
    istringstream iss(buffer);
    TYPE num;
    int numItems;
    iss >> numItems;
    string toPrint = "";
    for (int i = 0; i < numItems; i++) {
      iss >> num;
      toPrint = toPrint + to_string(num);
      if (i != numItems - 1) {
        toPrint = toPrint + " ";
      }
      bs.sort(num);
    }
    output << toPrint << endl;
    bs.sort(SENTINEL);

    TYPE current = NULL;
    while (current != SENTINEL) {
      current = bs.retrieve();
      if (current != SENTINEL) {
        output << current << " ";
      }
    }
    output << endl << endl;
  }

  if (argc == 3) {
    file_output.close();
  }
}
