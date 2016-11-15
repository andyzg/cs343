#define AUTOMATIC_SIGNAL uCondition cond;
#define WAITUNTIL(pred, before, after) if (pred) { cond.wait(); } else { while (!cond.empty()) { cond.signal(); } }
#define RETURN(expr) return expr;
