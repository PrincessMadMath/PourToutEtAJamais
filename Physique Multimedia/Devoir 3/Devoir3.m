function [ Dev, vbaf, vbof, rba, rbo, tc ] = Devoir3( wboitei,vballei,tballe )
%Devoir3 Simule la collision d'une balle et d'une boîte de conserve
%   
dtbase = 0.01;
deltat = dtbase;
ballelancee = false;
rballe = 0.0335;
mballe = 0.058;
hboite = 0.15;
rboite = 0.05;
mboite = 0.075;
rayonMax = sqrt(hboite^2+ rboite^2) + rballe;

qboite = zeros(500,7);
qballe = zeros(500,7);
qballe(1,:) = [0 vballei.' 0 0 2];
qboite(1,:) = [0 0 0 0 3 0 10;];
iter = 1;

precision = 0.001;

qPlan1a = [0 0 hboite/2];
qPlan1b = [rboite 0 hboite/2];
qPlan1c = [0 rboite hboite/2];

qPlan2a = [0 0 -hboite/2];
qPlan2b = [0 rboite -hboite/2];
qPlan2c = [rboite 0 -hboite/2];

qshit = zeros(500,3);
qshit2 = zeros(500,3);
qshit(1,:) = qPlan1a + qboite(iter,5:7);
qshit2(1,:) = qPlan2a + qboite(iter,5:7);

fin = false;
Dev = false;
pointCollision = [0 0 0];

while(iter < 500 && ~fin)
    iter = iter+ 1;
    curTime = qballe(iter-1,1) + deltat;
    
    %calcul des positions
    if(ballelancee)
        
        theta = wboitei.' * curTime;
        qballe(iter,:)= SEDRK4(qballe(iter-1,:), deltat, 'chuteAvecFrottementBalle');
        qboite(iter,:) = SEDRK4(qboite(iter-1,:), deltat, 'chuteAvecFrottementBoite');
        qshit(iter,:) = QARotation(qPlan1a,norm(theta),theta/norm(theta)) + qboite(iter,5:7);
        qshit2(iter,:) = QARotation(qPlan2a,norm(theta),theta/norm(theta)) + qboite(iter,5:7);
    else
        theta = wboitei.' * curTime;
        qballe(iter,:) = [curTime qballe(1,2:7)];
        qboite(iter,:) = SEDRK4(qboite(iter-1,:), deltat, 'chuteAvecFrottementBoite');
        
        qshit(iter,:) = QARotation(qPlan1a,norm(theta),theta/norm(theta)) + qboite(iter,5:7);
        qshit2(iter,:) = QARotation(qPlan2a,norm(theta),theta/norm(theta)) + qboite(iter,5:7);
        if(curTime == tballe)
            deltat = dtbase;
            ballelancee = true;
        elseif(curTime + deltat > tballe)
            deltat = tballe - curTime;
        end
    end
    
     
    %Detecter si collisions general
    posBalle = qballe(iter,5:7);
    posBoite = qboite(iter,5:7);
    
    centreACentre = posBalle - posBoite;
    
    dist = norm(centreACentre);
    
    if(dist <= rayonMax * 2 && deltat > dtbase /2)
        deltat = dtbase/2;
    end
    
    if(dist <= rayonMax)
        %detect collision avancee
        if(norm(wboitei)>0)
            theta = wboitei.' * curTime;
            qPlan1ac = QARotation(qPlan1a,norm(theta),theta/norm(theta))+posBoite;
            qPlan1bc = QARotation(qPlan1b,norm(theta),theta/norm(theta))+posBoite;
            qPlan1cc = QARotation(qPlan1c,norm(theta),theta/norm(theta))+posBoite;

            qPlan2ac = QARotation(qPlan2a,norm(theta),theta/norm(theta))+posBoite;
            qPlan2bc = QARotation(qPlan2b,norm(theta),theta/norm(theta))+posBoite;
            qPlan2cc = QARotation(qPlan2c,norm(theta),theta/norm(theta))+posBoite;
        else
            qPlan1ac = qPlan1a+posBoite;
            qPlan1bc = qPlan1b+posBoite;
            qPlan1cc = qPlan1c+posBoite;

            qPlan2ac = qPlan2a+posBoite;
            qPlan2bc = qPlan2b+posBoite;
            qPlan2cc = qPlan2c+posBoite;
        end
        
        droitePlan11 = qPlan1ac - qPlan1bc;
        droitePlan12 = qPlan1ac - qPlan1cc;
        
        normalePlan1 = cross(droitePlan11, droitePlan12) / norm(cross(droitePlan11, droitePlan12));
        
        
        droitePlan21 = qPlan2ac - qPlan2bc;
        droitePlan22 = qPlan2ac - qPlan2cc;
        
        normalePlan2 = cross(droitePlan21, droitePlan22) / norm(cross(droitePlan21, droitePlan22));
        
        
        %Projection sur deux plans
        distPlan1 = dot(normalePlan1, (posBalle - qPlan1ac));
        distPlan2 = dot(normalePlan2, (posBalle - qPlan2ac));
        %check distance de projection
        if(distPlan1 <= rballe+precision && distPlan2 <= rballe+precision)
            %si un positif et > que rayon balle : pas de collision, ne rien
            %faire
            if(distPlan1 > 0 && distPlan1 <= rballe+precision)
                %si positif, <= rayon balle : potentielle collision -> check
                    %distance au centre du plan concerné
                projectionOrtho = posBalle - normalePlan1 * distPlan1;
                distAuCentre = norm(qPlan1ac - projectionOrtho);
                distProjASurface = sqrt(rballe^2 - distPlan1^2);
                distSurfaceACentre = distAuCentre - distProjASurface;
                if(distSurfaceACentre <= rboite && distAuCentre <=rboite)
                    %collision directe balle cercle
                    if(distPlan1 >= rballe - precision && distPlan1 <= rballe+precision)
                        %collision! yay fin.
                        pointCollision = projectionOrtho;
                        fin = true;
                        Dev = true;
                    else
                        %raffiner resultats 
                        deltat = deltat/2;
                        iter = iter-1;
                    end
                    
                elseif(distAuCentre >= rboite)
                    %collision de côté
                    if(distSurfaceACentre <= rboite + precision && distSurfaceACentre >= rboite - precision)
                        %collision! yay fin
                        pointCollision = (qPlan1ac - projectionOrtho)/distAuCentre * distProjASurface + projectionOrtho;
                        fin = true;
                        Dev = true;
                    elseif(distSurfaceACentre <rboite)
                        %raffiner resultats
                        deltat = deltat/2;
                        iter = iter-1;
                    end
                    %sinon pas collision
                end
                    
            elseif(distPlan2 > 0 && distPlan2 <= rballe)
                %copier le truc en haut juste en changeant plan 1 pour
                %plan2
                projectionOrtho = posBalle - normalePlan2 * distPlan2;
                distAuCentre = norm(qPlan2ac - projectionOrtho);
                distProjASurface = sqrt(rballe^2 - distPlan2^2);
                distSurfaceACentre = distAuCentre - distProjASurface;
                if(distSurfaceACentre <= rboite && distAuCentre <=rboite)
                    %collision directe balle cercle
                    if(distPlan2 >= rballe - precision && distPlan2 <= rballe+precision)
                        %collision! yay fin.
                        pointCollision = projectionOrtho;
                        fin = true;
                        Dev = true;
                    else
                        %raffiner resultats 
                        deltat = deltat/2;
                        iter = iter-1;
                    end
                    
                elseif(distAuCentre >= rboite)
                    %collision de côté
                    if(distSurfaceACentre <= rboite + precision && distSurfaceACentre >= rboite - precision)
                        %collision! yay fin
                        pointCollision = (qPlan2ac - projectionOrtho)/distAuCentre * distProjASurface + projectionOrtho;
                        Dev = true;
                        fin = true;
                    elseif(distSurfaceACentre <rboite)
                        %raffiner resultats
                        deltat = deltat/2;
                        iter = iter-1;
                    end
                    %sinon pas collision
                end
            elseif(distPlan1 < 0 && distPlan2 < 0)
                %si deux negatif : check distance rayon cylindre
                axeBoite = qPlan2ac - qPlan1ac;
                axeBoite = axeBoite / norm(axeBoite);
                vectAProjeter = posBalle - qPlan1ac;
                projSurAxe = dot(axeBoite, vectAProjeter)*axeBoite + qPlan1ac;
                
                distAxe = norm(projSurAxe - posBalle);
                if(distAxe <= rboite+rballe+precision && distAxe >= rboite+rballe-precision)
                    %collision! yay fin
                    pointCollision = (projSurAxe - posBalle) / distAxe * rballe + posBalle;
                    Dev = true;
                    fin = true;
                elseif(distAxe < rboite+rballe)
                    %collision, raffiner resultats
                    deltat = deltat/2;
                    iter = iter-1;
                end
            end
        end
        
    elseif(posBalle(3) <= rballe + precision && posBalle(3) >= rballe - precision)
        %Collision avec sol
        fin = true;
    elseif(posBalle(3) < rballe)
        %raffiner pour collision avec sol
        deltat = deltat/2;
        iter = iter-1;
    elseif(ballelancee && dist > rayonMax * 2)
        %ni collision boite ni sol
        deltat = dtbase;
    end
    
end

%calculer collisions après si collision
vbaf= [qballe(iter,2:4) 0 0 0];
vbof= [qboite(iter,2:4) wboitei.'];
if(Dev)
    %Calcul des collisions voir chapitre 5 - 60
    i = 2 * mballe / 5 * rballe^2;
    IBalle = [ i 0 0; 0 i 0; 0 0 i];
    iz = mboite / 2 * rboite^2;
    i = mboite / 4 * rboite^2 + mboite/12*hboite^2;
    IBoite = [i 0 0; 0 i 0; 0 0 iz];
    rbaColl = pointCollision - qballe(iter,5:7);
    rboColl = pointCollision - qboite(iter,5:7);
    normale = rbaColl / norm(rbaColl);
    Gba = dot(normale, cross(cross(rbaColl, normale) / IBalle,rbaColl));
    Gbo = dot(normale, cross(cross(rboColl, normale) / IBoite,rboColl));
    
    vba = qballe(iter, 2:4);
    vbo = qboite(iter, 2:4) + cross(wboitei,rboColl);
    vrmoins = dot(normale, vba-vbo);
    
    impul = -1.5 * vrmoins / ( 1 / mballe + 1 / mboite + Gba + Gbo);
    wbaf = vbaf(1,4:6)+ impul * cross(rbaColl, normale) / IBalle;
    wbof = vbof(1,4:6)- impul * cross(rboColl, normale) / IBoite;
    vbaf(2,:)= [vbaf(1,1:3)+normale*impul/mballe wbaf];
    vbof(2,:) = [vbof(1,1:3)-normale*impul/mboite wbof];
    
else
    vbaf = [vbaf;vbaf];
    vbof = [vbof;vbof];
end

rba = qballe(1:iter,5:7);
rbo = qboite(1:iter,5:7);

tc = qballe(1:iter, 1);

[x,y,z] = sphere;
figure()
plot3(qboite(1:iter,5),qboite(1:iter,6),qboite(1:iter,7),'Marker','*');
hold on
plot3(qshit(1:iter,1),qshit(1:iter,2),qshit(1:iter,3),'Marker','*');
plot3(qshit2(1:iter,1),qshit2(1:iter,2),qshit2(1:iter,3),'Marker','*');
plot3(qballe(1:iter,5),qballe(1:iter,6),qballe(1:iter,7),'Marker','+');
plot3(pointCollision(1),pointCollision(2),pointCollision(3),'Marker','+');
surf(x*rballe+qballe(iter,5),y*rballe+qballe(iter,6),z*rballe+qballe(iter,7))
axis([pointCollision(1)-0.5 pointCollision(1)+0.5 pointCollision(2)-0.5 pointCollision(2)+0.5 pointCollision(3)-0.5 pointCollision(3)+0.5]);
xlabel('X');
ylabel('Y');
zlabel('N');
title('Trajectoires avec V1');
grid on
grid minor
axis square
hold off


end

