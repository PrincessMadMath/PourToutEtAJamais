type Filtre_Canny.m;
type Calculer_Precision.m;

%% Exercice 1

% 1) Charger l'image
imgageVoiture = imread('Formula_Ford.png');

figure;
imshow(imgageVoiture);
title('Image Voiture');

% 2) Construction PSF
PSF = fspecial('motion', 30, 25);

% 3) Deconvolution
k = 0;
imageUnblured = deconvwnr(imgageVoiture, PSF, k);

figure;
imshow(imageUnblured);
title('Unblured Voiture');

% 4) Meilleure estimation
I = im2double(imgageVoiture);
noise_var = 0.0001;
k = noise_var / var(I(:));

% 5) Deconvoluer image
imageUnblured = deconvwnr(imgageVoiture, PSF, k);

figure;
imshow(imageUnblured);
title('Unblured Voiture (better k)');

% Nous pouvons transmettre que la plaque d'imatriculation est : FF12 FMC

%% Exercice 2

% 1) Charger image
imageEscalier = imread('escaliers.jpg');

figure;
imshow(imageEscalier);
title('Image Escalier');

% 2) Filtre Canny
gaussien = fspecial('gaussian', 3, 0.5);

img_bin = Filtre_Canny(imageEscalier, gaussien, 20);

figure;
imshow(img_bin);
title('Image Binarise final');

% 3) Fonction Calculer_Precision

% 4) Charger image escalier
escalier_truSeg = imread('escaliers_TrueSeg.jpg') > 128;

% On remarque que plus on diminue le seuil, plus on observe du bruit
% ambiant (dans les planche du plancher)

% Seuil de 70 => perfo = 0.2696
% SEuil de 20 => perfo = 0.1994

figure;
imshow(escalier_truSeg);
title('True Segmantation');
[ perfo, tpf, tfn ] = Calculer_Precision(img_bin,escalier_truSeg);

