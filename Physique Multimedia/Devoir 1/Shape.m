%% PHS4700 : Physique Multim�dia - Devoir 1 - Shape
% Cr�� : 17 sept. 2015 
%
% Auteurs : 
%
% * Alex Gagn� (1689761)
% * F�lix La Rocque Cartier (1621348)
% * Mathieux Gamache (1626377)
% * Konstantin Fedorov (1679095)
%
% Cette classe repr�sente une forme. Elles est h�rit�e par *Sphere* et
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