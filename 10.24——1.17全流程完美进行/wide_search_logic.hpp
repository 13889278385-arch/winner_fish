#pragma once
#include <cmath>
#include <stdexcept>
#include <algorithm>
struct HorizontalLossRoute {
 int stage=1;unsigned turns=0;double candidate=-1,last_down=-1,center=700,last_now=-1,turn_until=-1,turn_start=0,front_missing=-1;bool ready=true;
 int step(double now,double t,double stamp,double x,bool horizontal,bool any,double down_stamp,bool left,double strength){
  if(!std::isfinite(stamp)||!std::isfinite(x)||stamp>now||now-stamp>.5||x < -1||x>1)throw std::runtime_error("Front vision stale/invalid");
  if(x>=0)front_missing=-1;else if(front_missing<0)front_missing=now;
  bool fresh=std::isfinite(down_stamp)&&down_stamp>0&&down_stamp<=now&&now-down_stamp<=.5&&std::isfinite(strength)&&strength>=0&&strength<=1;
  if(!fresh){candidate=-1;if(stage==2)throw std::runtime_error("Down camera stale during turn");}
  if(stage==2&&now>=turn_until){stage=1;candidate=-1;ready=false;}
  if(fresh&&down_stamp>last_down){
   last_down=down_stamp;
   if(stage!=2){
    if(!left){ready=true;candidate=-1;}
    else if(ready){if(candidate<0)candidate=down_stamp;if(down_stamp-candidate>=0){stage=2;++turns;center=700;turn_start=now;turn_until=now+1;ready=false;}}
   }
  }
  double dt=last_now<0?0:std::max(0.,std::min(.1,now-last_now));last_now=now;
  if(stage==2){
   double wanted=left?700-50*strength:center;
   center+=std::max(-600*dt,std::min(600*dt,wanted-center));
   return int(std::lround(center+50*std::sin(2*std::acos(-1.)*5*(now-turn_start))));
  }
  if(x<0){if(now-front_missing>=3)return 0;return int(std::lround(1550+100*std::sin(8*std::acos(-1.)*t)));}
  if(x<.30)return int(std::lround((1350+100*x/.30)+100*std::sin(10*std::acos(-1.)*t)));
  if(x>.35)return int(std::lround((1650+100*(x-.35)/.65)+100*std::sin(10*std::acos(-1.)*t)));
  return int(std::lround(1550+100*std::sin(8*std::acos(-1.)*t)));
 }
};
