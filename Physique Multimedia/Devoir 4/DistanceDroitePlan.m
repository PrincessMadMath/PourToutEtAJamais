function [ptCol, distCol] = DistanceDroitePlan(dirDroite, ptDroite, nPlan, ptPlan)
%UNTITLED6 Summary of this function goes here
%   Detailed explanation goes here
    if(dot(dirDroite, nPlan) == 0)
        ptCol = [0 0 0];
        distCol = 0;
    else
        w =ptDroite-ptPlan;
        u = dirDroite / norm(dirDroite);
        distCol = dot(-nPlan,w) / dot(u, nPlan);
        ptCol = ptDroite + distCol * u;
    end

end

