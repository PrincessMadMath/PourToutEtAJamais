function [ray] = CollisionRayon( ray, normale, pt, n0, n1)
%UNTITLED5 Summary of this function goes here
%   Detailed explanation goes here


    ray.Longueur = ray.Longueur + norm(ray.PointCourant - pt);
    ray.PointCourant = pt;
    
    ui = ray.DirectionCourante / norm(ray.DirectionCourante);
    j = cross(ui,normale);
    j = j / norm(j);
    k = cross(normale,j);
    
    %refract
    st = (n0 / n1)*dot(ui, k);
    if(st >= 1) % possibilite de reflexion
        %angleMax = asin(n1/n0);
       % angleInc = asin(dot(k,ui));
       % if(angleInc > angleMax || angleInc < -angleMax)
            %Reflexion
            ray.DirectionCourante = ui - 2 *normale*dot(ui,normale);
            return
      %  end
    else
        ray.DirectionCourante = -normale*sqrt(1-st^2)+k*st;
    end
end

