************************************************************************************
************************************************************************************
** Title:  Resistance.inc
** Author: Jongyeon Kim, VLSI Research Lab @ UMN
** Email:  kimx2889@umn.edu
************************************************************************************
** The dependence of resistance on  relative angle, temperature, and bias voltage
** is implemented.
************************************************************************************

.subckt RA n_plus n_minus Mx My Mz Tmp thi lx='lx' ly='ly' P0='P0' RA0='RA0' MA='MA'


*** Spherical coordinate ***
E_thip thip 0 vol='(1-MA)*acos((1-MA)*0.999*v(My)/((v(Mx)^2+v(My)^2)^(1/2)))'
E_th1 th1 0 vol='((v(My)^2+v(Mz)^2)^(1/2))'
E_th2 th2 0 vol='v(Mz)/v(th1)' max=1 min=-1
E_thp thp 0 vol='3.14/2*(1-v(th2))'

E_th th 0 vol='v(thip)+v(thp)' max='355/113-v(thi)' min='v(thi)'
E_phi phi 0 vol='(1-MA)*atan(v(Mx)/v(Mz))+MA*atan(v(My)/v(Mx))'


*** Temp. dependent parameters *******************
.param asp='2e-5'

*** Rp *******************************************
.param RA='RA0*1e-12'   $ [ohm*m2]
.param Rp='RA/(lx*ly)'
E_Rp Rp 0 vol='RA/(lx*ly)'

*** TMR ******************************************
.param v0='0.65'	
E_TMR0 TMR0 0 vol='2*P0^2*(1-asp*v(Tmp)^1.5)^2/(1-P0^2*(1-asp*v(Tmp)^1.5)^2)*100'
E_TMR TMR 0 vol='v(TMR0)/(1+((v(n_plus)-v(n_minus))/v0)^2)'
E_Rap Rap 0 vol='(v(TMR)/100+1)*Rp'

*** R(V,Tmp,th) **********************************
R_MTJ n_plus n_minus '(1+cos(v(th)))*(Rp-v(Rap))/2+v(Rap)'
*E_rmtj rmtj 0 vol='(1+cos(v(th)))*(Rp-v(Rap))/2+v(Rap)'
E_rmtj rmtj 0 vol='Rp + ((v(Rap) - Rp) / 2) * (1 - v(Mz))'


.ends
