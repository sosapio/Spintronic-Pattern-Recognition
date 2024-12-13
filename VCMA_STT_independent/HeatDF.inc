************************************************************************************
************************************************************************************
** Title:  HeatDF.inc
** Author: Jongyeon Kim, VLSI Research Lab @ UMN
** Email:  kimx2889@umn.edu
************************************************************************************
** Joule heating in the MTJ increases the internal temperature resulting in larger Hf. 
** Also, both Ms and P have a temperature dependency.
** The  Tmp is fed to LLG_solver.inc and Resistance.inc.
************************************************************************************

.subckt HD Ihd Tmp lx='lx' ly='ly' lz='lz' Tmp0='Tmp0'

*** Unit length for diffusion ******************
.param dL='lz/7*1e7'  $ [cm]

*** Volumetric specific heat capacity **********
*** f:Fe, o:MgO ********************************
.param Cvf='3.54'     $ [J/cm3*K]
.param Cvo='0.004'  

*** Heat conductivity **************************
.param Kf='0.802'     $ [W/cm*K]
.param Ko='0.6'

*** RC conversion for diffusion equation *******
.param Cf='Cvf'
.param Co='Cvo'
.param Rf='dL*dL/Kf'
.param Ro='dL*dL/Ko'

*** Head diffusion by distributed RC model *****
Rl01 Tmp l1 'Rf'
Rl12 l1 l2 'Rf'
Rl23 l2 l3 'Rf'
Rl34 l3 l4 'Rf'
Rl45 l4 l5 'Rf'
Rl56 l5 l6 'Rf'
Rl67 l6 l7 'Rf'
Vl l7 0 'Tmp0'

Cl0 Tmp 0 'Cf'
Cl1 l1 0 'Cf'
Cl2 l2 0 'Cf'
Cl3 l3 0 'Cf'
Cl4 l4 0 'Cf'
Cl5 l5 0 'Cf'
Cl6 l6 0 'Cf'

.param rho='1e-5'  $ [ohm*cm]
R_Ihd Ihd 0 '1'
G_Tmp 0 Tmp cur='rho*(v(Ihd)/(lx*ly*1e4))^2/Kf'


Rm01 Tmp m1 'Ro'
Rm02 m1 m2 'Ro'
Rr01 m2 r1 'Rf'
Rr12 r1 r2 'Rf'
Rr23 r2 r3 'Rf'
Rr34 r3 r4 'Rf'
Rr45 r4 r5 'Rf'
Rr56 r5 r6 'Rf'
Rr67 r6 r7 'Rf'
Vr r7 0 'Tmp0'

Cm0 Tmp 0 'Co'
Cm1 m1 0 'Co'
Cr0 m2 0 'Cf'
Cr1 r1 0 'Cf'
Cr2 r2 0 'Cf'
Cr3 r3 0 'Cf'
Cr4 r4 0 'Cf'
Cr5 r5 0 'Cf'
Cr6 r6 0 'Cf'


.ic v(l1)='Tmp0'
.ic v(l2)='Tmp0'
.ic v(l3)='Tmp0'
.ic v(l4)='Tmp0'
.ic v(l5)='Tmp0'
.ic v(l6)='Tmp0'

.ic v(Tmp)='Tmp0'
.ic v(m1)='Tmp0'
.ic v(m2)='Tmp0'

.ic v(r1)='Tmp0'
.ic v(r2)='Tmp0'
.ic v(r3)='Tmp0'
.ic v(r4)='Tmp0'
.ic v(r5)='Tmp0'
.ic v(r6)='Tmp0'


.ends
