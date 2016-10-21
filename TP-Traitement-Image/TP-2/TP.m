%% Partie 1 - Filtrage spatial
currentfolder = pwd;

%% E1 Rehaussement d'image

% 1)
img = imread('img1.jpg');
imgEgalized = Egalisation_Histogramme(img);
figure;
imshow(img);
title('Image Originale');

figure;
imshow(imgEgalized);
title('Histogramme Égalisé');

% 2)
gaussien = [1 2 1 2 1;2 4 8 4 2;1 8 18 8 1;2 4 8 4 2;1 2 1 2 1] * 1/90;
imgFiltreGaussien = Filtre_Gaussien(imgEgalized, gaussien);

figure;
imshow(imgFiltreGaussien);
title('Filtré Gaussien');

% 3)
delta = Filtre_Laplacien(imgFiltreGaussien);
minDelta = min(min(delta));
maxDelta = max(max(delta));

visibleDelta = uint8(((delta - minDelta)/(maxDelta - minDelta))*255);

figure;
imshow(visibleDelta);
title('Filtre Laplacien');

% 4)
K = 1.2;
gaussien = [1 2 3;2 4 2;1 2 1] * 1/16;
imgFiltreGaussien = Filtre_Gaussien(imgEgalized, gaussien);
contourRehausse = imgFiltreGaussien + K * delta;

figure;
imshow(contourRehausse);
title('contour Rehaussé');

% 5) Avec le Laplassion, on détecte les contours de l'image et avec le
% rehaussement, on accentue ces zones en augmentant les contrastes.

%% Exercice 2

% 1) 
img = imread('monnaie.png');
figure;
imshow(img);
title('Image Originale');

% 2)
seuil = 250;
binarized = Binariser(img, 250);
inverse = imcomplement(binarized);
figure;
imshow(inverse);
title('Image BInariser Complementaire');

% 3) ouverture -> fermeture

imageOuverte = imopen(inverse, strel('disk', 1));
imageFerme = imclose(imageOuverte, strel('disk', 3));

figure;
imshow(imageFerme);
title('Image BInariser Complementaire');