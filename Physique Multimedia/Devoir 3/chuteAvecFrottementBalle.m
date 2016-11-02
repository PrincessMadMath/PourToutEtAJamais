function g= chuteAvecFrottementBalle (q0)
%
% balle en chute libre et force visqueuse
%
k = 0.1 ;
A =  pi * 0.0335^2;
masse = 0.058;
a = - k * A * q0(2:4) / masse;
a(3) = a(3) - 9.8;
g=[1 a q0(2) q0(3) q0(4)] ;

