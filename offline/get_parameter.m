global MERGE_THRESHOLD
global CHAR_THRESHOLD

MERGE_THRESHOLD = 0.075;

%% Usage:
%
% find the correct value starting from this value.
% The script will continue searching for higher possible
% threshold until a false positive is encountered.
%
% The threshold parameter is output into threshold.txt.
%
char_threshold_search_start = 1;


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

CHAR_THRESHOLD = 250;

%% Import Data

global DATA_DIR;
DATA_DIR = "output";

global KEYFILE;
KEYFILE = "output/analyze.txt";

global MAPPING_SET;
global MAPPING_SET_APPEAR;
global MAPPING_DISTANCE;

global lookup_table
lookup_table = read_table_file();

% set up MAPPING_SET;
tmp_len = size(lookup_table);
tmp_len = tmp_len(1);
tmp_mapper = lookup_table(:, 1);
tmp_mapper = tmp_mapper.Variables;
MAPPING_SET = containers.Map(tmp_mapper, zeros(tmp_len, 1));
MAPPING_SET_APPEAR = containers.Map(tmp_mapper, zeros(tmp_len, 1));
MAPPING_DISTANCE = zeros(16);


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Prepare lookup table
%
% prepare lookup table first as global value to avoid long running
global LOOKUP_TABLE
LOOKUP_TABLE = lookup_table;
global LOOKUP_TABLE_ROWS;
LOOKUP_TABLE_ROWS = height(LOOKUP_TABLE);
global CURR_ROWTABLE
CURR_ROWTABLE = {};
global CURR_CHAR
CURR_CHAR = {};
global VALUE_ROWTABLE;
VALUE_ROWTABLE = {};
global VALUE_ARRAY;
VALUE_ARRAY = {};
for row = 1:LOOKUP_TABLE_ROWS
    CURR_ROWTABLE{row} = LOOKUP_TABLE(row, :);
    tmp_value = CURR_ROWTABLE{row};
    tmp_value = tmp_value(1, 1);
    tmp_value = tmp_value.Variables;
    tmp_value = tmp_value{1, 1};
    CURR_CHAR{row} = tmp_value;
    tmp_value = CURR_ROWTABLE{row};
    VALUE_ROWTABLE{row} = tmp_value(1, 2:end);
    tmp_value = VALUE_ROWTABLE{row};
    VALUE_ARRAY{row} = table2array(tmp_value);
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


files = dir(DATA_DIR + filesep + "output_*.txt");

result_here = search_range(char_threshold_search_start, 10000, files);
fprintf(1, "\n%s%d\n", "The threshold should be ", result_here - 1);
fileid = fopen('threshold.txt', 'w');
fprintf(fileid, '%d\n', result_here);
fclose(fileid);

%% Functions

function result = has_false_positive_single(found_chars, input_chars)
    for i=1:length(found_chars)
        if ~any(strcmp(input_chars, found_chars{i}))
            result = 1;
            break;
        end
    end
    result = 0;
end

function result = has_false_positive(total_found_chars, total_input_chars)
    result = 0;
    for i=1:length(total_input_chars)
        var1 = total_found_chars{i};
        var2 = total_input_chars{i};
        for j=1:length(var1)
            %var2
            %total_found_chars{j}
            if ~any(strcmp(var2, var1{j}))
                % found_chars contain error
                result = 1;
                break;
            end
        end
        if result == 1
            break;
        end
    end
    result = 0;
end

function [char_choice, my_distance] = get_distance(lookup_table, value_row)

    global CURR_CHAR;
    global VALUE_ARRAY;
    global LOOKUP_TABLE_ROWS;
    char_choice = '';
    my_distance = Inf;
    rows = LOOKUP_TABLE_ROWS;
    for row = 1:rows
        curr_char = CURR_CHAR{row};
        value_array = VALUE_ARRAY{row};
        euclideanDistance = norm(value_array - value_row, 2);
        if euclideanDistance < my_distance
            char_choice = curr_char;
            my_distance = euclideanDistance;
        end
    end
end


function result = read_table_file()
    global KEYFILE;
    T = readtable(KEYFILE);
    TT = rmmissing(T);
    result = TT;
end


function [input_chars, input_chars_timestamp, datalines] = read_data_file(filename)
    allchar = fileread(filename);
    C = strsplit(allchar, '\n');
    result = C;
    % last: empty
    % last-1: timestamps ; all chars
    % 1 to last-2: data
    tmp_len = length(C);
    lastline_chars = strsplit(C{tmp_len - 1}, ' ');
    lastline_size = length(lastline_chars) - 1;
    if mod(lastline_size, 2) ~= 0
        error('ERROR: last line is not mod 2!!!');
    end
    correct_size = lastline_size / 2;
    input_chars = lastline_chars(correct_size + 1:lastline_size);
    input_chars_timestamp = lastline_chars(1:correct_size);
    input_data_lines = C(1:tmp_len - 2);
    
    real_input_data_lines = {};
    global start_pair;
    global end_pair;
    % now, remove found info (not start with "DEBUG: " from input_data_lines
    for i=1:length(input_data_lines)
        if strcmp(input_data_lines{1, i}(1:6), 'DEBUG:')
            real_input_data_lines{end+1} = input_data_lines{1, i};
        end
        if strcmp(input_data_lines{1, i}(1:6), 'START:')
            tmp = split(input_data_lines{1, i}, ':');
            tmp = tmp{2};
            tmp = strip(tmp);
            start_pair{end+1} = tmp;
        end
        if strcmp(input_data_lines{1, i}(1:7), 'FINISH:')
            tmp = split(input_data_lines{1, i}, ':');
            tmp = tmp{2};
            tmp = strip(tmp);
            end_pair{end+1} = tmp;
        end
    end
    input_data_lines = real_input_data_lines;
    
    % now, process input_data_lines
    for i=1:length(input_data_lines)
        line_data = input_data_lines{1, i};
        tmp1 = strsplit(line_data, ':');
        curr_timestamp = tmp1{1, 2};
        curr_timestamp = erase(curr_timestamp, ' ');
        tmp2 = erase(tmp1{1, 3}, ';');
        tmp3 = strsplit(tmp2, ' ');
        curr_counters = tmp3(1, 2:length(tmp3));
        input_data_lines{1, i} = {curr_timestamp, curr_counters};
    end
    
    % convert cell structure to array
    datalines = [];
    for i=1:length(input_data_lines)
        tmp = input_data_lines{1, i}{1, 1};
        datalines(i, 1) = str2num(tmp);
        for j=1:length(input_data_lines{1, i}{1, 2})
            tmp = input_data_lines{1, i}{1, 2}{1, j};
            datalines(i, j+1) = str2num(tmp);
        end
    end
end

function [found_data, found_chars, input_chars, ...
    input_chars_timestamp, datalines, distance_array_part] ...
    = real_fileprocessing(curr_filename, lookup_table)
    
    [input_chars, input_chars_timestamp, datalines] = ...
        read_data_file(curr_filename);
    
    DEBUG = false;
    global MERGE_THRESHOLD
    global CHAR_THRESHOLD
        
    found_data = [];
    found_chars = {};
    found_dataset_counter = 0;
    
    distance_array = [];
    
    % kkk: total lines
    event_row = zeros(length(datalines), 1);
    kkk = length(datalines);
    %fprintf(1, "%s\n", curr_filename);
    
    for i=1:kkk
        %fprintf(1, "Now on %d/%d\n", i, kkk);
        search_row_counter = i - 1;
        found_in_window = false;
        while (search_row_counter > 0) && (~found_in_window) && ...
                (datalines(i, 1) - datalines(search_row_counter, 1) < MERGE_THRESHOLD)
            if event_row(search_row_counter, 1) == 1
                found_in_window = true;
                break;
            end
            search_row_counter = search_row_counter - 1;
        end % while search_row
        if found_in_window

            continue;
        end
        
        % now: if identified as event
        my_value_row = datalines(i, 2:end);
        [char_choice, my_distance] = get_distance(lookup_table, my_value_row);

        if my_distance < CHAR_THRESHOLD

            found_dataset_counter = found_dataset_counter + 1;
            found_data(found_dataset_counter, 1) = my_distance; % distance
            found_data(found_dataset_counter, 2) = datalines(i, 1); % timestamp
            found_chars{1, found_dataset_counter} = char_choice;
            distance_array = [distance_array my_distance];
            event_row(i, 1) = 1;
        else
            % identify combined
            if i-1 < 1
                continue;
            end
            datalines_tmp = [];
            for j = 2:length(datalines(i, :))
                datalines_tmp(i, j) = datalines(i-1, j) + datalines(i, j);
            end
            my_value_row = datalines_tmp(i, 2:end);
            [char_choice, my_distance] = get_distance(lookup_table, my_value_row);
                if my_distance < CHAR_THRESHOLD

                    found_dataset_counter = found_dataset_counter + 1;
                    found_data(found_dataset_counter, 1) = my_distance; % distance
                    found_data(found_dataset_counter, 2) = datalines(i, 1); % timestamp
                    found_chars{1, found_dataset_counter} = char_choice;
                    distance_array = [distance_array my_distance];
                    event_row(i, 1) = 1;
                end
        end
    end % for i=1:kkk
    
    distance_array_part = distance_array;
end


function result_here = do_this_search(char_threshold, files)

global lookup_table;
total_length = length(files);
result_here = 1;

global CHAR_THRESHOLD;
CHAR_THRESHOLD = char_threshold;


total_char_length_stat = 0;

num_allok = 0;
num_fail = 0;
num_char_ok = 0;
num_char_total = 0;

distance_array = [];

total_found_data = cell(1, total_length);
total_found_chars = cell(1, total_length);
total_input_chars = cell(1, total_length);
total_input_chars_timestamp = cell(1, total_length);
total_datalines = cell(1, total_length);
total_distance_array_part = cell(1, total_length);

global start_pair;
global end_pair;
start_pair = {};
end_pair = {};


for k=1:total_length
    curr_filename = "" + files(k).folder + filesep + files(k).name;
    [total_found_data{k}, total_found_chars{k}, total_input_chars{k}, ...
        total_input_chars_timestamp{k}, total_datalines{k}, ...
        total_distance_array_part{k}] = real_fileprocessing(curr_filename, lookup_table);
    %fprintf(1, "%d/%d", k, total_length);
end
for k=1:total_length
    distance_array = [distance_array total_distance_array_part{k}];
end

%fprintf(1, "DONE with data processing!\n");

    
for k =1:total_length
 
    found_chars = total_found_chars{k};
    input_chars = total_input_chars{k};
    
    if (length(found_chars) > length(input_chars)) % false positive
        result_here = 0;
        break;
    end
    
    if has_false_positive_single(found_chars, input_chars)
        result_here = 0;
        break;
    end
end
end

function result_here = search_range(lowrange, highrange, files)
    for i=lowrange:highrange
        fprintf(1, 'searching %d...\n', i);
        my_result = do_this_search(i, files);
        if my_result == 0
            result_here = i;
            break;
        end
    end
end
