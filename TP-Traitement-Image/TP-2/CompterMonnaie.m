function valeur = CompterMonnaie(img)

L = bwlabel(img);
valeur = max(max(L));
end