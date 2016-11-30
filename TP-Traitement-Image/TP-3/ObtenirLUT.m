function [ table_conversion ] = ObtenirLUT( nb_segments )

%% Discretisation du cube RGB

segment_size = ceil(256 / nb_segments);
table_conversion = uint8(zeros(256,1));

for i = 0:255
    index = floor(i / segment_size);
    low = index * segment_size;
    high = min((index+1) * segment_size - 1, 255);
    table_conversion(i+1) = uint8(floor(( low + high ) / 2));
end

end

