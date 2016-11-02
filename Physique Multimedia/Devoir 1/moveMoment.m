%% PHS4700 : Physique Multimédia - Devoir 1 - moveMoment
% Créé : 17 sept. 2015 
%
% Auteurs : 
%
% * Alex Gagné (1689761)
% * Félix La Rocque Cartier (1621348)
% * Mathieux Gamache (1626377)
% * Konstantin Fedorov (1679095)
%
% Cette fonction sert à calculer le moment d'inertie d'une forme par
% rapport à une position donnée.
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

