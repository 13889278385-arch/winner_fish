#include "wide_search_logic.hpp"
#include <cassert>
int main(){HorizontalLossRoute r;r.step(1,1,1,.325,false,false,1,true,1);r.step(1.12,1.12,1.12,.325,false,false,1.12,true,1);assert(r.stage==2&&std::abs(r.turn_until-2.12)<1e-8);for(int i=1;i<50;i++){double t=1.12+i*.02;int p=r.step(t,t,t,.325,false,false,t,true,1);assert(r.stage==2&&r.center>=850&&r.center<=900&&p>=750&&p<=1000);}double held=r.center;r.step(2.115,2.115,2.115,.325,false,false,2.115,false,0);assert(r.stage==2&&r.center==held);r.step(2.13,2.13,2.13,.325,false,false,2.13,false,0);assert(r.stage==1);}
