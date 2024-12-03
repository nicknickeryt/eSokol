clc; clear; close all;

% x is targetVelocityWheel, in km/h
dx = 0.1;
x = 0:dx:25;

algorithm_eq_component = -1.0;

y = ( (-0.00000044 .* (x.^5) ) - ( 0.000049 .* (x.^4) ) + ...
    ( 0.00164 .* (x.^3) ) + ( 0.0169 .* (x.^2) ) + ( 1.1815 .* x ) + 18.912 + ...
    algorithm_eq_component);

    plot(x,y); hold on; grid on; axis tight;
        xlabel("V [km/h]"); ylabel("Duty cycle [%]");

    yy =  80 + 0.1 .*( (650 - 100) .* mod((x+50), 6.25) ./ 6.25);
    % yy = generateSawtooth(200, 700, 4, 25, dx);
    plot(x, yy);



    fs = 44100; 
    dt = 0.01; 
    soundDuration = 0.01; 

    soundSignal = [];
    for freq = yy
        t = 0:1/fs:soundDuration; 
        soundWave = sin(2 * pi * freq * t); 
        soundSignal = [soundSignal, soundWave]; 
        soundDuration = soundDuration + 0.0001; 
    end

    soundSignal = soundSignal / max(abs(soundSignal));
    % sound(soundSignal, fs);

    hold off;
    % figure(2);
    % plot(soundSignal);

function [y] = generateSawtooth(minFreq, maxFreq, periods, xMax, dx)
    x = 0:dx:xMax;
    
    periodLength = xMax / periods;
    
    normalizedX = mod(x, periodLength) / periodLength;
    
    y = minFreq + (maxFreq - minFreq) * normalizedX;
end