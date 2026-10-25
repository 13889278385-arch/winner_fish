#include "wide_search_logic.hpp"
#include <cassert>
int pwm(double x,double t=0){HorizontalLossRoute r;return r.step(1,t,1,x,false,false,1,false,0);}
int main(){assert(pwm(0)==1300);assert(pwm(.15)==1350);assert(pwm(.2999)==1400);for(double x:{.30,.4,.50}){assert(pwm(x)==1500);assert(pwm(x,.0625)==1600);assert(pwm(x,.1875)==1400);}assert(pwm(.5001)==1600);assert(pwm(.75)==1650);assert(pwm(1)==1700);}
