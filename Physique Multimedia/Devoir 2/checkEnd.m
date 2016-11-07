function [ prise auSol ] = checkEnd( r )
    ray = 7.3/100/2;
    long = 30.48 / 2/100;
    prise = 0;
    auSol = 0;
    if ((r(1) >= -ray/2 && r(1) <= ray/2) && ...
            (r(2) + ray <= long && r(2) - ray >= -long) &&...
            (r(3) + ray <= 1.8 && r(3) - ray >= 0.8))
        prise = 1;
    end
    if (r(3)-ray < 0)
        auSol = 1;
    end
end

