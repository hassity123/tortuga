function y=slidingGoertzelDFTDemo(x,N)
% Sliding Goertzel DFT Demo {see slidingGoertzelDFT()}
% Author: Scott Watson

magdata(1:size(x,2),1:N)=0;
phasedata(1:size(x,2),1:N)=0;

for k=1:N
	result=slidingGoertzelDFT(x,k,N);
	magdata(:,k)=abs(real(result))+abs(imag(result));
	phasedata(:,k)=atan2(imag(result),real(result));
end

subplot(1,2,1)
surf(magdata)
xlabel('k')
ylabel('N')
zlabel('Amplitude')

subplot(1,2,2)
k=4;
plot((phasedata(:,k))*(180/pi))
title(['phase of bin #' num2str(k)])
xlabel('N')
ylabel('phase')
