iterRead = 1e8;

distL = single.empty;
distV = single.empty;
% for j=1:4
    f_ID = fopen("test.bin", 'rb'); %C:\Users\dmitr\Desktop\USBSniffExport
    i = 1;
    % system(".\main_file.exe")
    while true
        
        data = fread(f_ID, iterRead, "uint8=>single");
        
        % 
        if isempty(data)
            break
        end
    
        offs = 0;
        len = 1e6;
        
        sawLen = length(data);
        sawS = zeros(sawLen, 1);
        sawS(1) = data(1);
        for r=2:sawLen
            sawS(r) = sawS(r-1) + 1;
            if (sawS(r) == 256)
                sawS(r) = 0;
            end
        end
        % d2plt = data(offs+1:end);
           
        dd2plt = diff(data);
        % indList = find(dd2plt ~= 1 & dd2plt ~= -255);
        indList = find(dd2plt == -27 | dd2plt == -23 | dd2plt == -19 | dd2plt == -15);
        indListL = length(indList);
        valuesList = unique(dd2plt);
    
        IterInd = repelem(i, length(indList)).';
        distL = cat(1, distL, [indList IterInd] ); 
        distLL = length(distL(:,1));
        distV = unique(distL(:,1));
        % ind = 2;  
        
        if ~isempty(indList) & i ~= 1
            disp(length(indList))
            for i=5:length(indList)
                tmpV1 = data(indList(i)-150:indList(i)+150);
                tmpV2 = sawS(indList(i)-150:indList(i)+150);
                plot(1:length(tmpV1), tmpV1, 1:length(tmpV2), tmpV2)
                pause
            end
        end
        i = i + 1;
        % pause
        % end
        
    end
    % disp(j)
        % plot(data)
        fclose(f_ID);
% end