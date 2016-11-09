function [ perfo, tpf, tfn ] = Calculer_Precision( image_seg, image_ref )

contour_detecte = sum(sum(image_seg));
contour_reference = sum(sum(image_ref));

contour_correct = sum(sum(image_seg & image_ref));

faux_pos = contour_detecte - contour_correct;
faux_neg = contour_reference - contour_correct;

denum = (contour_correct + faux_pos + faux_neg);
perfo = contour_correct / denum;

tpf = faux_pos / denum;
tfn = faux_neg / denum;
end