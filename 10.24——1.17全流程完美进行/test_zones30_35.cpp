#include "wide_search_logic.hpp"
#include <cassert>
int pwm(double x,double t=0){HorizontalLossRoute r;return r.step(1,t,1,x,false,false,1,false,0);}
int main(){assert(pwm(0)==1350);assert(pwm(.15)==1400);assert(pwm(.2999)==1450);for(double x:{.30,.325,.35}){assert(pwm(x)==1550);assert(pwm(x,.0625)==1650);assert(pwm(x,.1875)==1450);}assert(pwm(.3501)==1650);assert(pwm(.675)==1700);assert(pwm(1)==1750);assert(pwm(-1)==1550);}
