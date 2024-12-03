clc; clear; close all;

% Wykresy Bodego dla low-pass

R = 1e3;      
C = 200e-9;   
f_c = 1 / (2 * pi * R * C)  % f cutoff

f_min = 10;               
f_max = 10e3;             
f = logspace(log10(f_min), log10(f_max), 500); 

% Wzór na H
H_mag = 1 ./ sqrt(1 + (2 * pi * f * R * C).^2); 
H_phase = -atan(2 * pi * f * R * C) * (180 / pi); 

    figure(1);
        subplot(2,1,1);
            semilogx(f, 20*log10(H_mag)); grid on;
                title("Wykresy Bodego dla układu RC, R=" + R + "Ω, C=" + C + "F");
                xline(f_c,"r");
                xline(784,"g-");
                xline(2500,"b-");
                xlabel('Frequency (Hz)');
                ylabel('Magnitude (dB)');

        subplot(2,1,2);
            semilogx(f, H_phase); grid on;
                xline(f_c,"-");
                xlabel('Frequency (Hz)');
                ylabel('Phase (degrees)');
