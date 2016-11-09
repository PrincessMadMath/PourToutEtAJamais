function [ perfo, faux_pos, faux_neg ] = Calculer_Precision( image_seg, image_ref )

contour_seg = sum(image_seg);
contour_ref = sum(image_ref);

intersec = sum(image_seg & image_ref);
end