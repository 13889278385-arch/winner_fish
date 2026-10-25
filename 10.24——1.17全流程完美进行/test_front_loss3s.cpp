#include "wide_search_logic.hpp"
#include <cassert>
int main(){HorizontalLossRoute r;for(int i=0;i<150;i++){double t=1+i*.02;int p=r.step(t,t,t,-1,false,false,t,false,0);assert(p>=1450&&p<=1650);}assert(r.step(4,4,4,-1,false,false,4,false,0)==0);assert(r.step(5,5,5,-1,false,false,5,false,0)==0);r.step(6,6,6,.325,false,false,6,false,0);assert(r.step(7,7,7,-1,false,false,7,false,0)!=0);
HorizontalLossRoute a;a.step(1,1,1,-1,false,false,1,true,1);a.step(1.5,1.5,1.5,-1,false,false,1.5,true,1);assert(a.stage==2);int p=a.step(1.6,1.6,1.6,-1,false,false,1.6,true,1);assert(p<=1050);a.step(3.1,3.1,3.1,-1,false,false,3.1,false,0);assert(a.stage==1);assert(a.step(4.1,4.1,4.1,-1,false,false,4.1,false,0)==0);
bool fail=false;try{r.step(10,10,8,-1,false,false,10,false,0);}catch(...){fail=true;}assert(fail);}
