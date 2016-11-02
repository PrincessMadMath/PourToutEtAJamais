%% Premier
clear;
clc;
[ prise11 vf11 x11 y11 z11 t11 ] = Devoir2(1,[-120;0;4.55] / 3.6);
[ prise12 vf12 x12 y12 z12 t12 ] = Devoir2(2,[-120;0;4.55] / 3.6);
[ prise13 vf13 x13 y13 z13 t13 ] = Devoir2(3,[-120;0;4.55] / 3.6);

figure(01)
plot3(x11,y11,z11,'Marker','*');
hold on
plot3(x12,y12,z12,'Marker','+');
plot3(x13,y13,z13,'Marker','o');
axis([-5 20 -1 1 0 3]);
xlabel('X');
ylabel('Y');
zlabel('N');
title('Trajectoires avec V1');
view([-63 29]);
grid on
grid minor
hold off

%Affichage 11
disp('Tir 1');
disp('Option 1');
fprintf('Prise: %d, Temps d''arret: %d \n',prise11,t11(end));
disp('Vitesse');
disp(vf11.');
disp('Position');
disp([x11(end) y11(end) z11(end)]);
disp('');

%Affichage 12
disp('Option 2');
fprintf('Prise: %d, Temps d''arret: %d \n',prise12,t12(end));
disp('Vitesse');
disp(vf12);
disp('Position');
disp([x12(end) y12(end) z12(end)]);
disp('');

%Affichage 13
disp('Option 3');
fprintf('Prise: %d, Temps d''arret: %d \n',prise13,t13(end));
disp('Vitesse');
disp(vf13);
disp('Position');
disp([x13(end) y13(end) z13(end)]);

%-------------------------------------------%
%% Tir 2%

[ prise21 vf21 x21 y21 z21 t21 ] = Devoir2(1,[-120;0;7.79] / 3.6);
[ prise22 vf22 x22 y22 z22 t22 ] = Devoir2(2,[-120;0;7.79] / 3.6);
[ prise23 vf23 x23 y23 z23 t23 ] = Devoir2(3,[-120;0;7.79] / 3.6);

figure(02)
plot3(x21,y21,z21,'Marker','*');
hold on
plot3(x22,y22,z22,'Marker','+');
plot3(x23,y23,z23,'Marker','o');
axis([-15 20 -1 1 0 3]);
xlabel('X');
ylabel('Y');
zlabel('N');
title('Trajectoires avec V2');
view([-63 29]);
grid on
grid minor
hold off

%Affichage 21
disp('-----------------------\n-----------------------');
disp('Tir 2');
disp('Option 1');
fprintf('Prise: %d, Temps d''arret: %d \n',prise21,t21(end));
disp('Vitesse');
disp(vf21.');
disp('Position');
disp([x21(end) y21(end) z21(end)]);
disp('');

%Affichage 22
disp('Option 2');
fprintf('Prise: %d, Temps d''arret: %d \n',prise22,t22(end));
disp('Vitesse');
disp(vf22);
disp('Position');
disp([x22(end) y22(end) z22(end)]);
disp('');

%Affichage 23
disp('Option 3');
fprintf('Prise: %d, Temps d''arret: %d \n',prise23,t23(end));
disp('Vitesse');
disp(vf13);
disp('Position');
disp([x23(end) y23(end) z23(end)]);

%-------------------------------------------%
%% Tir 2%

[ prise21 vf21 x21 y21 z21 t21 ] = Devoir2(1,[-120;0;7.79] / 3.6);
[ prise22 vf22 x22 y22 z22 t22 ] = Devoir2(2,[-120;0;7.79] / 3.6);
[ prise23 vf23 x23 y23 z23 t23 ] = Devoir2(3,[-120;0;7.79] / 3.6);

figure(02)
plot3(x21,y21,z21,'Marker','*');
hold on
plot3(x22,y22,z22,'Marker','+');
plot3(x23,y23,z23,'Marker','o');
axis([-15 20 -1 1 0 3]);
xlabel('X');
ylabel('Y');
zlabel('N');
title('Trajectoires avec V2');
view([-63 29]);
grid on
grid minor
hold off

%Affichage 21
disp('----------------------------------------------');
disp('Tir 2');
disp('Option 1');
fprintf('Prise: %d, Temps d''arret: %d \n',prise21,t21(end));
disp('Vitesse');
disp(vf21.');
disp('Position');
disp([x21(end) y21(end) z21(end)]);
disp('');

%Affichage 22
disp('Option 2');
fprintf('Prise: %d, Temps d''arret: %d \n',prise22,t22(end));
disp('Vitesse');
disp(vf22);
disp('Position');
disp([x22(end) y22(end) z22(end)]);
disp('');

%Affichage 23
disp('Option 3');
fprintf('Prise: %d, Temps d''arret: %d \n',prise23,t23(end));
disp('Vitesse');
disp(vf23);
disp('Position');
disp([x23(end) y23(end) z23(end)]);

%% Tir 3

[ prise31 vf31 x31 y31 z31 t31 ] = Devoir2(1,[-120;1.8;5.63] / 3.6);
[ prise32 vf32 x32 y32 z32 t32 ] = Devoir2(2,[-120;1.8;5.63] / 3.6);
[ prise33 vf33 x33 y33 z33 t33 ] = Devoir2(3,[-120;1.8;5.63] / 3.6);

figure(03)
plot3(x31,y31,z31,'Marker','*');
hold on
plot3(x32,y32,z32,'Marker','+');
plot3(x33,y33,z33,'Marker','o');
axis([-15 20 -1 1 0 3]);
xlabel('X');
ylabel('Y');
zlabel('N');
title('Trajectoires avec V3');
view([-63 29]);
grid on
grid minor
hold off

%Affichage 21
disp('---------------------------------------------');
disp('Tir 3');
disp('Option 1');
fprintf('Prise: %d, Temps d''arret: %d \n',prise31,t31(end));
disp('Vitesse');
disp(vf31.');
disp('Position');
disp([x31(end) y31(end) z31(end)]);
disp('');

%Affichage 22
disp('Option 2');
fprintf('Prise: %d, Temps d''arret: %d \n',prise32,t32(end));
disp('Vitesse');
disp(vf32);
disp('Position');
disp([x32(end) y32(end) z32(end)]);
disp('');

%Affichage 23
disp('Option 3');
fprintf('Prise: %d, Temps d''arret: %d \n',prise33,t33(end));
disp('Vitesse');
disp(vf33);
disp('Position');
disp([x33(end) y33(end) z33(end)]);