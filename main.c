clc;
clear;

% Create Arduino object with required libraries
ar = arduino('COM4', 'Uno', 'Libraries', {'Ultrasonic'}, 'ForceBuildOn', true);

% Create ultrasonic object (Trig = D10, Echo = D8)
ul = ultrasonic(ar, 'D10', 'D8');

% Motor Control Pins
motorIn1 = 'D5';
motorIn2 = 'D6';
motorEnable = 'D9';

% LED Pins
led1 = 'D13'; % Mode/Status LED 1
led2 = 'D12'; % Mode/Status LED 2

% Set motor and LED control pins as outputs
configurePin(ar, motorIn1, 'DigitalOutput');
configurePin(ar, motorIn2, 'DigitalOutput');
configurePin(ar, motorEnable, 'PWM');
configurePin(ar, led1, 'DigitalOutput');
configurePin(ar, led2, 'DigitalOutput');

% Initialize variables
speed = 0;
set_speed = 0;
distance = 0;
mode = 0; % 0: Normal, 1: CC, 2: ACC
constant = 0;
blinkState = 0; % For blinking LEDs

disp("Starting Adaptive Cruise Control...");

while true
    % Read analog voltages from buttons
    increase_speed = readVoltage(ar, 'A0');
    decrease_speed = readVoltage(ar, 'A1');
    cancel = readVoltage(ar, 'A2');
    set_btn = readVoltage(ar, 'A3');
    acc_btn = readVoltage(ar, 'A4');
    
    % Read ultrasonic distance (in meters)
    distance = readDistance(ul);
    
    % Mode selection
    if cancel >= 4
        mode = 0;
    elseif set_btn >= 4
        mode = 1;
        set_speed = speed;
    elseif acc_btn >= 4
        mode = 2;
        constant = speed;
    end

    % --- Normal Mode ---
    if mode == 0
        % LEDs: Normal Mode (LED1 OFF, LED2 ON)
        writeDigitalPin(ar, led1, 0);
        writeDigitalPin(ar, led2, 1);

        if increase_speed >= 4
            speed = speed + 1;
            pause(0.1);
        elseif decrease_speed >= 4
            speed = speed - 1;
            pause(0.1);
        else
            speed = speed - 1;
            pause(0.5);
        end

        if speed < 0
            speed = 0;
        end

        fprintf('Normal Mode | Speed: %d | Distance: %.2f m\n', speed, distance);

    % --- Cruise Mode ---
    elseif mode == 1
        % LEDs: Cruise Mode (LED1 ON, LED2 OFF)
        writeDigitalPin(ar, led1, 1);
        writeDigitalPin(ar, led2, 0);

        if increase_speed >= 4
            speed = speed + 1;
            set_speed = speed;
            pause(0.1);
        elseif decrease_speed >= 4
            speed = speed - 1;
            set_speed = speed;
            pause(0.1);
        else
            speed = set_speed;
        end

        if speed < 0
            speed = 0;
        end

        fprintf('Cruise Mode | Speed: %d | Distance: %.2f m\n', speed, distance);

    % --- Adaptive Cruise Control Mode ---
    elseif mode == 2
        % ACC Mode LED logic
        if distance < 0.2
            % Blink both LEDs when too close
            blinkState = ~blinkState;
            writeDigitalPin(ar, led1, blinkState);
            writeDigitalPin(ar, led2, blinkState);
        else
            % Both LEDs steady ON when safe
            writeDigitalPin(ar, led1, 1);
            writeDigitalPin(ar, led2, 1);
        end

        if distance < 0.2
            speed = speed - 1;
        else
            speed = speed + 1;
        end

        if speed > constant
            speed = constant;
        end

        if speed < 0
            speed = 0;
        end

        fprintf('ACC Mode | Speed: %d | Distance: %.2f m\n', speed, distance);
    end

    % --- Motor Control Logic ---
    % Clamp speed to [0, 100] for PWM scaling
    speed = max(0, min(speed, 100));
    pwmWrite = speed / 100; % Duty cycle [0,1]

    if speed > 0
        % Forward direction
        writeDigitalPin(ar, motorIn1, 1);
        writeDigitalPin(ar, motorIn2, 0);
        writePWMDutyCycle(ar, motorEnable, pwmWrite); % Use writePWMDutyCycle for Uno
    else
        % Stop the motor
        writeDigitalPin(ar, motorIn1, 0);
        writeDigitalPin(ar, motorIn2, 0);
        writePWMDutyCycle(ar, motorEnable, 0);
    end

    % Adjust pause for visible blinking in ACC mode when close
    if mode == 2 && distance < 0.2
        pause(0.1); % slower for visible blink
    else
        pause(0.01); % normal operation
    end
end