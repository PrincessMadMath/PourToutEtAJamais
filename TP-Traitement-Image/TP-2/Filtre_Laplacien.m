function result = Filtre_Laplacien(img)

laplacien = [-1 -1 -1;-1 8 -1;-1 -1 -1];
result = conv2(im2double(img), im2double(laplacien),'same');
end