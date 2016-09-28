type TFD.m;

%% Exercice 1 : Signaux Simples

% 1)
X=[0:(pi/13):(7*pi)];
Y=sin(X);
C=2.8*ones(1,length(X));

% Bruit Gaussien
a=sqrt(0.4);
b=0.1;
B=a*randn(1,length(X))+b;

Z = Y+C+B;

% Mean
M = mean(Z);
disp('Mean : ');
disp(M);

% Variance
V = var(Z);
disp('Variance : ');
disp(V);

% 2)
% Plot
figure;
plot(X,Y,X,C,X,B,X,Z);
legend('Sinus','Constant','Gaussien','Sum');
grid on;
title('Signaux Simples');

%% Exercice 2 : Echantillonnage

% 1)
% Fr�quence 1: 82,5 Hz
% Fr�quence 2: 3 Hz
% Fr�quence 3: 40 Hz

% 2)
figure;
F=[20,75,100,160,180,330];
for index = 1:length(F)
    t=[0:1/F(1,index):1];
    Y=2*sin(165*pi*t) + 13*cos(6*pi*t) - 3*cos(80*pi*t);
    subplot(length(F),1,index);
    plot(t,Y);
    title(strcat('Echantillonage : ',int2str(F(1,index))));
    ylim([-20 20]);
    xlabel('Time (sec)');
end
% 3)
% Plus la fr�quence d'�chantilonnage est �lev�, plus la fonction est bien
% approxim�. On peut voir qu'avant 2x la fr�quence maximale de la fonction
% (80 Hz) on a de grandes pertes sur le signal. Au dessus de celle-ci, le
% signal se rapproche de plue en plus du signal r�el.

% 4)
% La fr�quence de 180 et celle de 330 Hz. La fr�quence optimale est 180 car
% nous avons une meilleure optimisation de la m�moire

%% Exercice 3: Analyse spectrale

Fe = 250;
p = 1/Fe;
t = [0:p:2];

Y1 = 5*sin(2*pi*4*t);
Y2 = 3*sin(2*pi*45*t);
Y3 = 2*sin(2*pi*70*t);

% 1)
figure;
plot(t,Y1,t,Y2,t,Y3);
title('Les trois signaux sinusoidales');
legend('Y1','Y2','Y3');
xlabel('Time (sec)');
ylabel('Y');
grid minor;
grid on;

% 2)
% P�riode Y1 : 0.25 sec
% P�riode Y2 : 0.022 sec
% P�riode Y3 : 0.015 sec

% 3)
Z = Y1+Y2+Y3;
figure;
plot(t,Z);
title('La somme trois signaux sinusoidales');
xlabel('Time (sec)');
ylabel('Y');
grid minor;
grid on;

% On voit sur le graphique une fr�quence de : 0.23

% Plus grand diviseur commun des fr�quences est 1 donc on peux
% analytiquement d�termin� la p�riode comme �tant 1

% 4)
[f0,FY1] = TFD(Fe, Y1);
[f1,FY2] = TFD(Fe, Y2);
[f2,FY3] = TFD(Fe, Y3);

figure;
plot(f0,FY1, f0,FY2, f0,FY3);
title('Spectre de fr�quences');
legend('FFT Y1','FFT Y2','FFT Y3');
grid minor;
grid on;
% On peut voir que la plus haute influence est visible par la fr�quence de
% 4 Hz. On retrouve les m�mes fr�quences que ceux trouv�s dans nos signaux
% (4, 45, 70 Hz)

% 5)
[f0,FZ] = TFD(Fe, Z);
figure;
plot(f0,FZ);
title('Spectre de fr�quences du signal composite');
grid minor;
grid on;

% C'est le m�me graphique qu'au num�ro 4, ce qui �tait le r�sultat
% escompt�. Le but de la FFT est de d�composer les fr�quences dont le
% signal est compos�. Il est donc normale de voir les m�mes signaux
% resortis.

%% Exercice 4

% 1)
 
[data,Fe] = audioread('Audio.wav');

audio = audioplayer(data, Fe);

% On entand un grinchement constant (haute frequence) et un autre bruit
% metalique (basse frequence)

% 2) TFD audio original
[f0,Fdata] = TFD(Fe,data);

figure;
plot(f0,Fdata);
title('Spectre du signal audio original');
grid minor;
grid on;
xlabel('Frequence');
ylabel('Amplitude');

% 3) La perturbation la plus haute est un r�# (1244 Hz)

% 4) Filtre passa bas d'ordre 128
fc = 900;
n = 128;
b = fir1(n,fc/(Fe/2),'low');

lowpassaudio = filter(b,1,data);
[f0,Fdata] = TFD(Fe,lowpassaudio);

figure;
plot(f0,Fdata);
title('Spectre du signal audio avec filtre passe bas n=128');
grid minor;
grid on;
xlabel('Frequence');
ylabel('Amplitude');

audio = audioplayer(lowpassaudio, Fe);

% Nous avons att!nuer toutes les fr�quences plus hautes que la fr�quence de
% coupure. Pour conserver cet information, un coupe bande peut �tre
% utiliser

% 5) High Pass
fc = 250;
L = 1200;

% Chebychev
[b_cheby,a_cheby] = fir1(n,fc/(Fe/2),'high',chebwin(n+1,30));

dataout = filter(b_cheby,a_cheby,data);
[f0,Fdata] = TFD(Fe,dataout);

figure;
plot(f0,Fdata);
title('Spectre du signal audio avec filtre Chebychev passe Haut n=128');
grid minor;
grid on;
xlabel('Frequence');
ylabel('Amplitude');

% Hamming
[b_hamm,a_hamm] = fir1(n,fc/(Fe/2),'high',hamming(n+1));

dataout = filter(b_hamm,a_hamm,data);
[f0,Fdata] = TFD(Fe,dataout);

figure;
plot(f0,Fdata);
title('Spectre du signal audio avec filtre Hamming passe Haut n=128');
grid minor;
grid on;
xlabel('Frequence');
ylabel('Amplitude');

% Blackman
[b_black,a_black] = fir1(n,fc/(Fe/2),'high',blackman(n+1));

dataout = filter(b_black,a_black,data);
[f0,Fdata] = TFD(Fe,dataout);

figure;
plot(f0,Fdata);
title('Spectre du signal audio avec filtre Blackman passe Haut n=128');
grid minor;
grid on;
xlabel('Frequence');
ylabel('Amplitude');

% 6) Freqz

hd_cheby = dfilt.dffir(b_cheby);
hb_hamm = dfilt.dffir(b_hamm);
hb_black = dfilt.dffir(b_black);
hd = [hd_cheby hb_hamm hb_black];

freqz(hd);
xlim([0 0.1]);
legend('Chebychev','Hamming','Blackman');
grid minor;
grid on;

% La plus grande diff�rence entre les 3 filtres est la diff�rence
% d'att�nuation pour les fr�quences coup�s. On peut aussi voir que le
% filtre chebychev ondule contrairement aux deux autres filtres.

% 7) 

chebyFiltered = filter(b_cheby,a_cheby,lowpassaudio);
hammFiltered = filter(b_hamm,a_hamm,lowpassaudio);
blackFiltered = filter(b_black,a_black,lowpassaudio);

% audio = audioplayer(chebyFiltered, Fe);
% audio = audioplayer(lowpassaudio, Fe);
% audio = audioplayer(blackFiltered, Fe);

% Maintenant, nous n'avons plus le bruit de haute fr�quence ni celui de
% basse fr�quence. On remarque que, pour couper le bruit de haute fr�quence
% totalement, nous devons aussi diminuer une partie de la musique et ainsi
% perdre de l'information.

% 8)

[f0,FdataCheby] = TFD(Fe,chebyFiltered);
[f1,FdataHamm] = TFD(Fe,hammFiltered);
[f2,FdataBlack] = TFD(Fe,blackFiltered);

figure;
plot(f0,FdataCheby );
ylim([-500 500]);
xlim([-1500 1500]);
title('Spectre du signal audio filtr� avec Chebychev');
legend('TFD Chebychev');
grid minor;
grid on;
xlabel('Frequence');
ylabel('Amplitude');

figure;
plot(f0,FdataHamm);
ylim([-500 500]);
xlim([-1500 1500]);
title('Spectre du signal audio filtr� avec Hamming');
legend('TFD Hamming');
grid minor;
grid on;
xlabel('Frequence');
ylabel('Amplitude');

figure;
plot(f0,FdataBlack );
ylim([-500 500]);
xlim([-1500 1500]);
title('Spectre du signal audio filtr� avec Blackman');
legend('TFD Blackman');
grid minor;
grid on;
xlabel('Frequence');
ylabel('Amplitude');

% On remarque que le filtre Chebychev coupe beaucoup des fr�quences basses
% contrairement aux deux autres filtres. On peut aussi voir que le filtre
% de Hamming a une at�nuation plus forte que celui de Blackman pour les
% basses fr�quences.
