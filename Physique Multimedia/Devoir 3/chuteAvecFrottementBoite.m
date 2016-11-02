function g= chuteAvecFrottementBoite (q0)
%
% balle en chute libre et force visqueuse
%
k = 0.1 ;
A =  0.05^2 + 0.15^2 ;
masse = 0.075;
a = - k * A * q0(2:4) / masse;
a(3) = a(3) - 9.8;
g=[1 a q0(2) q0(3) q0(4)] ;


