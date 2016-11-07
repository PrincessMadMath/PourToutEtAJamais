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
title('Histogramme �galis�');

% 2)
gaussien = [1 2 1 2 1;2 4 8 4 2;1 8 18 8 1;2 4 8 4 2;1 2 1 2 1] * 1/90;
imgFiltreGaussien = Filtre_Gaussien(imgEgalized, gaussien);

figure;
imshow(imgFiltreGaussien);
title('Filtr� Gaussien');

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
title('contour Rehauss�');

% 5) Avec le Laplassion, on d�tecte les contours de l'image et avec le
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

% 4)
valeur = CompterMonnaie(imageFerme);


%% Partie 2 - Filtrage spectrale

%% Exercice 3

% 1)

barresVerticales = imread('Barres_Verticales.png');
barresObliques = imread('Barres_Obliques.png');
barresHorizontales = imread('Barres_Horizontales.png');

figure;
imshow(barresVerticales);
title('Barres Verticales');

figure;
imshow(barresObliques);
title('Barres Obliques');

figure;
imshow(barresHorizontales);
title('Barres Horizontales');

% 2) Calcul tfd

TFDShift(barresVerticales, 'TFD Barres Verticales');

TFDShift(barresObliques, 'TFD Barres Obliques');

TFDShift(barresHorizontales, 'TFD Barres Horizontales');

% 3) 

barresVerticalesRotated = imrotate(barresVerticales, 70, 'bilinear','crop');

figure;
imshow(barresVerticalesRotated);
title('Barres Verticales tourne');

TFDShift(barresVerticalesRotated, 'TFD Barres Verticales tourne');

% 4) On peut observer une rotation identique dans le plan des fr�quences.
% On remarque aussi un effet de croit, comme observ� dans la TDF de l'image
% des barres Obliques.

%% Exercice 4

% 1)
houseImage = imread('NewHouse.png');
houseImageGray = rgb2gray(houseImage);

figure;
imshow(houseImageGray);
title('House Image Gray Scale');

img = TFDShift(houseImageGray, 'House Image Gray Scale TFD Shift');

% 2)

% Les lignes Horizontales de la porte sont facile � d�terminer comme les
% lignes verticales de la FFT. (centrale et les autres r�curentes)

% De m�me, les lignes Verticaux des murs sont facile � d�terminer comme
% les lignes horizontales de la FFT.

% Les formes de carr� se repr�sente sur la FFT comme des �toiles ( + ).
% (fen�tre carr�)
% ceux en angles sont donc des carr� avec une rotation (fen�tre en losange)

front = imread('Frontieres.png');
front = rgb2gray(front);

figure;
imshow(front);
title('Fronti�res');

% 3) 
TFDShifted = fftshift(fft2(houseImageGray));

H = fspecial('gaussian', size(TFDShifted), 13);
fftFiltered = TFDShifted .* H / max(max(H));

figure;
imshow(log(1 + abs(fftFiltered)), []);
title('House Image Gaussien');

imgfiltre = uint8(ifft2(ifftshift(fftFiltered)));
figure;
imshow(imgfiltre);
title('House Image Gaussien');

% 4)
%minDelta = min(min(img));
%maxDelta = max(max(img));
%img = uint8(((img - minDelta)/(maxDelta - minDelta))*255);
%imwrite(img, 'original.png');

imageFilter = imread('fenetreFiltre.png');
imageFilter = rgb2gray(imageFilter);

fftFiltered = TFDShifted .* double(imageFilter / max(max(imageFilter)));

imgfiltre = uint8(ifft2(ifftshift(fftFiltered)));

figure;
imshow(log(1 + abs(fftFiltered)), []);
title('Filtre pour fenetre');

figure;
imshow(imgfiltre);
title('Filtre avec fenetre');



% 5) Texture de la porte

imageFilter = imread('porteFiltre.png');
imageFilter = rgb2gray(imageFilter);

fftFiltered = TFDShifted .* double(imageFilter / max(max(imageFilter)));

imgfiltre = uint8(ifft2(ifftshift(fftFiltered)));

figure;
imshow(log(1 + abs(fftFiltered)), []);
title('Filtre pour porte');

figure;
imshow(imgfiltre);
title('Filtre avec porte');

% 6) Si on ne conserve pas le componsant moyen, on perd completement la
% couleur de fond (ici blanc) et tout deviens noir si aucune texture n'est
% observ�.

% 7) Dans un filtre Butterworth, nous avons des coupures beaucoup moins
% pr�cise et la p�riode de transition n'est pas id�ale (certaines
% fr�quences augment�).

% 8) Un filtre passe haut. Lorsque la fr�quence de coupure est haute, que
% la porte ou la chemin� sont visibles. Plus celle-ci diminue vers les
% basses fr�quences, plus on se met � voir des �l�ments de plus en plus bas
% en fr�quences.