#pragma once
#include <cmath>
#include <stdexcept>
struct HorizontalLossRoute {
 int stage=1;unsigned turns=0;
 double until=0,lost_since=-1,turn_start=0,found_until=-1;
 bool seen=false,wait_horizontal=false;
 int swing(double now){return int(std::lround(850+150*std::sin(6*std::acos(-1.)*(now-turn_start))));}
 int step(double now,double t,double stamp,double x,bool horizontal,bool any){
  if(!std::isfinite(stamp)||!std::isfinite(x)||stamp>now||now-stamp>.5||x < -1||x>1)throw std::runtime_error("Vision unavailable/stale");
  if(horizontal){seen=true;wait_horizontal=false;lost_since=-1;}
  if(stage==2){
   if(found_until>=0){
    if(now<found_until)return swing(now);
    stage=1;wait_horizontal=!horizontal;lost_since=-1;found_until=-1;
   }else{
    if(now>=until){if(any)found_until=now+1;until=now+.1;}
    return swing(now);
   }
  }else if(seen&&!wait_horizontal&&!horizontal){
   if(lost_since<0)lost_since=now;
   if(now-lost_since>=1){stage=2;found_until=-1;turn_start=now;until=now+.1;lost_since=-1;++turns;return swing(now);}
  }
  if(x<0)return 0;
  if(x<.4)return int(std::lround(1400+50*std::sin(8*std::acos(-1.)*t)));
  if(x>.6)return int(std::lround(1600+50*std::sin(8*std::acos(-1.)*t)));
  return int(std::lround(1500+50*std::sin(8*std::acos(-1.)*t)));
 }
};
