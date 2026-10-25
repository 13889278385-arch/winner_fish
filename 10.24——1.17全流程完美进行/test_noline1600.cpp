#include "wide_search_logic.hpp"
#include <cassert>
int main(){HorizontalLossRoute r;assert(r.step(1,0,1,-1,false,false,1,false,0)==1600);assert(r.step(1.1,.0625,1.1,-1,false,false,1.1,false,0)==1700);assert(r.step(1.2,.1875,1.2,-1,false,false,1.2,false,0)==1500);assert(r.step(4,0,4,-1,false,false,4,false,0)==0);assert(r.step(5,0,5,.325,false,false,5,false,0)==1550);assert(r.step(6,0,6,-1,false,false,6,false,0)==1600);r.step(6.1,0,6.1,-1,false,false,6.1,true,1);int p=r.step(6.22,0,6.22,-1,false,false,6.22,true,1);assert(r.stage==2&&p>=600&&p<=750);}
