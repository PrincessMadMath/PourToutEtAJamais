%% PHS4700 : Physique Multim�dia - Devoir 1 - moveMoment
% Cr�� : 17 sept. 2015 
%
% Auteurs : 
%
% * Alex Gagn� (1689761)
% * F�lix La Rocque Cartier (1621348)
% * Mathieux Gamache (1626377)
% * Konstantin Fedorov (1679095)
%
% Cette fonction sert � calculer le moment d'inertie d'une forme par
% rapport � une position donn�e.
%
% $$ I_{d} = I_{c} + m
% \left (\begin{array}{ccc} (d_{c,y}^{2} + d_{c,z}^{2}) & -d_{c,x}d_{c,y} & -d_{c,x}d_{c,z}\\
% -d_{c,x}d_{c,y} & (d_{c,x}^{2} +d_{c,z}^{2}) & -d_{c,y}d_{c,z} \\
% -d_{c,x}d_{c,z} & -d_{c,y}d_{c,z} & (d_{c,x}^{2} + d_{c,y}^{2})\end{array} \right) $$
% 

function [r] = moveMoment(pointFinal, shape)
    d = shape.centerOfMass - pointFinal;
    matTrans = [ d(2)^2 + d(3)^2 , -d(1)*d(2), -d(1)*d(3)
                 -d(2)*d(1), d(1)^2 + d(3)^2, -d(2)*d(3)
                 -d(1)*d(3), -d(2)*d(3), d(2)^2 + d(1)^2 ];
    r = shape.weight * matTrans + shape.moment;
end

