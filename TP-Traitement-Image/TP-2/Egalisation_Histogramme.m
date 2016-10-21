function img = Egalisation_Histogramme(img)

imgEqual = imhist(img);

% Cummulative sum
for n = 2:size(imgEqual,1)
    imgEqual(n,1) = imgEqual(n,1)+ imgEqual(n-1,1);
end

% total pixels
totalPixels = imgEqual(size(imgEqual,1),1);

% histo normalized and scaled
for n = 1:size(imgEqual,1)
    imgEqual(n,1) = (imgEqual(n,1) / totalPixels) * 255;
end

% iterate on img
for n = 1:size(img,1)
for m = 1:size(img,2)
    img(n,m) = imgEqual(img(n,m),1);
end
end

end