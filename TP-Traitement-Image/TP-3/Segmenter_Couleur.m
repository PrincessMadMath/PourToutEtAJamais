function [ output_args ] = Segmenter_Couleur( image_couleur, LUT_r, LUT_g, LUT_b )

output_args(:,:,1) = intlut(image_couleur(:,:,1), LUT_r);
output_args(:,:,2) = intlut(image_couleur(:,:,2), LUT_g);
output_args(:,:,3) = intlut(image_couleur(:,:,3), LUT_b);

end

