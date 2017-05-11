classdef LEDArduino < handle
    properties
        serialObj;
        validCommands;
        mapPattern;
        validPatterns;
    end
    
    methods(Access=private)
        % send command 'cmd'
        function sendCommand(s,cmd)
            assert(ismember(cmd,s.validCommands));
            fprintf(s.serialObj, cmd);
            pause(0.01);
        end

        %write 
        function writeToSerial(s,cmd)
            fwrite(s.serialObj,cmd,'uint8');
        end

        
    end

    methods
        function s=LEDArduino(comPort)
            if nargin<1
                comPort='COM6';
            end

            s.validPatterns={'left','right','top','bottom','central','ring'};
            s.mapPattern=containers.Map(s.validPatterns,0:numel(s.validPatterns)-1);

            s.validCommands='arGgvz';

            s.serialObj = serial(comPort,'BaudRate',115200);
            try
                fopen(s.serialObj);
            catch ME
                disp(ME.message);
                fclose(s.serialObj);
                error(['Could not open port: ' comPort]);
            end
        end
        
        % destructor
        function delete(s)
            s.reset();
            fclose(s.serialObj);
        end

        function blink(s,num,d)
            s.writeToSerial(['b' num d]);
        end
        
        function advance(s)
            s.sendCommand('a');
        end
        
        function setPower(s,pwr)
            assert(pwr>=0 && pwr<=255);
            s.writeToSerial(['w' pwr]);
        end

        function turnLED(s, led)
            assert(led>=0 && led<=255);
            s.writeToSerial(['i' led]);
        end
        
        function reset(s)
            s.sendCommand('r');
        end

        function turnLEDsOff(s)
            s.sendCommand('z');
        end

        function turnLEDArray(s,arr)
            assert(numel(arr)>0);
            assert(all(arr)>=0 && all(arr)<=255);
            s.writeToSerial(['l' numel(arr) arr]);
        end

        function turnLEDRange(s,startLED, endLED)
            assert(startLED>=0 && startLED<=255);
            assert(endLED>=0 && endLED<=255);
            assert(endLED>=startLED);
            s.writeToSerial(['c' startLED endLED]);
        end

        function turnLEDPattern(s, pattern)
            
            if ischar(pattern)
                assert(s.mapPattern.isKey(pattern),sprintf('Pattern %s is not a valid pattern', pattern));
                patternInd=s.mapPattern(pattern);
            else
                assert(pattern>=0 && pattern<=numel(s.validPatterns)-1,sprintf('Pattern index must be in the range [0,%d]',numel(s.validPatterns)-1));
                patternInd=pattern;
            end
            s.writeToSerial(['p' patternInd]);
        end
        
        function enableCounter(s)
            s.writeToSerial(['e' 1]);
        end
        
        function disableCounter(s)
            s.writeToSerial(['e' 0]);
        end
        
        function setCounter(s,c)
            s.writeToSerial(['s' c]);
        end

        function sendPatterns(s, LEDs, patterns)
            s.writeToSerial(['n' length(LEDs) length(patterns)]);
            
            % send LEDs
            if(length(LEDs)>20)
                batchSize=20;
                numBatches=ceil(length(LEDs)/batchSize);
                for i=1:numBatches
                    ind=(i-1)*batchSize+1:min(length(LEDs),i*batchSize);
                    s.writeToSerial(['L' length(ind) LEDs(ind)]);
                end
            else
                s.writeToSerial(['L' length(LEDs) LEDs]);
            end

            % send patterns
            patternInd=[];
            for iter=1:numel(patterns)
                if(~s.mapPattern.isKey(patterns{iter}))
                    error('Pattern %s is not a valid pattern',patterns{iter});
                end
                patternInd(end+1)=s.mapPattern(patterns{iter});                 
            end
            s.writeToSerial(['P' length(patternInd) patternInd]);
        end

        function pwr=getPower(s)
            s.sendCommand('G');
            pwr=fread(s.serialObj,1,'uint8');
        end

        function count=getCounter(s)
            s.sendCommand('g');
            count=fread(s.serialObj,1,'uint8');
        end

        function status=getInfo(s)
            s.sendCommand('v');
            status.counter=fread(s.serialObj,1,'uint8');
            status.enableFlag=fread(s.serialObj,1,'uint8');
            status.power=fread(s.serialObj,1,'uint8');
            numLEDs = fread(s.serialObj,1,'uint8');
            numPatterns=fread(s.serialObj,1,'uint8');
            status.LEDs=fread(s.serialObj, numLEDs, 'uint8');
            status.patterns=fread(s.serialObj, numPatterns, 'uint8');
        end

        function parseChannel(s, chnl)
            if(strcmp(chnl(1:3),'LED'))
                LED=uint8(str2double(chnl(4:end)));
                s.turnLED(LED);
            elseif length(chnl)>8 && strcmp(chnl(1:7),'CUSTOM_')
                str=chnl(8:end);
                startLED=uint8(str2double(str(1:3)));
                endLED = uint8(str2double(str(5:end)));
                s.turnLEDRange(startLED,endLED);
            elseif length(chnl)>7 && strcmp(chnl(1:6),'ARRAY_')
                str = chnl(7:end);
                s.turnLEDPattern(str);
            end
        end
    end
end