function [ qSol, prise ] = SOLRK4C( q0, dt0, fonctiong )
    [qSol, prise] = SOLRK4(q0,dt0,fonctiong,0);
    dtn = dt0;
    n = 1;
    e = 1;
    while e >= 0.001 && n < 10
        dtn = dtn / 2;
        [q2, prise] = SOLRK4(q0, dtn, fonctiong,0);
        [pos,ignore] = size(q2(:,1));
        if(mod(pos,2) == 1)
            pos = pos+1;
        end
        
        posPrecis = pos - 1;
        pos = pos/2;
        err = q2(posPrecis,5:7)-qSol(pos,5:7); 
        avg = q2(posPrecis,5:7)+ qSol(pos,5:7) /2.; 
        e = max(abs(err./avg));
        qSol = q2;
        n = n+1;
    end
    
    
    % check for collision with at x = 0
    
    ray = 7.3 /100 /2;
    reprise = 0;
    lastQ = qSol(end,:);
    if(~prise && lastQ(7) < ray && lastQ(5) < 0)
        I = find(qSol(:,5)>0);
        before0 = I(end);
        v = norm(qSol(before0,2:4));
        dtp = ray / v / 2; % delta t pour que le deplacement soit environ 1/4 de la balle
        [qSol2, prise] = SOLRK4(qSol(before0,:), dtp, fonctiong,1);
        if(prise)
            reprise = 1;
            qSol = [qSol(1:before0-1,:); qSol2];
        end
    end
    
    % add precision at collision point
    if(~reprise)
        v = norm(qSol(end-1,2:4));
        dtp = ray / v / 2; % delta t pour que le deplacement soit environ 1/4 de la balle
        [qSol2, ignore2] = SOLRK4(qSol(end-1,:),dtp,fonctiong,0);
        qSol = [qSol(1:end-2,:); qSol2];
    end
    
    
    
end

