#include <iostream>
#include <fstream>
#include <string>
#include <map>

using namespace std;

enum STATE {
  INITIAL,
  E,
  SIGN,
  EXP_SIGN,
  START_DIGIT,
  FLOATING,
  END_DIGIT,
  START_DOT,
  END_DOT,
};

typedef map< STATE, map< char, STATE > > DFA;
typedef map< STATE, bool > END_STATES;
static DFA dfa;
static END_STATES es;

_Coroutine FloatConstant {
  public:
    enum Status { CONT, MATCH };		// possible status
  private:
    char ch;
    Status status;
    STATE state;
    void main() {
      state = INITIAL;
      while (true) {
        // cout << state << " " << ch << endl;
        if (dfa.find(state) == dfa.end() || dfa[state].find(ch) == dfa[state].end()) {
          // TODO: Throw some exception
          // cout << "Error " << (dfa.find(state) == dfa.end()) << " " << (dfa[state].find(ch) == dfa[state].end()) << endl;
          // _Resume Error();
        }
        state = dfa[state][ch];
        if (es.find(state) == es.end()) {
          status = Status::CONT;
        } else {
          status = Status::MATCH;
        }
        suspend();
      }
    }
  public:
    _Event Error {};					// last character is invalid
    Status next( char c ) {
      ch = c;							// communication in
      resume();						// activate
      return status;					// communication out
    }
};

void initializeMap() {
  es[FLOATING] = true;
  es[END_DIGIT] = true;
  es[END_DOT] = true;

  dfa[INITIAL]['+'] = SIGN;
  dfa[INITIAL]['-'] = SIGN;

  dfa[SIGN]['.'] = START_DOT;
  dfa[INITIAL]['.'] = START_DOT;
  dfa[START_DIGIT]['.'] = END_DOT;

  for (int i = 0; i < 10; i++) {
    char c = (char) i + 48;
    dfa[INITIAL][c] = START_DIGIT;
    dfa[SIGN][c] = START_DIGIT;

    dfa[START_DIGIT][c] = START_DIGIT;
    dfa[END_DIGIT][c] = END_DIGIT;

    dfa[END_DOT][c] = END_DIGIT;

    dfa[E][c] = END_DIGIT;
  }

  dfa[END_DOT]['f'] = FLOATING;
  dfa[END_DOT]['l'] = FLOATING;
  dfa[END_DOT]['F'] = FLOATING;
  dfa[END_DOT]['L'] = FLOATING;

  dfa[END_DIGIT]['f'] = FLOATING;
  dfa[END_DIGIT]['1'] = FLOATING;
  dfa[END_DIGIT]['F'] = FLOATING;
  dfa[END_DIGIT]['L'] = FLOATING;

  dfa[START_DIGIT]['e'] = E;
  dfa[START_DIGIT]['E'] = E;

  dfa[E]['+'] = EXP_SIGN;
  dfa[E]['-'] = EXP_SIGN;
}

void uMain::main() {
  initializeMap();
  ifstream file_input;
  if (argc == 2) {
    file_input.open(argv[1]);
  }
  istream& input = argc == 2 ? file_input : cin;

  FloatConstant fc;

  while (!input.eof()) {
    string buffer;
    getline(input, buffer);
    try {
      _Enable {
        FloatConstant::Status status;
        for (string::iterator it = buffer.begin(); it != buffer.end(); ++it) {
          status = fc.next(*it);
        }
        cout << "\"" << buffer << "\" : \"" << buffer << "\" ";
        if (status == FloatConstant::Status::CONT) {
          cout << "no" << endl;
        } else {
          cout << "yes" << endl;
        }
      }
    } catch (FloatConstant::Error) {
      cout << "Caught some error" << endl;
    }
  }
}
