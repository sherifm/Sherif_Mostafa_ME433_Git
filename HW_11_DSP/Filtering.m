load accel_data_1Hz_1000samps.txt
z_raw = transpose(accel_data_1Hz_1000samps(1:end,1));%raw unflitered z_accel data
z_MAF = transpose(accel_data_1Hz_1000samps(1:end,2));%pre-processing filter MAF filter implemented on PIC

%####################Post-processing filter design MAF and FIR######
b=fir1(10,0.15);
freqz(b);%Plot the frequency response of the fir filter
z_fir=conv(b,z_raw);%Convolution of the raw data and the filter (post-processing)

%#####################plot the z signals############################
% f=figure();
% set(f,'name','z_(MAF)','numbertitle','off');
% plot(z_raw,'color','r')
% hold on
% plot(z_MAF);
% plot(z_fir,'color','g')
% legend('z-raw','z-MAF','z-fir')

%#####################plot the FFT for z_raw########################
% hold on
% f=figure();
% set(f,'name','z_raw_FFT','numbertitle','off');
% plotFFT(z_raw);
% fprintf('The maginitude of the spike at 0 Hz should be eaual to %.1d, the abolute average of the z_accel signal\n',abs(mean(z_raw)));

%#####################plot the FFT for z_MAF########################
% hold on
% f=figure();
% set(f,'name','z_MAF_FFT','numbertitle','off');
% plotFFT(z_MAF);
% fprintf('The maginitude of the spike at 0 Hz should be eaual to %.1d, the abolute average of the z_accel signal\n',abs(mean(z_raw)));



