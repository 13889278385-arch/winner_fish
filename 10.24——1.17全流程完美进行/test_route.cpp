#include "route_logic.hpp"
#include <cassert>
#include <iostream>
int main(){
 Route r;double now=100;
 auto step=[&](double t,double x,bool h){now=t;return r.step(t,t-100,t,x,h);};
 assert(step(100,.2,false)==1400);
 assert(step(100.01,.8,false)==1600);
 assert(step(100.02,-1,false)==0);
 step(101,.5,true);assert(r.stage==1);
 assert(step(101.21,.5,true)==600&&r.stage==2);
 assert(step(105.20,.5,true)==600&&r.stage==2);
 step(105.22,.5,true);assert(r.stage==3);
 step(106,.5,true);assert(r.stage==3); // same crossbar cannot retrigger
 step(107,.5,false);step(107.31,.5,false);
 step(108,.5,true);assert(step(108.21,.5,true)==600&&r.stage==4);
 assert(step(112.20,.5,false)==600);
 step(112.22,.5,true);assert(r.stage==5);
 step(113,.5,false);step(114,.5,false);step(115,.5,true);step(116,.5,true);assert(r.stage==5);
 bool threw=false;try{r.step(120,20,119,.5,false);}catch(...){threw=true;}assert(threw);
 Route duplicate;duplicate.step(100,0,100,.5,true);duplicate.step(100.3,.3,100,.5,true);assert(duplicate.stage==1);
 Route noise;noise.step(100,0,100,.5,true);noise.step(100.1,.1,100.1,.5,false);noise.step(100.2,.2,100.2,.5,true);assert(noise.stage==1);
 Route center;for(int i=0;i<100;++i){double t=i*.01;int p=center.step(100+t,t,100+t,.5,false);assert(p>=1450&&p<=1550);}
 std::cout<<"PASS five stages, 4s turns, no third turn, rearm, duplicate frames, noise, stale frames, 3Hz range\n";
}
