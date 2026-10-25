#include "wide_search_logic.hpp"
#include <cassert>
int main(){HorizontalLossRoute r;for(int i=0;i<25;i++){double t=1+i*.02;r.step(t,t,t,.325,false,false,t,true,.5);assert(r.stage==1);}r.step(1.5,1.5,1.5,.325,false,false,1.5,true,.5);assert(r.stage==2&&r.turn_until==3);double held=r.center;for(int i=1;i<75;i++){double t=1.5+i*.02;r.step(t,t,t,.325,false,false,t,false,0);assert(r.stage==2&&r.center==held);}r.step(3,3,3,.325,false,false,3,false,0);assert(r.stage==1);
HorizontalLossRoute a;a.step(1,1,1,.325,false,false,1,true,1);a.step(1.4,1.4,1.4,.325,false,false,1.4,false,0);a.step(1.6,1.6,1.6,.325,false,false,1.6,true,1);a.step(2,2,2,.325,false,false,2,true,1);assert(a.stage==1);a.step(2.2,2.2,2.2,.325,false,false,2.2,true,1);assert(a.stage==2);}
