% 1. Get the sounds.
filepath = fileparts(mfilename('fullpath'));
letters = imread(fullfile(...
    filepath,...
    '..',...
    'sources',...
    'alphabet.jpeg'));

% 2. Create a place to store the sounds in the workspace.
imageBank = struct('letter',char(),'image',cell(26,1));

% 3. Extract each letter. They're in a 5x5 grid with the extra letter on
% the bottom. The letters are significantly brighter than their
% surroundings.
stdVert = std(double(letters),0,2);
stdHoriz = std(double(letters),0,1);
meanVert = mean(double(letters),2);
meanHoriz = mean(double(letters),1);

showPlot = false;
if showPlot
    subplot(2,2,1) %#ok<UNRCH> 
    image(letters)
    subplot(2,2,2)
    plot(squeeze(meanVert),-1:-1:-(size(letters,1)))
    ylim([-size(letters,1) -1])
    subplot(2,2,3)
    plot(1:(size(letters,2)),squeeze(meanHoriz))
    xlim([1 size(letters,2)])
end

% This finds the 5x5 grid passably well.
leftOffset = find(meanHoriz(:,:,3) > 2*meanHoriz(1,1,3),1,'first');
rightOffset = find(meanHoriz(:,:,3) > 2*meanHoriz(1,1,3),1,'last');
topOffset = find(meanVert(:,:,3) > 2*meanVert(1,1,3),1,'first');
bottomOffset = find(meanVert(:,:,3) > 2*meanVert(1,1,3),1,'last');

if showPlot
    figure
    image(letters)
    axis image
    hold on
    plot([1 1]*leftOffset,[1 size(letters,1)])
    plot([1 1]*rightOffset,[1 size(letters,1)])
    plot([1 size(letters,2)],[1 1]*topOffset)
    plot([1 size(letters,2)],[1 1]*bottomOffset)
end

xOffsets = round(linspace(leftOffset,rightOffset,6));
yOffsets = round(linspace(topOffset,bottomOffset,6));
yOffsets = [yOffsets 2*yOffsets(6)-yOffsets(5)];

for idx = 1:26
    imageBank(idx).letter = 'a'+idx-1;
    xIdx = mod(idx-1,5)+1;
    yIdx = floor((idx-1)/5)+1;
    imageBank(idx).image = letters(yOffsets(yIdx):yOffsets(yIdx+1),xOffsets(xIdx):xOffsets(xIdx+1),:);
    if showPlot
        figure
        image(imageBank(idx).image)
        axis image
    end
end

% 4. Save each image to a file.
for idx = 1:26
    imwrite(imageBank(idx).image,fullfile(filepath,'..',[imageBank(idx).letter '.jpg']));
end