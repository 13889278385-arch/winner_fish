#include "wide_search_logic.hpp"
#include <cassert>
int main(){HorizontalLossRoute r;int p=r.step(1,1,1,-1,false,false,1,true,1);assert(r.stage==2&&r.turn_until==2&&p>=600&&p<=750);r.step(2.01,2.01,2.01,.325,false,false,2.01,true,1);assert(r.stage==1);r.step(2.1,2.1,2.1,.325,false,false,2.1,false,0);r.step(2.2,2.2,2.2,.325,false,false,2.2,true,1);assert(r.stage==2&&r.turns==2);}
