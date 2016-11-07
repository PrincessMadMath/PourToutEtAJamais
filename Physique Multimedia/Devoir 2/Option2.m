function g= Option2 (q0)
%
% corps en chute libre et force visqueuse
%
a = - pi * 0.073^2 / 8 * 1.1644 * 1.45 * norm(q0(2:4))* q0(2:4) / 0.145;
a(3) = a(3) - 9.8;
g=[1 a q0(2) q0(3) q0(4)] ;

