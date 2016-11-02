clc;
%Tir 1
%Sans rotation de boite
[ Dev, vbaf, vbof, rba, rbo, tc ] = Devoir3([0;0;0], [6.85;0;6.85], 0.66);
disp('Tir 1');
disp('Sans rotation de la boite');
disp('Collision :');
disp(Dev);
disp('vbaf:');
disp(vbaf);
disp('vbof:');
disp(vbof);
disp('pos balle fin:');
disp(rba(end,:));
disp('pos boite fin:');
disp(rbo(end,:));
disp('Temps d''arret');
disp(tc(end));
%Sans rotation de boite
[ Dev, vbaf, vbof, rba, rbo, tc ] = Devoir3([0;2.3;0], [6.85;0;6.85], 0.66);
disp('Avec rotation de la boite');
disp('Collision :');
disp(Dev);
disp('vbaf:');
disp(vbaf);
disp('vbof:');
disp(vbof);
disp('pos balle fin:');
disp(rba(end,:));
disp('pos boite fin:');
disp(rbo(end,:))
disp('Temps d''arret');
disp(tc(end));
disp('------------------------------');

%Tir 2
%Sans rotation de boite
[ Dev, vbaf, vbof, rba, rbo, tc ] = Devoir3([0;0;0], [28;0.5;10], 1.1);
disp('Tir 2');
disp('Sans rotation de la boite');
disp('Collision :');
disp(Dev);
disp('vbaf:');
disp(vbaf);
disp('vbof:');
disp(vbof);
disp('pos balle fin:');
disp(rba(end,:));
disp('pos boite fin:');
disp(rbo(end,:))
disp('Temps d''arret');
disp(tc(end));
%Sans rotation de boite
[ Dev, vbaf, vbof, rba, rbo, tc ] = Devoir3([0;2.3;0], [28;0.5;10], 1.1);
disp('Avec rotation de la boite');
disp('Collision :');
disp(Dev);
disp('vbaf:');
disp(vbaf);
disp('vbof:');
disp(vbof);
disp('pos balle fin:');
disp(rba(end,:));
disp('pos boite fin:');
disp(rbo(end,:))
disp('Temps d''arret');
disp(tc(end));
