#include "wide_search_logic.hpp"
#include "logic_before_plus50_test.hpp"
#include <cassert>
int main(){for(double x:{-1.,0.,.15,.2999,.30,.4,.50,.5001,.75,1.}){HorizontalLossRoute a;PreviousRoute b;for(int i=0;i<500;i++){double t=1+i*.02;int p=a.step(t,t,t,x,false,false,t,false,0),q=b.step(t,t,t,x,false,false,t,false,0);assert(p==(q==0?0:q+50));}}
for(double strength:{0.,.5,1.}){HorizontalLossRoute a;PreviousRoute b;for(int i=0;i<300;i++){double t=1+i*.02;bool left=i<180;int p=a.step(t,t,t,.4,false,false,t,left,strength),q=b.step(t,t,t,.4,false,false,t,left,strength);assert(p==q+50);assert(a.stage==b.stage);assert(a.turns==b.turns);}}
}
