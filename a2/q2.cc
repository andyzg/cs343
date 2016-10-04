class Potato {
    // YOU ADD MEMBERS HERE
  public:
    _Event Explode {};
    Potato( unsigned int maxTicks = 10 );
    void reset( unsigned int maxTicks = 10 );
    void countdown();
};

_Coroutine Player {
    // YOU ADD MEMBERS HERE
    void main();
  public:
    typedef ... PlayerList; // container type of your choice
    Player( Umpire &umpire, unsigned int Id, const PlayerList &players );
    void toss( Potato &potato );
};

_Coroutine Umpire {
    // YOU ADD MEMBERS HERE
    void main();
  public:
    Umpire( Player::PlayerList &players );
    void set( unsigned int player );
};
