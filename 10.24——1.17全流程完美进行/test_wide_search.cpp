#include "wide_search_logic.hpp"
#include <cassert>
int main(){HorizontalLossRoute r;
r.step(0,0,0,.5,false,false);assert(r.stage==1);
r.step(1,1,1,.5,true,true);r.step(2,2,2,.2,false,false);
r.step(2.99,2.99,2.99,.2,false,false);assert(r.stage==1);
assert(r.step(3,3,3,.2,false,false)==850);assert(r.stage==2);
assert(r.swing(3+1.0/12)==1000);assert(r.swing(3.25)==700);
r.step(3.099,3.099,3.099,.2,false,true);assert(r.stage==2);
r.step(3.1,3.1,3.1,.2,false,true);assert(r.stage==2);
r.step(4.099,4.099,4.099,.2,false,true);assert(r.stage==2);
r.step(4.1,4.1,4.1,.2,false,true);assert(r.stage==1);
r.step(5,5,5,.2,false,true);assert(r.stage==1);
r.step(6,6,6,.5,true,true);r.step(7,7,7,.5,false,false);
r.step(8,8,8,.5,false,false);assert(r.stage==2);
r.step(8.1,8.1,8.1,.5,false,false);assert(r.stage==2);
r.step(8.2,8.2,8.2,.5,false,true);assert(r.stage==2);
r.step(9.199,9.199,9.199,.5,false,true);assert(r.stage==2);
r.step(9.2,9.2,9.2,.5,false,true);assert(r.stage==1);
HorizontalLossRoute f;
assert(f.step(.0625,.0625,.0625,.5,false,true)==1550);
assert(f.step(.1875,.1875,.1875,.5,false,true)==1450);
assert(f.step(.3125,.3125,.3125,.5,false,true)==1550);
assert(f.step(.0625,.0625,.0625,.2,false,true)==1450);
assert(f.step(.1875,.1875,.1875,.8,false,true)==1550);
bool failed=false;try{r.step(10,10,9,.5,false,true);}catch(...){failed=true;}assert(failed);
}