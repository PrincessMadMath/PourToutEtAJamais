function [ dt ] = FixDTZoneCrit( dt0,vn,x,z )
    ray = 7.3 / 100 /2;
    if(x(end) <= -vn(1)*dt0*2 && x(end)>= vn(1)*dt0*2 || z(end)<vn(3)*dt0*-2)
        dt = ray / norm(vn);
    else
        dt = dt0;
    end
end

