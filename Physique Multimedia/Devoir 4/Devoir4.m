function Devoir4( posObs, n0, nt )
%UNTITLED4 Summary of this function goes here
%   Detailed explanation goes here
%% Declaration de nos variables
%On travaille ici en cm.
murs(12,1) = Mur;

%Murs du bloc transparent
murs(1) = Mur([0 3.5 12.5],[-1 0 0],0,7,15,true,0);
murs(2) = Mur([7 3.5 12.5],[1 0 0],0,7,15,true,0);
murs(3) = Mur([3.5 0 12.5],[0 -1 0],7,0,15,true,0);
murs(4) = Mur([3.5 7 12.5],[0 1 0],7,0,15,true,0);
murs(5) = Mur([3.5 3.5 5],[0 0 -1],7,7,0,true,0);
murs(6) = Mur([3.5 3.5 20],[0 0 1],7,7,0,true,0);

%Murs du bloc opaque
murs(7) = Mur([3 4 14.5],[-1 0 0],0,2,5,false,[1 0 0]);
murs(8) = Mur([4 4 14.5],[1 0 0],0,2,5,false,[0 1 1]);
murs(9) = Mur([3.5 3 14.5],[0 -1 0],1,0,5,false,[0 1 0]);
murs(10) = Mur([3.5 5 14.5],[0 1 0],1,0,5,false,[1 1 0]);
murs(11) = Mur([3.5 4 12],[0 0 -1],1,2,0,false,[0 0 1]);
murs(12) = Mur([3.5 4 17],[0 0 1],1,2,0,false,[1 0 1]);

rayons = [];
rayoni = 1;
%% Creation des rayons et premiere collision
for i = 1:6
    obsAFace = (murs(i).Position - posObs);
    if(dot(obsAFace,murs(i).Normale) < 0)
        curmesh = murs(i).GetMeshGrid(0.1);
        clear ra;
        ra(size(curmesh,1),1) = Rayon;
        rayons = [rayons; ra];
        for j=1:size(curmesh,1)
            rayons(rayoni) = Rayon(posObs,curmesh(j,:));
            rayons(rayoni) = CollisionRayon(rayons(rayoni), murs(i).Normale, curmesh(j,:), n0, nt);
            rayoni = rayoni + 1;
        end
    end
end

%% Simulations des rayons
nbCollisions = 0;
for i=1:size(rayons)
    while ~rayons(i).ACollision && ~rayons(i).EstRejete
        ptCol=[0 0 0];
        iCol=0;
        distCol=99999;
        for j =1:size(murs)
            [tempPtCol, tempDistCol] = DistanceDroitePlan(rayons(i).DirectionCourante, rayons(i).PointCourant,murs(j).Normale, murs(j).Position);
            if tempDistCol > 0 && tempDistCol < distCol && murs(j).PtDansMur(tempPtCol) && (j<=6 || dot(rayons(i).DirectionCourante, murs(j).Normale)<0)
                distCol = tempDistCol;
                ptCol = tempPtCol;
                iCol = j;
            end
        end
        if iCol == 0 %pas de collision
            rayons(i).EstRejete = true;
        elseif iCol > 6 %collision avec le bloc opaque
            rayons(i).Longueur = rayons(i).Longueur + distCol;
            rayons(i).ACollision = true;
            rayons(i).Couleur = murs(iCol).Couleur;
            nbCollisions = nbCollisions + 1;
        else
           rayons(i) =  CollisionRayon(rayons(i),-murs(iCol).Normale,ptCol,nt,n0);
        end
    end
end
%% Affichage des points finaux
positionsVirtuelles = zeros(nbCollisions,3);
couleurs = zeros(nbCollisions,3);
iter = 1;
for i = 1:size(rayons)
    if rayons(i).ACollision
        positionsVirtuelles(iter,:) = rayons(i).Longueur .* rayons(i).DirectionOriginale + posObs;
        couleurs(iter,:) = rayons(i).Couleur;
        iter = iter+1;
    end
end
              
vertices =   [0 0 5
              0 7 5
              0 7 20
              7 7 20
              7 0 20
              0 0 20
              7 0 5
              7 7 5];
          
faces = [ 1 2 8 7
          1 2 3 6
          1 7 5 6
          4 5 6 3
          4 5 7 8
          4 8 2 3];
s = ones( numel(couleurs(:,1)),1)*20;
figure
scatter3(positionsVirtuelles(:,1),positionsVirtuelles(:,2),positionsVirtuelles(:,3),s,couleurs,'filled');
hold on
scatter3(posObs(1),posObs(2),posObs(3),80,[0 0 0]);
patch('Vertices',vertices,'Faces',faces,'FaceColor','none','LineWidth',1)
hold off
%graph 

