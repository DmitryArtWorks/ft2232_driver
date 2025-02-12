f_ID = fopen("test.bin", 'rb'); %C:\Users\dmitr\Desktop\USBSniffExport
iterRead = 1e8;
i = 1;
distL = single.empty;
distV = single.empty;

while true
    
    data = fread(f_ID, iterRead, "uint8=>single");
    
    % 
    if isempty(data)
        break
    end

    offs = 0;
    len = 1e6;
    
    % d2plt = data(offs+1:end);
       
    dd2plt = diff(data);
    indList = find(dd2plt ~= 1 & dd2plt ~= -255);
    indListL = length(indList);
    valuesList = unique(dd2plt);

    IterInd = repelem(i, length(indList)).';
    distL = cat(1, distL, [indList IterInd] ); 
    distLL = length(distL(:,1));
    distV = unique(distL(:,1));
    % ind = 2;
    
    % if ~isempty(indList)
    %     disp(length(indList))
    %     for i=1:length(indList)
    %         plot(data(indList(i)-150:indList(i)+150))
    %         pause
    %     end
    % end
    i = i + 1;
    % pause
    % end
    
end

    % plot(data)
    fclose(f_ID);