%% PHS4700 : Physique Multimédia - Devoir 1 - Cylindre
% Créé : 17 sept. 2015 
%
% Auteurs : 
%
% * Alex Gagné (1689761)
% * Félix La Rocque Cartier (1621348)
% * Mathieux Gamache (1626377)
% * Konstantin Fedorov (1679095)
%
% Cette classe représente un cylindre. Son constructeur calcule son volume,
% masse et moment d'inertie

classdef Cylinder < Shape
    properties
        weight
        volume
    end
    methods
        function r = Cylinder(cM, dens, ray, long, axe) 
            args{1} = cM;
            args{2} = dens;
            r@Shape(args{:});
            r.volume = ray^2 * pi *long;
            r.weight = r.volume * r.density;
            % cylindre aligne sur quel axe 
            %axe = 1: x, axe = 2 :y, axe = 3 : z
            momFir = ray ^2 * r.weight /2;
            momSec = ray ^2 * r.weight /4 + long ^2 * r.weight /12 ;
            
            switch axe
                case 1 
                    r.moment = [momFir,0,0
                              0,momSec,0
                              0,0,momSec];
                case 2
                    r.moment = [momSec,0,0
                              0,momFir,0
                              0,0,momSec];
                otherwise
                    r.moment = [momSec,0,0
                              0,momSec,0
                              0,0,momFir];
            end
        end 
    end
end