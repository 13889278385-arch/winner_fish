#include "wide_search_logic.hpp"
#include <cassert>
int main(){for(double strength:{0.,1.}){HorizontalLossRoute r;r.step(1,1,1,.325,false,false,1,true,strength);r.step(1.12,1.12,1.12,.325,false,false,1.12,true,strength);assert(r.stage==2&&std::abs(r.turn_until-2.12)<1e-8);for(int i=1;i<100;i++){double t=1.12+i*.01;int p=r.step(t,t,t,.325,false,false,t,true,strength);assert(r.stage==2&&r.center>=750&&r.center<=800);assert(std::abs(p-r.center)<=50.5);assert(p>=700&&p<=850);}r.step(2.13,2.13,2.13,.325,false,false,2.13,false,0);assert(r.stage==1);}}
