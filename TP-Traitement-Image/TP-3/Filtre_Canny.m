function [ image ] = Filtre_Canny( image, masque, seuil )

tb = 0.4;

% lissage gaussien (reduction du bruit)
image_filtre = imfilter(image, masque);

% calcul du gradient (intensite des contours)
[gmag, gdir] = imgradient(image_filtre);

% recherche max locaux
maxN = size(image,1);
maxM = size(image,2);

for n = 1:maxN
for m = 1:maxM

    pixel1 = 0;
    pixel2 = 0;
    pixel = gmag(n,m);
    
    dir = abs(gdir(n,m));
    
    % horizontal
    if(dir > 67.5 && dir <= 112.5)
        if(n > 1)
            pixel1 = gmag(n-1, m);
        end
        if(n < maxN)
            pixel2 = gmag(n+1, m);
        end
       
    end
    
    % vertical
    if(dir <= 22.5 || dir > 157.5)        
        
         if(m > 1)
            pixel1 = gmag(n, m-1);
        end
        if(m < maxM)
            pixel2 = gmag(n, m+1);
        end
    end
    
    % diago /
    if(dir > 112.5 && dir <= 157.5)        
        if(n > 1 && m > 1)
            pixel1 = gmag(n-1, m-1);
        end
        if(n < maxN && m < maxM)
            pixel2 = gmag(n+1, m+1);
        end
    end
    
    % diago \
    if(dir > 22.5 && dir <= 67.5)
        if(n > 1 && m < maxM)
            pixel1 = gmag(n-1, m+1);
        end
        if(n < maxN && m > 1)
            pixel2 = gmag(n+1, m-1);
        end
    end
    
    if(pixel > pixel1 && pixel > pixel2)
        image(n,m) = pixel;
    else
        image(n,m) = 0;
    end
end
end

maximum_local = image;
image = image > seuil;

figure;
imshow(image);
title('Image Binarise inter');

for n = 1:maxN
for m = 1:maxM
    pixel = image(n,m);
    
    if(pixel == 1)
        % check horizontale
        if(m > 1 && m < maxM)
            % pixel de droite
            if(image(n, m-1) == 1 && maximum_local(n, m+1) > tb)
                image(n,m+1) = 1;
            end
            
            %pixel de gauche
            if(image(n, m+1) == 1 && maximum_local(n, m-1) > tb)
                image(n,m-1) = 1;
            end
        end
        
        % check verticale
        if(n > 1 && n < maxN)
            % pixel de droite
            if(image(n-1, m) == 1 && maximum_local(n+1, m) > tb)
                image(n+1,m) = 1;
            end
            
            %pixel de gauche
            if(image(n+1, m) == 1 && maximum_local(n-1, m) > tb)
                image(n-1,m) = 1;
            end
        end
    end
end
end

end