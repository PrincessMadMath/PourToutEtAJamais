function img = Binariser(img, seuil)

for n = 1:size(img,1)
for m = 1:size(img,2)

    if(img(n,m) < seuil)
        img(n,m) = 0;
    else
        img(n,m) = 255;
    end
end
end

end