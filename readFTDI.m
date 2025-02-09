f_ID = fopen("test.bin", 'rb');
iterRead = 1e7;
i = 1;
distL = single.empty;
distV = single.empty;

% while true
    
    data = fread(f_ID, iterRead, "uint8=>single");
    
    
    % if isempty(data)
    %     break
    % end

    offs = 0;
    len = 1e6;
    
    d2plt = data(offs+1:end);
       
    dd2plt = diff(d2plt);
    indList = find(dd2plt ~= 1 & dd2plt ~= -255);
    valuesList = unique(dd2plt);

    distL = cat(1, distL, diff(indList)); 
    distV = unique(distL);
    ind = 1600;
    plot(data(indList(ind)-1e3:indList(ind)+1e3))
    i = i + 1;
% end

    % plot(dd2plt)
    fclose(f_ID);