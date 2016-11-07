%% PHS4700 : Physique Multimédia - Devoir 1
% Créé : 17 sept. 2015 
%
% Auteurs : 
%
% * Alex Gagné (1689761)
% * Félix La Rocque Cartier (1621348)
% * Mathieux Gamache (1626377)
% * Konstantin Fedorov (1679095)
%
% Ce travail à pour but de nous familiariser avec les matrices de rotation,
% du calcul des centres de masses, moments d'inerties et accélérations
% angulaires dans une application multimédia

%% Déclaration des variables

% Longueurs
longJamb  = 0.75;
longTronc = 0.70;
longCou   = 0.10;
longBras  = 0.75;

% Rayons
rayonTete  = 0.10;
rayonJamb  = 0.06;
rayonCou   = 0.04;
rayonBras  = 0.03;
rayonTronc = 0.15;

% Densités
densiteTete  = 1056;
densiteJamb  = 1052;
densiteTronc = 953;
densiteCou   = 953;
densiteBras  = 1052;

% Autres
pointForce      = [0;0;longJamb+longTronc+longCou+rayonTete];
vitessePatineur = [0;0;10];

%% Creation de nos parties du corps et calcul de la masse
% Classes utilisées : <Sphere.html Sphere>, <Cylinder.html Cylinder> qui
% héritent de <Shape.html Shape>
%
% Fonction utilisées : <moveMoment.html moveMoment> 
rightLeg = Cylinder( [-0.10;0;longJamb/2], densiteJamb, rayonJamb, longJamb, 3 );
leftLeg  = Cylinder( [0.10;0;longJamb/2], densiteJamb, rayonJamb, longJamb, 3 );
tronc    = Cylinder( [0;0;longJamb + longTronc/2], densiteTronc, rayonTronc, longTronc, 3 );
leftArm  = Cylinder( [-rayonTronc-rayonBras;0;longJamb+longTronc-longBras/2], densiteBras, rayonBras, longBras, 3 );
rightArm = Cylinder( [rayonTronc+rayonBras;0;longJamb+longTronc-longBras/2], densiteBras, rayonBras, longBras, 3 );
leftArm2 = Cylinder( [-rayonTronc-longBras/2;0;longJamb+longTronc-rayonBras], densiteBras, rayonBras, longBras, 1 );
neck     = Cylinder( [0;0;longJamb + longTronc + longCou/2], densiteCou, rayonCou, longCou, 3 );
head     =   Sphere( [0;0;longJamb+longTronc+longCou + rayonTete], densiteTete, rayonTete );

%La masse des parties du corps individuelles sont calculées à leur création
totalMass = rightLeg.weight ...
    + leftLeg.weight ...
    + tronc.weight ...
    + rightArm.weight ...
    + leftArm.weight ...
    + neck.weight ...
    + head.weight;

%% Cas 1 : patineur droit, bras vertical
centerOfMass = (rightLeg.centerOfMass * rightLeg.weight ...
    + leftLeg.centerOfMass * leftLeg.weight ...
    + tronc.centerOfMass * tronc.weight ...
    + rightArm.centerOfMass * rightArm.weight ...
    + leftArm.centerOfMass * leftArm.weight ...
    + neck.centerOfMass * neck.weight ...
    + head.centerOfMass * head.weight)...
    / totalMass;


momentInertie = moveMoment(centerOfMass,leftLeg) ...
    + moveMoment(centerOfMass,rightLeg) ...
    + moveMoment(centerOfMass,tronc) ...
    + moveMoment(centerOfMass,rightArm) ...
    + moveMoment(centerOfMass,leftArm) ...
    + moveMoment(centerOfMass,neck) ...
    + moveMoment(centerOfMass,head);

momentForce     = cross(pointForce - centerOfMass, [0;-200;0]);
accAngulaire    = momentInertie\momentForce; % equivalent a inv(momentGlobal) * momentForce
momentCinetique = momentInertie*vitessePatineur;
accAngulaire2   = momentInertie\(momentForce + cross(momentCinetique,vitessePatineur));

%% Cas 2 Patineur droit, bras étendu
% Les calculs sont les mêmes que pour le premier cas, à la différence que
% le bras gauche est maintenant étendu
centerOfMass_2 = (rightLeg.centerOfMass * rightLeg.weight ...
    + leftLeg.centerOfMass * leftLeg.weight ...
    + tronc.centerOfMass * tronc.weight ...
    + rightArm.centerOfMass * rightArm.weight ...
    + leftArm2.centerOfMass * leftArm.weight ...
    + neck.centerOfMass * neck.weight ...
    + head.centerOfMass * head.weight)...
    / totalMass;
momentInertie_2 = moveMoment(centerOfMass,leftLeg) ...
    + moveMoment(centerOfMass,rightLeg) ...
    + moveMoment(centerOfMass,tronc) ...
    + moveMoment(centerOfMass,rightArm) ...
    + moveMoment(centerOfMass,leftArm2) ...
    + moveMoment(centerOfMass,neck) ...
    + moveMoment(centerOfMass,head);


momentForce_2     = cross(pointForce - centerOfMass_2,[0;-200;0]);
accAngulaire_2    = momentInertie_2\momentForce_2;
momentCinetique_2 = momentInertie_2 * vitessePatineur;
accAngulaire2_2   = momentInertie_2\(momentForce_2 + cross(momentCinetique_2,vitessePatineur));

%% Cas 3 : Patineur incline, bras le long du corps
% Dans cette situation, il faut appliquer une rotation sur les valeurs du
% centre de masse et du moment d'inertie trouvé au cas 1.
angle = -pi / 18;

matRotation = [cos(angle),  0, sin(angle)
               0,           1, 0
               -sin(angle), 0, cos(angle)];
pointForceTourne = matRotation * pointForce;
vitessePatineurTourne = matRotation * vitessePatineur;

centerOfMass_3    = matRotation * centerOfMass;
momentInertie_3   = matRotation * momentInertie / matRotation;
momentForce_3     = cross(pointForceTourne - centerOfMass_3,[0;-200;0]);
accAngulaire_3    = momentInertie_3\momentForce_3;
momentCinetique_3 = momentInertie_3 * vitessePatineurTourne;
accAngulaire2_3   = momentInertie_3\(momentForce_3 + cross(momentCinetique_3,vitessePatineurTourne));

%% cas 4 : Patineur incline, bras etendu

centerOfMass_4    = matRotation * centerOfMass_2;
momentInertie_4   = matRotation * momentInertie_2 / matRotation;
momentForce_4     = cross(pointForceTourne - centerOfMass_4,[0;-200;0]);
accAngulaire_4    = momentInertie_4\momentForce_4;
momentCinetique_4 = momentInertie_4 * vitessePatineurTourne;
accAngulaire2_4   = momentInertie_4\(momentForce_4 + cross(momentCinetique_4,vitessePatineurTourne));


%% Affichage des resultats
disp('Cas 1 : Patineur droit, bras le long du corps');
disp('   Centre de masse:');
disp(centerOfMass);
disp('   Moment d''inertie: ')
disp(momentInertie);
disp('   Accélération angulaire sans vitesse initale:');
disp(accAngulaire);
disp('   Accélération angulaire avec vitesse initiale:');
disp(accAngulaire2);
disp('____________');

disp('Cas 2 : Patineur droit, bras étendu');
disp('   Centre de masse:');
disp(centerOfMass_2);
disp('   Moment d''inertie: ')
disp(momentInertie_2);
disp('   Accélération angulaire sans vitesse initale:');
disp(accAngulaire_2);
disp('   Accélération angulaire avec vitesse initiale:');
disp(accAngulaire2_2);
disp('____________');

disp('Cas 3 : Patineur incliné, bras le long du corps');
disp('   Centre de masse:');
disp(centerOfMass_3);
disp('   Moment d''inertie: ')
disp(momentInertie_3);
disp('   Accélération angulaire sans vitesse initale:');
disp(accAngulaire_3);
disp('   Accélération angulaire avec vitesse initiale:');
disp(accAngulaire2_3);
disp('____________');

disp('Cas 4 : Patineur incliné, bras étendu');
disp('   Centre de masse:');
disp(centerOfMass_4);
disp('   Moment d''inertie: ')
disp(momentInertie_4);
disp('   Accélération angulaire sans vitesse initale:');
disp(accAngulaire_4);
disp('   Accélération angulaire avec vitesse initiale:');
disp(accAngulaire2_4);
