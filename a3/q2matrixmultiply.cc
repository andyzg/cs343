#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

// Task class
_Task T {

  // Matrices
  int **X;
  int **Y;
  int **Z;

  // Row ranges
  unsigned int xs;
  unsigned int xe;

  // xcyr
  unsigned int y;
  // yc
  unsigned int z;

  void main();
public:
  T(int *X[], int *Y[], int *Z[],
             unsigned int xs, unsigned int xe,
             unsigned int y,
             unsigned int z);
};

T::T(int *X[], int *Y[], int *Z[],
           unsigned int xs, unsigned int xe,
           unsigned int y,
           unsigned int z) {
  this->X = X;
  this->Y = Y;
  this->Z = Z;

  this->xs = xs;
  this->xe = xe;
  this->y = y;
  this->z = z;
}

void T::main() {
  // Make tasks until the range is on 1 number
  if (xs != xe) {
    T tbin1(X, Y, Z, xs, xe + (xs - xe)/2 + 1, y, z);
    T tbin2(X, Y, Z, xe + (xs - xe)/2, xe, y, z);
    return;
  }

  // Calculate values for the row of Z at xs
  for (unsigned int i = 0; i < z; i++) {
    int sum = 0;
    for (unsigned int j = 0; j < y; j++) {
      sum += X[xs][j] * Y[j][i];
    }
    Z[xs][i] = sum;
  }
}



void matrixmultiply( int *Z[], int *X[], unsigned int xr, unsigned int xc, int *Y[], unsigned int yc ) {
  T tbin(X, Y, Z, xr-1, 0, xc, yc);
}

// Generate a string filled with a character "fill"
string genFill( int num, char fill) {
  string space = "";
  for (int i = 0; i < num; i++) {
    space = space + fill;
  }
  return space;
}

// Print out the matrices X, Y and Z
void output( int *x[], int *y[], int *z[], int a, int b, int c) {
  int leftSpacing = -1 + b * 9 + 4;
  string space = genFill(leftSpacing, ' ');

  for (int i = 0; i < b; i++) {
    cout << space << "|";
    for (int j = 0; j < c; j++) {
      int numLength = to_string(y[i][j]).length();
      string buff = genFill(9 - numLength, ' ');
      cout << buff << y[i][j];
    }
    cout << " " << endl;
  }

  int rightSpacing = 9 * c + 1;
  cout << genFill(leftSpacing, '-') << "*" << genFill(rightSpacing, '-') << endl;

  for (int i = 0; i < a; i++) {
    for (int j = 0; j < b; j++) {
      int numLength = to_string(x[i][j]).length();
      string buff = genFill(9 - (j == 0 ? 1 : 0) - numLength, ' ');
      cout << buff << x[i][j];
    }
    cout << genFill(4, ' ') << '|';
    for (int j = 0; j < c; j++) {
      int numLength = to_string(z[i][j]).length();
      string buff = genFill(9 - numLength, ' ');
      cout << buff << z[i][j];
    }
    cout << " " << endl;
  }
}

void uMain::main() {
  bool runWithoutFiles = false;
  if (argc == 4) {
    runWithoutFiles = true;
  } else if (argc != 6) {
    cout << "Usage: " << argv[0] << " xrows (> 0) xycols (> 0) ycols (> 0)  [ x-matrix-file  y-matrix-file ]" << endl;
    return;
  }

  ifstream x_matrix;
  ifstream y_matrix;
  if (!runWithoutFiles) {
    try {
      x_matrix.open(argv[4]);
    } catch (uFile::FileAccess::OpenFailure e) {
      cout << "Error! Cannot open x";
      cout << "-matrix input-file \"" << argv[4] << "\"" << endl;
      cout << "Usage: " << argv[0] << " xrows (> 0) xycols (> 0) ycols (> 0)  [ x-matrix-file  y-matrix-file ]" << endl;
      return;
    }

    try {
      y_matrix.open(argv[5]);
    } catch (uFile::FileAccess::OpenFailure e) {
      cout << "Error! Cannot open y";
      cout << "-matrix input-file \"" << argv[5] << "\"" << endl;
      cout << "Usage: " << argv[0] << " xrows (> 0) xycols (> 0) ycols (> 0)  [ x-matrix-file  y-matrix-file ]" << endl;
      return;
    }

    try {
      for (unsigned int i = 0; i < strlen(argv[1]); i++) {
        if (!isdigit(argv[1][i])) { throw 1; }
      }
      for (unsigned int i = 0; i < strlen(argv[2]); i++) {
        if (!isdigit(argv[2][i])) { throw 1; }
      }
      for (unsigned int i = 0; i < strlen(argv[3]); i++) {
        if (!isdigit(argv[3][i])) { throw 1; }
      }
    } catch (int e) {
      cout << "Usage: " << argv[0] << " xrows (> 0) xycols (> 0) ycols (> 0)  [ x-matrix-file  y-matrix-file ]" << endl;
      return;
    }
  }

  int a = atoi(argv[1]);
  int b = atoi(argv[2]);
  int c = atoi(argv[3]);

  // DONE ALL OF THE ERROR CHECKS
  int **x = new int*[a];
  int **y = new int*[b];
  int **z = new int*[a];

  // Compiler optimizations
  uProcessor p[a - 1] __attribute__(( unused ));

  string line;
  int num;
  for (int i = 0; i < a; i++) {
    if (!runWithoutFiles) {
      getline(x_matrix, line);
    }
    istringstream iss(line);
    x[i] = new int[b];
    for (int j = 0; j < b; j++) {
      if (!runWithoutFiles) {
        iss >> num;
      } else {
        num = 37;
      }
      x[i][j] = num;
    }
  }

  for (int i = 0; i < b; i++) {
    if (!runWithoutFiles) {
      getline(y_matrix, line);
    }
    istringstream iss(line);
    y[i] = new int[c];
    for (int j = 0; j < c; j++) {
      if (!runWithoutFiles) {
        iss >> num;
      } else {
        num = 37;
      }
      y[i][j] = num;
    }
  }

  for (int i = 0; i < a; i++) {
    z[i] = new int[c];
  }

  matrixmultiply(z, x, a, b, y, c);
  if (!runWithoutFiles) {
    output(x, y, z, a, b, c);
  }

  for (int i = 0; i < a; i++) {
    free(x[i]);
    free(z[i]);
  }

  for (int i = 0; i < b; i++) {
    free(y[i]);
  }
  delete x;
  delete y;
  delete z;
}
