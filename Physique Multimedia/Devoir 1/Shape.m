%% PHS4700 : Physique Multimédia - Devoir 1 - Shape
% Créé : 17 sept. 2015 
%
% Auteurs : 
%
% * Alex Gagné (1689761)
% * Félix La Rocque Cartier (1621348)
% * Mathieux Gamache (1626377)
% * Konstantin Fedorov (1679095)
%
% Cette classe représente une forme. Elles est héritée par *Sphere* et
% *Cylinder*.
classdef Shape
   properties
       centerOfMass = [0;0;0]
       density
       moment = [0,0,0
                 0,0,0
                 0,0,0]
   end
   methods
       function r = Shape(cM, dens)
           r.centerOfMass = cM;
           r.density = dens;
       end
   end
end