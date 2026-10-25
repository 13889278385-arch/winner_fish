#include "swing_search_logic.hpp"
#include <cassert>
int main(){HorizontalLossRoute r;
r.step(0,0,0,.5,true,true);
assert(r.step(1,1,1,.2,false,false)==1400);
assert(r.step(3.99,3.99,3.99,.2,false,false)==1400);
assert(r.step(4,4,4,.2,false,false)==1200);
for(int i=1;i<100;++i){double t=4+i*.01;int p=r.step(t,t,t,.2,false,true);assert(p>=1150&&p<=1250);assert(r.stage==2);}
r.step(5,5,5,.2,false,true);assert(r.found_until==6);
r.step(5.99,5.99,5.99,.2,false,true);assert(r.stage==2);
assert(r.step(6,6,6,.2,false,true)==1400);assert(r.stage==1);
assert(r.step(10,10,10,.8,false,true)==1600);
r.step(11,11,11,.5,true,true);r.step(12,12,12,-1,false,false);
r.step(15,15,15,-1,false,false);r.step(16,16,16,-1,false,false);assert(r.stage==2&&r.found_until<0);
bool failed=false;try{r.step(17,17,16,.2,false,true);}catch(...){failed=true;}assert(failed);
}