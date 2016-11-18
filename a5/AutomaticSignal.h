// Declare a uCondition
#define AUTOMATIC_SIGNAL uCondition cond;
// if pred is true, then run before, wait, then after. Otherwise, wake up everyone waiting on uCondition
#define WAITUNTIL(pred, before, after) if (!(pred)) { before; while (!cond.empty()) { cond.signal(); } do { cond.wait(); } while (!(pred)); after; }
// Return the expression
#define RETURN(expr) while (!cond.empty()) { cond.signal(); } return expr;
