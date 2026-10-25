#include "horizontal_loss_logic.hpp"
#include <cassert>
int main(){HorizontalLossRoute r;
r.step(0,0,0,.5,true,true);
assert(r.step(1,1,1,.2,false,true)==1400);
assert(r.step(3.99,3.99,3.99,.2,false,true)==1400);
r.step(4,4,4,.5,true,true);
assert(r.step(5,5,5,.2,false,true)==1400);
assert(r.step(7.99,7.99,7.99,.2,false,true)==1400);
assert(r.step(8,8,8,.2,false,true)==1200);
assert(r.step(8.99,8.99,8.99,.2,false,true)==1200);
assert(r.step(9,9,9,.2,false,true)==1400);
assert(r.step(10,10,10,.2,false,true)==1400);
bool failed=false;try{r.step(11,11,10,.2,false,true);}catch(...){failed=true;}assert(failed);
}