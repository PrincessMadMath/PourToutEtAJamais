function [ prise vf x y z t ] = Devoir2( option,vi )
    dt0 = 0.02; %Delta t par defaut
    x = 18.44;
    y = 0;
    z = 2.1;
    t = 0;
    ri = [x;y;z];
    dt = dt0;
    prise = 0;
    auSol = 0;
    ray = 7.3/100/2;
    if option == 1
        a = [0;0;-9.8];
        i = 1;
        while prise == 0 && auSol == 0
            vn = vi + t(end)*a;
            dt = FixDTZoneCrit(dt0,vn,x,z);
            tn = t(end) + dt;
            r = ri + vi * tn + 0.5 * a * tn^2;
            x = [x r(1)];
            y = [y r(2)];
            z = [z r(3)];
            t = [t tn];
            i = i+1;
            [prise, auSol] = checkEnd(r);
        end
        vf = vn;
    else
        q0 = [0 vi.' ri.'];
        if(option == 2)
            [qSol, prise ] = SOLRK4C(q0,dt0,'Option2');
        elseif(option == 3)
            [qSol, prise ] = SOLRK4C(q0,dt0,'Option3');
        else
            warning('Option invalide ');
        end
        vf = qSol(end, 2:4);
        x = qSol(:,5);
        y = qSol(:,6);
        z = qSol(:,7);
        t = qSol(:,1);
    end
end

