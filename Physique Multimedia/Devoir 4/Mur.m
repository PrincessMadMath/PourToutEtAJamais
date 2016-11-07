classdef Mur
    %Mur Représente un mur du bloc solide ou transparent
    
    properties
        Position
        Normale
        LargeurX
        LargeurY
        LargeurZ
        EstTransparent
        Couleur
    end
    
    methods
        function mur = Mur(pos, normale, largeurX, largeurY,largeurZ,estTransparent, couleur)
            if  nargin > 0
                mur.Position = pos;
                mur.Normale = normale;
                mur.LargeurX = largeurX;
                mur.LargeurY = largeurY;
                mur.LargeurZ = largeurZ;
                mur.EstTransparent = estTransparent;
                mur.Couleur = couleur;
            end
        end
        
        function present = PtDansMur(Mur,pos)
            distX =abs( Mur.Position(1) - pos(1));
            distY =abs( Mur.Position(2) - pos(2));
            distZ =abs( Mur.Position(3) - pos(3));
            present = (dot((pos-Mur.Position), Mur.Normale) == 0 &&...
                    distX <= Mur.LargeurX/2 && ...
                    distY <= Mur.LargeurY/2 && ...
                    distZ <= Mur.LargeurZ/2);
        end
        
        function mesh = GetMeshGrid(Mur, n)
            if(Mur.Normale(1) ~= 0)
                [y,z] = meshgrid(Mur.Position(2)-Mur.LargeurY/2+n/2:n:Mur.Position(2)+Mur.LargeurY/2-n/2,...
                    Mur.Position(3)-Mur.LargeurZ/2+n/2:n:Mur.Position(3)+Mur.LargeurZ/2-n/2);
                mesh = zeros(size(y,1)*size(y,2),3);
                for i=1:size(y,1)
                    for j = 1:size(y,2)
                        mesh((i-1)*size(y,2)+j,:) = [Mur.Position(1) y(i,j) z(i,j)];
                    end
                end
            elseif(Mur.Normale(2) ~= 0)
                [x,z] = meshgrid(Mur.Position(1)-Mur.LargeurX/2+n/2:n:Mur.Position(1)+Mur.LargeurX/2-n/2,...
                    Mur.Position(3)-Mur.LargeurZ/2+n/2:n:Mur.Position(3)+Mur.LargeurZ/2-n/2);
                mesh = zeros(size(x,1)*size(x,2),3);
                for i=1:size(x,1)
                    for j = 1:size(x,2)
                        mesh((i-1)*size(x,2)+j,:) = [x(i,j) Mur.Position(2) z(i,j)];
                    end
                end
            else
                [x,y] = meshgrid(Mur.Position(1)-Mur.LargeurX/2+n/2:n:Mur.Position(1)+Mur.LargeurX/2-n/2,...
                    Mur.Position(2)-Mur.LargeurY/2+n/2:n:Mur.Position(2)+Mur.LargeurY/2-n/2);
                mesh = zeros(size(x,1)*size(x,2),3);
                for i=1:size(x,1)
                    for j = 1:size(x,2)
                        mesh((i-1)*size(x,2)+j,:) = [ x(i,j) y(i,j) Mur.Position(3)];
                    end
                end
                    
            end
        end
    end
    
end

