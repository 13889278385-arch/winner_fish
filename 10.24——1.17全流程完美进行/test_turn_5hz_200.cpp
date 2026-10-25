#include "wide_search_logic.hpp"
#include <cassert>
int main(){HorizontalLossRoute r;r.stage=2;r.center=1125;r.last_now=1;r.last_down=1;
int lo=9999,hi=0,previous=0;for(int i=1;i<=200;i++){double now=1+i*.001;int p=r.step(now,now,now,.5,false,false,now,true,.5);lo=std::min(lo,p);hi=std::max(hi,p);if(i==50)assert(p==1325);if(i==100)assert(p==1125);if(i==150)assert(p==925);if(i==200)assert(p==1125);previous=p;}assert(lo==925&&hi==1325);
for(double strength:{0.,1.}){HorizontalLossRoute a;for(int i=0;i<300;i++){double t=10+i*.02;int p=a.step(t,t,t,.5,false,false,t,true,strength);assert(p>=650&&p<=1600);}assert(a.stage==2);a.step(17,17,17,.5,false,false,17,false,0);assert(a.stage==1);}
}
