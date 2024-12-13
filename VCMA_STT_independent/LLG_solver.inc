************************************************************************************
************************************************************************************
** Title:  LLG_solver.inc
** Author: Jeehwan Song, VLSI Research Lab @ UMN (songx944@umn.edu)
** This is modified based on previous STT model by Jongyeon Kim, VLSI Research Lab @ UMN (kimx2889@umn.edu)
************************************************************************************
** At the given MTJ dimensions and material parameters, the dynamic tr motion is
** implemented by LLG equation according to the type of MTJ. 
************************************************************************************

.subckt LLG Mx My Mz Is Ias Tmp thi e3 lx='lx' ly='ly' lz='lz' Ms0='Ms0' P0='P0' alpha='alpha' MA='MA' ini='ini' tc='tc' tox='tox' 

.include therm_dev_45.in
.param therm = '1' $ applicaility of thermal fluctuation (therm=1:applied, therm=0:removed)

*** Physical parameters *************************************************
.param pi='355/113'
.param gamma='2.8e6*2*pi'
.param h='6.625e-27/(2*pi)'
.param e='1.602e-19'
.param kb='1.38e-16'

*** Temp. dependent parameters ******************************************
.param Tcurie='1420'
.param beta='0.4'
.param asp='2e-5'
E_Ms Ms 0 vol='Ms0*(1-v(Tmp)/Tcurie)^beta'
E_P  P  0 vol='P0*(1-asp*v(Tmp)^1.5)'

*** Magnetization of pinned layer ***************************************
.param Mpx='0.0'
.param Mpy='1-MA'
.param Mpz='MA'

*** Shape anisotropy - Demagnetizing factors ****************************
.param Nsh(a,b,c)='1/pi*((b^2-c^2)/(2*b*c)*log((sqrt(a^2+b^2+c^2)-a)/(sqrt(a^2+b^2+c^2)+a))+(a^2-c^2)/(2*a*c)*log((sqrt(a^2+b^2+c^2)-b)/(sqrt(a^2+b^2+c^2)+b))+b/(2*c)*log((sqrt(a^2+b^2)+a)/(sqrt(a^2+b^2)-a))+a/(2*c)*log((sqrt(a^2+b^2)+b)/(sqrt(a^2+b^2)-b))+c/(2*a)*log((sqrt(b^2+c^2)-b)/(sqrt(b^2+c^2)+b))+c/(2*b)*log((sqrt(a^2+c^2)-a)/(sqrt(a^2+c^2)+a))+2*atan((a*b)/(c*sqrt(a^2+b^2+c^2)))+(a^3+b^3-2*c^3)/(3*a*b*c)+(a^2+b^2-2*c^2)/(3*a*b*c)*sqrt(a^2+b^2+c^2)+c/(a*b)*(sqrt(a^2+c^2)+sqrt(b^2+c^2))-((b^2+a^2)^(3/2)+(b^2+c^2)^(3/2)+(a^2+c^2)^(3/2))/(3*a*b*c))'

.param Nx='4*pi*Nsh(lz,ly,lx)'
.param Ny='4*pi*Nsh(lz,lx,ly)'
.param Nz='4*pi*Nsh(ly,lx,lz)'
E_Nz Nz 0 vol='4*pi*Nsh(ly,lx,lz)'

*** Interfacial anisotropy field for perpendicular MTJ ******************
 E_Ki Ki 0 vol='(2*pi*v(Ms)*v(Ms)*tc*1e2)-(vcma_coeff*(1e7/1e2)*v(e3)/(tox*1e2))' $VCMA-effect
 E_Hiz Hiz 0 vol='2*v(Ki)/(v(Ms)*lz*1e2)*v(Mz)'	

*** Initialization ******************************************************
.param Msi='Ms0*(1-Tmp0/Tcurie)^beta'
 E_Msi Msi 0 vol='Ms0*(1-Tmp0/Tcurie)^beta'
.param iPMA='(2*pi*((tc/lz)-(Nz/(4*pi)))*Msi*Msi*lx*ly*lz*1e6)/(kb*Tmp0)-(vcma_coeff*(1e7/1e2)*lx*ly*1e4*v(e3)/(tox*1e2*kb*Tmp0))' $VCMA-effect
 E_thste thste 0 vol='iPMA'

.param thi='asin((1/(2*iPMA))^(1/2))'
E_thi thi 0 vol='asin((1/(2*iPMA))^(1/2))'

.param Mx0='0.0'
.param My0='sin(thi)'
.param Mz0='(1-2*ini)*cos(thi)'

.ic v(Mx)='Mx0'
.ic v(My)='My0'
.ic v(Mz)='Mz0'

*** Demagnetizating field for in-plane MTJ ******************************
E_Hdx Hdx 0 vol='-Nx*v(Mx)*v(Ms)'
E_Hdy Hdy 0 vol='-Ny*v(My)*v(Ms)'
E_Hdz Hdz 0 vol='-Nz*v(Mz)*v(Ms)'

*** Effective anisotropy field + External field + Thermal fluctuation ***
E_Hefx Hefx 0 vol='v(Hdx)+Hext_x+therm*v(Hthx)'		
E_Hefy Hefy 0 vol='v(Hdy)+Hext_y+therm*v(Hthy)'		
E_Hefz Hefz 0 vol='v(Hdz)+MA*v(Hiz)+Hext_z+therm*v(Hthz)'	

*** Polarized spin current (J=Ias/lx/ly) ********************************
R_Is Is 0 'v(Ias)*v(P)*h/(2*e*lx*ly*lz*1e6*v(Ms))'

*** LLG solving for Mx, My, Mz ******************************************
C_Mx Mx 0 '(1+alpha*alpha)/gamma'
G_dMx_prec 0 Mx cur='-(v(My)*v(Hefz)-v(Hefy)*v(Mz))'
G_dMx_damp 0 Mx cur='-alpha*(v(My)*(v(Mx)*v(Hefy)-v(Hefx)*v(My))-(v(Mz)*v(Hefx)-v(Hefz)*v(Mx))*v(Mz))'
G_dMx_torq 0 Mx cur='v(Is)*(v(My)*(v(Mx)*Mpy-Mpx*v(My))-(v(Mz)*Mpx-Mpz*v(Mx))*v(Mz))'

C_My My 0 '(1+alpha*alpha)/gamma'
G_dMy_prec 0 My cur='-(v(Mz)*v(Hefx)-v(Hefz)*v(Mx))'
G_dMy_damp 0 My cur='-alpha*(v(Mz)*(v(My)*v(Hefz)-v(Hefy)*v(Mz))-(v(Mx)*v(Hefy)-v(Hefx)*v(My))*v(Mx))'
G_dMy_torq 0 My cur='v(Is)*(v(Mz)*(v(My)*Mpz-Mpy*v(Mz))-(v(Mx)*Mpy-Mpx*v(My))*v(Mx))'

C_Mz Mz 0 '(1+alpha*alpha)/gamma'
G_dMz_prec 0 Mz cur='-(v(Mx)*v(Hefy)-v(Hefx)*v(My))'
G_dMz_damp 0 Mz cur='-alpha*(v(Mx)*(v(Mz1)*v(Hefx)-v(Hefz)*v(Mx))-(v(My)*v(Hefz)-v(Hefy)*v(Mz1))*v(My))'
G_dMz_torq 0 Mz cur='v(Is)*(v(Mx)*(v(Mz1)*Mpx-Mpz*v(Mx))-(v(My)*Mpz-Mpy*v(Mz1))*v(My))'
E_Mz1 Mz1 0 vol='v(Mz)' max='cos(v(thi))' min='-cos(v(thi))'
E_dMz_prec Mz_prec 0 vol='-(v(Mx)*v(Hefy)-v(Hefx)*v(My))'
E_dMz_damp Mz_damp 0 vol='-alpha*(v(Mx)*(v(Mz1)*v(Hefx)-v(Hefz)*v(Mx))-(v(My)*v(Hefz)-v(Hefy)*v(Mz1))*v(My))'
E_dMz_torq Mz_torq 0 vol='v(Is)*(v(Mx)*(v(Mz1)*Mpx-Mpz*v(Mx))-(v(My)*Mpz-Mpy*v(Mz1))*v(My))'
E_dMz_sum_prec_damp_torq Mz_sum_prec_damp_torq 0 vol='v(Mz_prec)+v(Mz_damp)+v(Mz_torq)'


.ends

