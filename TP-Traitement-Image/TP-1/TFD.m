function [f0,fdata] = TFD(Fe,data)

m = length(data);
n = pow2(nextpow2(m));
f0 = (-n/2:n/2-1)*(Fe/n);
fdata = fftshift(fft(data,n));

end