classdef Rayon
    %Rayon Représente un rayon de lumière
    
    properties
        DirectionOriginale
        DirectionCourante
        ACollision
        EstRejete
        Longueur
        PointCourant
        Couleur
    end
    
    methods
        function ray = Rayon(pointInit, pointDirection)
            if  nargin > 0
                dir = pointDirection - pointInit;
                ray.Longueur = 0;
                ray.DirectionOriginale = dir / norm(dir);
                ray.DirectionCourante = ray.DirectionOriginale;
                ray.PointCourant = pointInit;
                ray.EstRejete = false;
                ray.ACollision = false;
            end
            
        end
    end
    
end

