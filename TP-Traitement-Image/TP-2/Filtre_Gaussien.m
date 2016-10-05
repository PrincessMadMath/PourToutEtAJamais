function result = Filtre_Gaussien (img, masque)

result = conv2(im2double(img), im2double(masque),'same');
end