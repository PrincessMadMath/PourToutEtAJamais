%% PHS4700 : Physique Multim�dia - Devoir 1 - Sphere
% Cr�� : 17 sept. 2015 
%
% Auteurs : 
%
% * Alex Gagn� (1689761)
% * F�lix La Rocque Cartier (1621348)
% * Mathieux Gamache (1626377)
% * Konstantin Fedorov (1679095)
%
% Cette classe repr�sente une sphere. Son constructeur calcule son volume,
% masse et moment d'inertie

classdef Sphere < Shape
    properties
        weight
        volume
    end
    methods
        function r = Sphere( cM, dens, ray)
            args{1} = cM;
            args{2} = dens;
            r@Shape(args{:});
            r.volume = ray^3 * pi *4/3;
            r.weight = r.volume * r.density;
            mom = ray^2 * 2 /5 * r.weight;
            r.moment = [mom,0,0;0,mom,0;0,0,mom];
        end 
    end
end