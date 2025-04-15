clear; clc; close all;

% Parametry
MIN_V = 0.9;
MAX_V = 2.55;
DUTY_MIN = 0;
DUTY_MAX = 100;
alpha = 0.01;
time = linspace(0, 2, 200); % 200 próbek na 2 sekundy

% Symulacja manetki
throttleVoltage = MIN_V * ones(size(time));
throttleVoltage(50:151) = 2.55;  % nagły skok
throttleVoltage(151:end) = 1.2;  % powrót w dół

% Wyliczenie targetDutyCycle
normalizedVoltage = (throttleVoltage - MIN_V) / (MAX_V - MIN_V);
normalizedVoltage = max(0, min(1, normalizedVoltage));
targetDutyCycle = DUTY_MIN + (DUTY_MAX - DUTY_MIN) * (normalizedVoltage .^ 2.5);

% Wygładzanie EMA
currentDutyCycle = zeros(size(time));
for i = 2:length(time)
    currentDutyCycle(i) = currentDutyCycle(i-1) + alpha * (targetDutyCycle(i) - currentDutyCycle(i-1));
end

% Wykres
figure;
subplot(2,1,1);
plot(time, throttleVoltage, 'k', 'LineWidth', 2);
xlabel('Czas [s]');
ylabel('Throttle Voltage [V]');
title('Symulacja manetki');
grid on;

subplot(2,1,2);
plot(time, targetDutyCycle, 'r--', 'LineWidth', 2); hold on;
plot(time, currentDutyCycle, 'b', 'LineWidth', 2);
xlabel('Czas [s]');
ylabel('Duty Cycle [%]');
title(['Target vs wygładzony Duty Cycle (EMA \alpha=' num2str(alpha) ')']);
legend('TargetDutyCycle', 'CurrentDutyCycle');
grid on;
