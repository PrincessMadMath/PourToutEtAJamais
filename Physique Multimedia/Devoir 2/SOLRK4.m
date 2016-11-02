function [ qSol, prise ] = SOLRK4(q0, dt0, fonctiong, priseOnly )
    i = 1;
    prise = 0;
    auSol = 0;
    qSol (1 ,:) = q0;
    while prise == 0 && auSol == 0 && (~priseOnly || qSol(end,5)>0)
        qSol (i+1 ,:)= SEDRK4 ( qSol (i ,:) , dt0 ,fonctiong);
        
        [prise, auSol] = checkEnd(qSol(end,5:7));
        i = i+1;
    end
end

