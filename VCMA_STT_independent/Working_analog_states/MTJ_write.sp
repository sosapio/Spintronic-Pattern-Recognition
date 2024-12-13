************************************************************************************
************************************************************************************
** Title: VCMA-assisted STT switching
** Author: Jeehwan Song, VLSI Research Lab @ UMN (songx944@umn.edu)
** This is modified based on previous STT model by Jongyeon Kim, VLSI Research Lab @ UMN (kimx2889@umn.edu)
************************************************************************************
** This run file simulates the dynamic motion of  MTJ.
** # Instruction for simulation
** 1. Set the MTJ dimensions and material parameters.
** 2. Select initial state of free layer(P/AP).
** 3. Select VCMA coefficient and external magnetic field 
**    ex. vcma_coeff = 33e-15, 105e-15, 290e-15 [J*V^-1*m^-1]
**        Hext_x = 200 [Oe]
** 4. Adjust voltage pulse parameters(voltage-amplitude, pulsewidth) for four switching directions.
**    ex. voltage pulse (1): P-to-P, AP-to-P switching @ ini='0/1'
**     	  voltage pulse (2): P-to-AP, AP-to-AP switching @ ini='0/1'
************************************************************************************
** # Description of parameters
** lx,ly,lz: width, length, and thickness of free layer
** tox: MgO thickness
** Ms0: saturation magnetizaion at 0K
** P0: polarization factor at 0K 
** alpha: damping factor
** Tmp0: temperature [K]
** RA0: resistance-are Product at parallel state
** MA: magnetic anisotropy (MA=0:In-plane,MA=1:Perpendicular)
**     also sets magnetization in pinned layer, MA=0:[0,1,0],MA=1:[0,0,1]
** ini: initial state of free layer (ini=0:Parallel,ini=1:Anti-parallel)
** tc: critical thickness (single interface: tc=1.5nm, Double interface: tc=3nm)
** vcma_coeff: coefficient of voltage-controlled magnetic anisotropy (VCMA)
** VE: voltage for VCMA-effect
** VSTT: voltage for STT-effect (VSTTP:positive VSTT, VSTTN:negative VSTT)
** PW_VE, PW_VSTT: pulsewidths of VE, VSTT
** Hext_x, H_ext_y, Hext_z: external magnetic field toward x-asix, y-axis, z-axis
************************************************************************************
.include 'MTJ_model.inc'

*** Options ************************************************************************
.option post
	+ runlvl=3
	+ ITL4=100
.save

*** Device Parameters of MTJ *******************************************************
*.param lx='70n' ly='70n' lz='1.49n' Ms0='950' P0='0.54' alpha='0.025' Tmp0='358' RA0='130' MA='1' tc='1.5n' tox='1.4n' ini = '0' vcma_coeff = '33e-15'
*.param pi='355/113'

.param lx='70n' ly='70n' lz='1.49n' Ms0='950' P0='0.54' alpha='1.6' Tmp0='358' RA0='130' MA='1' tc='1.5n' tox='1.4n' ini = '0' vcma_coeff = '76e-15'
.param pi='355/113' 

*** MTJ ****************************************************************************
XMTJ 1 0 2 4 Mx My Mz MTJ lx='lx' ly='ly' lz='lz' Ms0='Ms0' P0='P0' alpha='alpha' Tmp0='Tmp0' RA0='RA0' MA='MA' ini='ini' tc='tc' tox='tox' 

*** Experimental Parameters of MTJ ************************************************* 
*.param Tmp0='358' Hext_x = '100' Hext_y = '0' Hext_z = '-200'
*.param VE = '2.45' PW_VE = '0.48n' VSTTP = '1.8' VSTTN = '-1.8' PW_VSTT = '2.00n'
*.param t0 = '0.5n'			$time before VE pulse
*.param t1 = 'VE*0.02n'  		$rising time from 0V to VE
*.param t2='(VE-VSTTN)/(VE/t1)'
*.param t3='(0-VSTTN)/(VE/t1)'

*** Experimental Parameters of MTJ ************************************************* 
.param Tmp0='358' Hext_x = '200' Hext_y = '0' Hext_z = '20'
.param VE = '2.45' VSTTP = '1.8' VSTTN = '-1.8' PW_VE = '0.48n' PW_VSTT = '2.00n'
.param t0 = '0.5n'			$time before VE pulse
.param t1 = 'VE*0.02n'  		$rising time from 0V to VE
.param t2='(VE-VSTTN)/(VE/t1)'
.param t3='(0-VSTTN)/(VE/t1)'


*** RC-delay for energy barrier time constant **************************************
.param R_Eb = '0k' C_Eb = '0f' 

R_Eb 1 2 'R_Eb'	$resistance of RC-delay model
C_Eb 2 0 'C_Eb' $capacitance of RC-delay model

*** Analysis ***********************************************************************
.param pw='64ns'
.tran 1p 'pw' START=1.0e-18  uic  $sweep PW_VE 0.45n 0.50n 0.01n

*** Voltage pulse(1a): VE ************************************
*Vpwl 1 0 pwl (0 0 't0' 0 't0+t1' 'VE' 't0+t1+PW_VE' 'VE' 't0+t1+PW_VE+t2' 'VSTTN' 't0+t1+PW_VE+t2+PW_VSTT' 'VSTTN' 't0+t1+PW_VE+t2+PW_VSTT+t3' 0 'pw' 0)
*Vpwl 1 0 pwl (0 0 't0' 0 't0+t1' 'VE' 't0+t1+PW_VE+5n' 'VE' 't0+t1+PW_VE+t2+t3+5n' 0 'pw' 0)


*V_VCMA 1 0 pwl (0 0 't0' 0 't0+t1' 'VE' 't0+t1+PW_VE' 'VE' 't0+t1+PW_VE+t3' 0 'pw' 0)
*V_VCMA 1 0 pwl (0 0 0.5n 0 0.55n 0.8 1.03n 0.8 1.53n 0 'pw' 0)
*V_VCMA 1 0 pwl (0 0 0.5n 0 0.55n 2.2 1.55n 2.2 2.00n 0.6 3.00n 0)
*V_VCMA 1 0 pwl (0 0 7.65n 0 8n 1.5 16n 1.5 16.2n 0 23.8n 0 24n 1.5 32n 1.5 32.2n 0 39.8n 0 40n 1.5 48n 1.5 48.2n 0 55.8n 0 56n 1.5 64n 1.5 64.2n 0 71.8n 0 72n 1.5 80n 1.5 80.2n 0 87.8n 0 88n 1.5 96n 1.5 96.2n 0 100n 0)
*V_VCMA 1 0 pwl (0 0 7.65n 0 8n 1.5 16n 1.5 16.2n 0.5 23.8n 0.5 24n 1.5 32n 1.5 32.2n 0.5 39.8n 0.5 40n 1.5 48n 1.5 48.2n 0.5 55.8n 0.5 56n 1.5 64n 1.5 64.2n 0.5 71.8n 0.5 72n 1.5 80n 1.5 80.2n 0.5 87.8n 0.5 88n 1.5 96n 1.5 96.2n 0.5 100n 0.5)
.param Vmax=1.5  $ Maximum voltage of the pulse
.param Vres=0.7  $ Residual voltage (1/3 of Vmax)
.param Tperiod=16n  $ Period of the pulse
*V_VCMA 1 0 pwl (0 0 'Tperiod*0.478125' 0 'Tperiod*0.5' Vmax 'Tperiod' Vmax 'Tperiod+Tperiod*0.0125' Vres 'Tperiod+Tperiod*0.4875' Vres 'Tperiod+Tperiod*0.5' Vmax '2*Tperiod' Vmax '2*Tperiod+Tperiod*0.0125' Vres '2*Tperiod+Tperiod*0.4875' Vres '2*Tperiod+Tperiod*0.5' Vmax '3*Tperiod' Vmax '3*Tperiod+Tperiod*0.0125' Vres '3*Tperiod+Tperiod*0.4875' Vres '3*Tperiod+Tperiod*0.5' Vmax '4*Tperiod' Vmax '4*Tperiod+Tperiod*0.0125' Vres '4*Tperiod+Tperiod*0.4875' Vres '4*Tperiod+Tperiod*0.5' Vmax '5*Tperiod' Vmax '5*Tperiod+Tperiod*0.0125' Vres '5*Tperiod+Tperiod*0.4875' Vres '5*Tperiod+Tperiod*0.5' Vmax '6*Tperiod' Vmax '6*Tperiod+Tperiod*0.0125' Vres '6*Tperiod+Tperiod*0.4875' Vres '6*Tperiod+Tperiod*0.5' Vmax '7*Tperiod' Vmax '7*Tperiod+Tperiod*0.0125' Vres '7*Tperiod+Tperiod*0.4875' Vres '7*Tperiod+Tperiod*0.5' Vmax '8*Tperiod' Vmax '8*Tperiod+Tperiod*0.0125' Vres '8*Tperiod+Tperiod*0.4875' Vres '8*Tperiod+Tperiod*0.5' Vmax '9*Tperiod' Vmax '9*Tperiod+Tperiod*0.0125' Vres '9*Tperiod+Tperiod*0.4875' Vres '9*Tperiod+Tperiod*0.5' Vmax '10*Tperiod' Vmax '10*Tperiod+Tperiod*0.0125' Vres '10*Tperiod+Tperiod*0.4875' Vres)
*V_VCMA 1 0 pwl (0 0 8n 0 8.2n 1.0 15.8n 1.0 16n 2.0 24n 2.0 24.2n 3.0 31.8n 3.0 32n 4.0 40n 4.0 40.2n 5.0 47.8n 5.0 48n 6.0 56n 6.0 56.2n 7.0 63.8n 7.0 64n 8.0 72n 8.0 72.2n 9.0 79.8n 9.0 80n 10.0 88n 10.0 88.2n 11.0 95.8n 11.0 96n 12.0 104n 12.0 104.2n 13.0 111.8n 13.0 112n 14.0 120n 14.0)
V_VCMA 1 0 pwl (0 0 8n 0 8.2n 0.5 16n 0.5 16.2n 1.0 24n 1.0 24.2n 1.5 32n 1.5 32.2n 2.0 40n 2.0 40.2n 2.45 48n 2.45 48.2n 2.45 48.5n 2.95 54.8n 2.95 55.0n 3.45 63.8n 3.45)
*V_VCMA 1 0 pwl (0 0 8n 0 8.2n 0.5 16n 0.5 16.2n 1.0 24n 1.0 24.2n 1.5 32n 1.5 32.2n 2.0 40n 2.0 40.2n 2.45 48n 2.45 48.2n 2.45 48.5n 2.95 54.8n 2.95 55.0n 3.45 63.8n 3.45 64.0n 0 66.0n 0)


*** Voltage pulse(1b): Negative VSTT (VSTTN) ************************************
*V_STT 4 0 pwl (0 0 't0' 0 't0+t3' 'VSTTN' 't0+t3+PW_VSTT' 'VSTTN' 't0+t3+PW_VSTT+t3' 0 'pw' 0)
*V_STT 4 0 pwl (0 0 't0' 0 't0+t3' 'VSTTN' 't0+t3+PW_VSTT' 'VSTTN' 't0+t3+PW_VSTT+t3' 0 'pw' 0)
*V_STT 4 0 pwl (0 0 0.5n 0 0.75n -1.8 2.75n -1.8 3.00n 0 'pw' 0)
*V_STT 4 0 pwl (0 0 0.5n 0 0.75n -1.5 3.75n -1.5 4.25n 0)
*V_STT 4 0 pwl (0 0 0.5n 0 0.75n 0 3.75n 0 4.25n 0)
*V_STT 4 0 pwl (0 0 8n 0 8.2n 1.0 15.8n 1.0 16n 2.0 24n 2.0 24.2n 3.0 31.8n 3.0 32n 4.0 40n 4.0 40.2n 5.0 47.8n 5.0 48n 6.0 56n 6.0 56.2n 7.0 63.8n 7.0 64n 8.0 72n 8.0 72.2n 9.0 79.8n 9.0 80n 10.0 88n 10.0 88.2n 11.0 95.8n 11.0 96n 12.0 104n 12.0 104.2n 13.0 111.8n 13.0 112n 14.0 120n 14.0)





*** switching time measurement
.meas t_start		when v(1)='VE/2' rise=1
.meas t_finish		when v(Mz)=-0.5	fall=1 TD='t0+t1+PW_VE+t2'  
.meas Tsw		param='t_finish-t_start'
.meas final_State	find v(Mz) at 'pw'

*** switching energy measurment
.meas t_mid		when v(1)='0' fall=1
.meas i_avg_ve 		AVG i(XMTJ.ve1) from='t0'	to='t_mid'
.meas i_avg_vstt 	AVG i(XMTJ.ve1) from='t_mid'	to='t_finish'
.meas v_avg_ve 		AVG V(1) 	from='t0'	to='t_mid'
.meas v_avg_vstt	AVG V(1) 	from='t_mid'	to='t_finish'

.end