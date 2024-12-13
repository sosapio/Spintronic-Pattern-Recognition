% SimulationStartTime = time when noise is injected in the ckt
% SimulationEndTime = when noise injection is finished
% TimeIncrement = time step, change in noise value
% Mean = mean value of noise
% StDev = standard deviation of noise
% NominalVoltage = nominal value of noise, some DC value 
% Number = usually 1, how many files you want


function [out1, out2] = NoiseGen_2(SimulationStartTime, SimulationEndTime, TimeIncrement, Mean, StDev, NominalVoltage, Number)

fidx = fopen('pwlFile_30.in','w');

%  alpha = 0.03; Kb=1.38*10^-16; mu0 = 1; gamma=2*pi*2.8*10^6; lx=70e-7;ly=70e-7; lz=1.49e-7; Ms = 950;
% deltaT=TimeIncrement;
% Hth1 = sqrt(2*alpha*Kb*358/mu0/gamma/lx/ly/lz/Ms/10e-12)
% Hth2 = sqrt(2*alpha*Kb*358/mu0/gamma/lx/ly/lz/Ms/10e-12*0.4)
%StDev = Hth2
k=1;
for line=1:Number
    %% x axis
fprintf(fidx, 'Vth_x Hthx 0 PWL(');
for i=SimulationStartTime:TimeIncrement:SimulationEndTime
time=i*1e9;
x=randn(1)*StDev + Mean;
x=NominalVoltage + x;
out1(k)=time;
out2(k) = x;
fprintf(fidx,'%fn, ',time);
fprintf(fidx,'%4.5f ',x);
k=k+1;
end
fprintf(fidx, ')\n');

    %% yaxis
fprintf(fidx, 'Vth_y Hthy 0 PWL(');
for i=SimulationStartTime:TimeIncrement:SimulationEndTime
time=i*1e9;
x=randn(1)*StDev + Mean;
x=NominalVoltage + x;
out1(k)=time;
out2(k) = x;
fprintf(fidx,'%fn, ',time);
fprintf(fidx,'%4.5f ',x);
k=k+1;
end
fprintf(fidx, ')\n');

    %% z axis
fprintf(fidx, 'Vth_z Hthz 0 PWL(');
for i=SimulationStartTime:TimeIncrement:SimulationEndTime
time=i*1e9;
x=randn(1)*StDev + Mean;
x=NominalVoltage + x;
out1(k)=time;
out2(k) = x;
fprintf(fidx,'%fn, ',time);
fprintf(fidx,'%4.5f ',x);
k=k+1;
end
fprintf(fidx, ')\n');

end

fclose(fidx);

end
