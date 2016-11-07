function [TFDLog] = TFDShift( image, titleString )

TFD = abs(fft2(image));
absTFD = abs(TFD);

TFDCentered = fftshift(absTFD);
TFDLog = log( 1 + TFDCentered);

figure;
imshow(TFDLog, []);
title(titleString);

end

