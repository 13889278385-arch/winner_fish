#include "wide_search_logic.hpp"
#include <cassert>
int pwm(double x,double t=0){HorizontalLossRoute r;return r.step(1,t,1,x,false,false,1,false,0);}
int main(){assert(pwm(0)==1300);assert(pwm(.225)==1350);assert(pwm(.4499)==1400);for(double x:{.45,.55,.6}){assert(pwm(x)==1500);assert(pwm(x,.0625)==1600);assert(pwm(x,.1875)==1400);}assert(pwm(.6001)==1600);assert(pwm(.8)==1650);assert(pwm(1)==1700);}
