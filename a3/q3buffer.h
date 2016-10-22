#include <vector>

template<typename T> class BoundedBuffer {
  private:
    const unsigned int size;
    std::vector<T> buffer;
  public:
    BoundedBuffer( const unsigned int size = 10 );
    void insert( T elem );
    T remove();
};

_Task Producer {
    void main();
  private:
    BoundedBuffer<int> buffer;
    const int produce;
    const int delay;
  public:
    Producer( BoundedBuffer<int> &buffer, const int Produce, const int Delay );
    Producer() = delete;
};

_Task Consumer {
    void main();
  private:
    BoundedBuffer<int> buffer;
    const int delay;
    const int sentinel;
    int sum;
  public:
    Consumer( BoundedBuffer<int> &buffer, const int Delay, const int Sentinel, int &sum );
    Consumer() = delete;
};