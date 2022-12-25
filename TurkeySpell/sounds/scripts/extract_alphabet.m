% Extract 26 alphabet sounds from alphabet.m4a and save as WAVs.

% 1. Get the sounds.
filepath = fileparts(mfilename('fullpath'));
[y,Fs] = audioread(fullfile(...
    filepath,...
    '..',...
    'sources',...
    'alphabet.m4a'));

% 2. Create a place to store the sounds in the workspace.
soundBank = struct('letter',char(),'sound',cell(26,1));

% 3. Extract each sound. I recorded them at 2-second intervals starting at
% 3 seconds.
for idx = 1:26
    soundBank(idx).letter = 'a'+idx-1;
    soundBank(idx).sound = y((1+2*idx)*Fs+1:(3+2*idx)*Fs);
end

% 4. Save each sound to a file.
for idx = 1:26
    audiowrite(fullfile(filepath,'..',[soundBank(idx).letter '.wav']),...
        soundBank(idx).sound,Fs);
end