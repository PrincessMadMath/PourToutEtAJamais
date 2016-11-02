function qs= SEDRK4 (q0 , Deltat , fonctiong )
%
% Solution ED dq/dt= fonctiong (q,t)
% Methode de Runge - Kutta d’ ordre 4
% qs : vecteur final [tf q(tf )]
% q0 : vecteur initial [ti q(ti )]
% Deltat : intervalle de temps
% fonctiong : membre de droite de ED.
% Ceci est un m- file de matlab
% qui retourne [1 dq/dt(ti )]
%
k1= feval ( fonctiong ,q0 )* Deltat ;
k2= feval ( fonctiong ,q0+k1 /2)* Deltat ;
k3= feval ( fonctiong ,q0+k2 /2)* Deltat ;
k4= feval ( fonctiong ,q0+k3 )* Deltat ;
qs=q0 +( k1 +2* k2 +2* k3+k4 )/6;

