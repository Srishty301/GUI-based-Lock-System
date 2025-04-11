function KeypadLockSystem()
    % Create the main figure for the GUI
    hFig = figure('Name', 'Keypad Lock System', ...
                  'Position', [100, 100, 400, 500], ...
                  'MenuBar', 'none', ...
                  'NumberTitle', 'off', ...
                  'Color', [0 0 0], ... % Base black color
                  'CloseRequestFcn', @onClosing);

    % Create gradient background (Dark Gray to Black)
    ax = axes('Parent', hFig, 'Position', [0 0 1 1], 'Visible', 'off');
    [X,Y] = meshgrid(linspace(0,1,400), linspace(0,1,500));
    gradient = 0.3 - Y*0.3; % Vertical gradient from 0.3 to 0
    imagesc(ax, gradient);
    colormap(ax, gray);
    axis(ax, 'off');
    uistack(ax, 'bottom');

    % Initialize serial communication (adjust COM port as needed)
    comPort = 'COM11'; % Change this to match your PSoC's port
    baudRate = 9600;
    ser = serialport(comPort, baudRate);
    pause(2); % Wait for serial connection to initialize

    % Initialize entry text
    entryText = '';

    % Create display field with dark theme
    hDisplay = uicontrol('Style', 'edit', ...
                         'Position', [50, 400, 300, 40], ...
                         'FontSize', 14, ...
                         'HorizontalAlignment', 'center', ...
                         'BackgroundColor', [0.15 0.15 0.15], ...
                         'ForegroundColor', [1 1 1], ...
                         'FontWeight', 'bold');

    % Define button labels
    buttons = {'1', '2', '3'; ...
               '4', '5', '6'; ...
               '7', '8', '9'; ...
               '*', '0', '#'; ...
               'Enter', 'Cancel', 'New Pass'};

    % Create buttons with dark theme
    for row = 1:size(buttons, 1)
        for col = 1:size(buttons, 2)
            label = buttons{row, col};
            xPos = 50 + (col - 1) * 100;
            yPos = 350 - (row - 1) * 60;
            
            % Color adjustments for dark theme
            if strcmp(label, 'Enter')
                bgColor = [0.1, 0.6, 0.1]; % Dark green
            elseif strcmp(label, 'Cancel')
                bgColor = [0.6, 0.1, 0.1]; % Dark red
            elseif strcmp(label, 'New Pass')
                bgColor = [0.1, 0.3, 0.7]; % Dark blue
            else
                bgColor = [0.25 0.25 0.25]; % Dark gray
            end

            uicontrol('Style', 'pushbutton', ...
                      'String', label, ...
                      'Position', [xPos, yPos, 80, 40], ...
                      'FontSize', 12, ...
                      'BackgroundColor', bgColor, ...
                      'ForegroundColor', [1 1 1], ...
                      'FontWeight', 'bold', ...
                      'Callback', @(src,event)buttonClick(label));
        end
    end

    % Timer to check serial feedback
    feedbackTimer = timer('ExecutionMode', 'fixedRate', ...
                          'Period', 0.1, ...
                          'TimerFcn', @checkSerialFeedback);
    start(feedbackTimer);

    % Callback function for button clicks
    function buttonClick(value)
        if ismember(value, {'0','1','2','3','4','5','6','7','8','9','*','#'})
            if length(entryText) < 4
                entryText = strcat(entryText, value);
                hDisplay.String = repmat('*', size(entryText));
                write(ser, value, "char");
            end
        elseif strcmp(value, 'Enter')
            write(ser, "E", "char"); % Send Enter command to PSoC
            
            if ~strcmp(entryText, "1234") % Replace "1234" with the correct password logic
                playErrorTone(); % Play error sound when wrong PIN is entered
                msgbox('Wrong PIN!', 'Error');
            end
            
            entryText = '';
            hDisplay.String = '';
        elseif strcmp(value, 'Cancel')
            entryText = '';
            hDisplay.String = '';
            write(ser, "C", "char"); % Send Cancel command to PSoC
        elseif strcmp(value, 'New Pass')
            write(ser, "N", "char"); % Send New Pass command to PSoC
            msgbox('Enter the old password on the keypad, then the new one.', 'Info');
        end
    end

    % Callback function to check serial feedback
    function checkSerialFeedback(~, ~)
        if ser.NumBytesAvailable > 0
            response = char(read(ser, ser.NumBytesAvailable));
            response = strtrim(response); % Remove trailing whitespace

            switch response
                case "U"
                    msgbox('System Unlocked!', 'Success');
                case "L"
                    playErrorTone(); % Play error sound when wrong PIN feedback is received from PSoC
                    msgbox('Wrong Password!', 'Error');
                case "W"
                    playErrorTone(); % Play error sound when wrong old password feedback is received from PSoC
                    msgbox('Wrong Old Password!', 'Error');
                case "S"
                    msgbox('New Password Set!', 'Success');
            end
        end
    end

    % Function to play error sound (custom tone)
    function playErrorTone()
        fs = 44100; % Sampling frequency (Hz)
        duration = 0.5; % Duration of the tone (seconds)
        freq = 400; % Frequency of the tone (Hz)
        t = linspace(0, duration, fs * duration); % Time vector
        y = sin(2 * pi * freq * t); % Generate sine wave for tone
        [y, fs] = audioread('wrong-buzzer-6268.wav');
        sound(y, fs); % Play sound using MATLAB's sound function
    end

    % Callback function for closing the GUI
    function onClosing(~, ~)
        stop(feedbackTimer);
        delete(feedbackTimer);
        clear ser;
        delete(hFig);
    end

end
