function g= Option3 (q0)
%
% corps en chute libre et force visqueuse et truc weird
%
vr = [0 ;0 ;50];
v = q0(2:4);
av = - pi * 0.073^2 / 8 * 1.1644 * 1.45 * norm(v)* v / 0.145;
ag = [0 0 -9.8];
am = pi * 0.073^2 / 8  * 1.1644 * 0.0016 *  norm(v) * cross(vr,v) / 0.145 ;
a= av+ ag +am;
g=[1 a q0(2) q0(3) q0(4)] ;


