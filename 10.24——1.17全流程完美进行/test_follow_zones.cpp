#include "wide_search_logic.hpp"
#include <cassert>
int main(){HorizontalLossRoute r;assert(r.step(1,.05,1,.2,false,false)==1500);assert(r.step(1,.15,1,.2,false,false)==1300);assert(r.step(1,.05,1,.8,false,false)==1700);assert(r.step(1,.15,1,.8,false,false)==1500);assert(r.step(1,.0625,1,.5,false,false)==1550);assert(r.step(1,.1875,1,.5,false,false)==1450);}
