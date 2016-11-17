#define AUTOMATIC_SIGNAL uCondition cond;
#define WAITUNTIL(pred, before, after) if (pred) { before; cond.wait(); after; } else { while (!cond.empty()) { cond.signal(); } }
#define RETURN(expr) return expr;
