%% Finding PEAK
clear;
close all;
clc;

% start on 17 Nov; shift 14 hours
shift = 14;
step = 1;
ac = readtable('test_Export_2021-12-01_12-45-18.xlsx');
ac_date = table2array(ac(1:height(ac(:,1)),'DATE_TIME'));
ac_avg_power = table2array(ac(1:height(ac(:,1)),'AVERAGE_POWER_W__'));
ac_max_power = table2array(ac(1:height(ac(:,1)),'MAX_POWER_W__'));
ac_avg_energy = table2array(ac(1:height(ac(:,1)),'AVERAGE_ENERGY_KWH__'));

outside = readtable('IoT_capstone_outside.csv');
outside_datetime = table2array(outside(1:height(outside(:,1)),'timestamp'));
outside_datetime = outside_datetime + shift/24;
outside_humid = table2array(outside(1:height(outside(:,1)),'humid'));
outside_temp = table2array(outside(1:height(outside(:,1)),'temp'));
outside_light = table2array(outside(1:height(outside(:,1)),'light'));

nextTo = readtable('IoT_capstone_nextTo.csv');
nextTo_datetime = table2array(nextTo(1:height(nextTo(:,1)),'timestamp'));
nextTo_datetime = nextTo_datetime + shift/24;
nextTo_humid = table2array(nextTo(1:height(nextTo(:,1)),'humid'));
nextTo_temp = table2array(nextTo(1:height(nextTo(:,1)),'temp'));
nextTo_heatindex = table2array(nextTo(1:height(nextTo(:,1)),'heat_index'));

main = readtable('IoT_capstone_main.csv');
main_datetime = table2array(main(1:height(main(:,1)),'timestamp'));
main_datetime = main_datetime + shift/24;
main_count = table2array(main(1:height(main(:,1)),'count'));
main_humid = table2array(main(1:height(main(:,1)),'avg_humid'));
main_temp = table2array(main(1:height(main(:,1)),'avg_temp'));
main_heatindex = table2array(main(1:height(main(:,1)),'heat_index'));

past = datetime(2021, 11, 17, 10, 0, 0); % start EXP on 17 Nov 10:00
%limit_date = datetime(2021, 11, 26, 23, 59, 59);
limit_date = ac_date(height(ac_date));
limit_25C_transient = 50;

%%
[pks,locs] = findpeaks(ac_avg_power, 'MinPeakHeight', 100);
summary_peak_table = table();
summary_peak_table.datetime = ac_date(locs);
summary_peak_table.avg_power = ac_avg_power(locs);
summary_peak_table.max_power = ac_max_power(locs);
summary_peak_table.avg_energy = ac_avg_energy(locs);

%% Collecting air data (nextTo, outside) between 2 peaks -> then average them
j = 1;
air_data = zeros(height(summary_peak_table),10);
interval_array = datetime(zeros(height(summary_peak_table)-1,2), 0, 0);
while(true)
    if j+1 > height(summary_peak_table)
        break;
    end
    start = table2array(summary_peak_table(j,'datetime'));
    stop = table2array(summary_peak_table(j+1,'datetime'));
    interval_array(j,:) = [start stop];
    time_diff = etime(datevec(datenum(stop)), datevec(datenum(start)))/60;
    energy_diff = table2array(summary_peak_table(j+1,'avg_energy')) - ...
        table2array(summary_peak_table(j,'avg_energy'));
    buff_out1 = zeros(100,3);
    buff_next1 = zeros(100,3);
    range_out1 = find(outside_datetime >= start & outside_datetime < stop);
    range_next1 = find(nextTo_datetime >= start & nextTo_datetime < stop);
    if ~isempty(range_out1)
        for k = 1:height(range_out1)
            buff_out1(k,:) = [...
                outside_humid(range_out1(k)), ...
                outside_temp(range_out1(k)), ...
                outside_light(range_out1(k))];
        end
    end
    if ~isempty(range_next1)
        for k = 1:height(range_next1)
            buff_next1(k,:) = [...
                nextTo_humid(range_next1(k)), ...
                nextTo_temp(range_next1(k)), ...
                nextTo_heatindex(range_next1(k))];
        end
    end
    
    buff_out1( ~any(buff_out1,2), : ) = [];
    buff_next1( ~any(buff_next1,2), : ) = [];
    air_data(j,:) = [time_diff,...
        energy_diff,...
        table2array(summary_peak_table(j,'avg_energy')),...
        table2array(summary_peak_table(j+1,'avg_energy')),...
        mean(buff_out1(:,1)),...
        mean(buff_out1(:,2)),...
        mean(buff_out1(:,3)),...
        mean(buff_next1(:,1)),...
        mean(buff_next1(:,2)),...
        mean(buff_next1(:,3))];
    j = j+1;
end
interval_table = array2table(interval_array,'VariableNames', {'start','stop'});
clean_air_data = air_data;
clean_air_data_table = array2table(clean_air_data, 'VariableNames', ...
    {'minute_diff','energy_diff','energy_start','energy_stop','out_humid','out_temp','out_light','next_humid','next_temp','next_heatindex'});
complete_peak_table = [summary_peak_table clean_air_data_table];

complete_peak_table = rmmissing(complete_peak_table);
% remove strange data span
complete_peak_table([height(complete_peak_table)-1, height(complete_peak_table)],:) = [];
toDelete = complete_peak_table.minute_diff > 420;
complete_peak_table(toDelete,:) = [];

filename = 'finalform/exp_complete_peak_table.csv';
writetable(complete_peak_table, filename)

%% "main" air data - PMV

avg_data = zeros(height(ac_date), 15);
i = 1;
step2 = 5;
past = datetime(2021, 11, 17, 10, 0, 0); % start EXP on 17 Nov 10:00
skip = false;

while(true)
    buff_out2 = zeros(100,3);
    buff_next2 = zeros(100,3);
    buff_main = zeros(100,3);
    range_out = find(outside_datetime >= past & outside_datetime < past + step2/(24*60));
    range_next = find(nextTo_datetime >= past & nextTo_datetime < past + step2/(24*60));
    range_main = find(main_datetime >= past & main_datetime < past + step2/(24*60));
    if ~isempty(range_out)
        for k = 1:height(range_out)
            buff_out2(k,:) = [...
                outside_humid(range_out(k)), ...
                outside_temp(range_out(k)), ...
                outside_light(range_out(k))];
        end
    end
    if ~isempty(range_next)
        for k = 1:height(range_next)
            buff_next2(k,:) = [...
                nextTo_humid(range_next(k)), ...
                nextTo_temp(range_next(k)), ...
                nextTo_heatindex(range_next(k))];
        end
    end
    if ~isempty(range_main)
        for k = 1:height(range_main)
            buff_main(k,:) = [...
                main_humid(range_main(k)), ...
                main_temp(range_main(k)), ...
                main_count(range_main(k))];
            if main_temp(range_main(k)) > limit_25C_transient
               skip = true; 
            end
        end
    end
    
    if past >= limit_date
        break;
    end
    if skip
        skip = false;
        past = past + step2/(24*60);
        i = i + 1;
        continue;
    end
    buff_out2( ~any(buff_out2,2), : ) = [];
    buff_next2( ~any(buff_next2,2), : ) = [];
    buff_main( ~any(buff_main,2), : ) = [];
    [y, m, d] = ymd(past);
    avg_data(i,:) = [y, m, d, hour(past + step2/(24*60)), minute(past + step2/(24*60)),...
        mean(buff_out2(:,1)),...
        mean(buff_out2(:,2)),...
        mean(buff_out2(:,3)),...
        mean(buff_next2(:,1)),...
        mean(buff_next2(:,2)),...
        mean(buff_next2(:,3)),...
        mean(buff_main(:,1)),...
        mean(buff_main(:,2)),...
        mean(buff_main(:,3)),...
        hour(past + step2/(24*60)) + minute(past + step2/(24*60))/60];
    past = past + step2/(24*60);
    i = i + 1;
end
avg_data( ~any(avg_data,2), : ) = [];
clean_avg_data = avg_data;
clean_avg_data(any(isnan(clean_avg_data),2),:) = [];
clean_avg_data = array2table(clean_avg_data, 'VariableNames',...
    {'year', 'month', 'day', 'hour', 'minute', ...
    'out_humid', 'out_temp', 'out_light',...
    'next_humid', 'next_temp', 'next_heatindex',...
    'main_humid', 'main_temp', 'main_count', 'deci_hours'});
stackedplot(clean_avg_data);
saveas(gcf,'Figures/stackedplot_EXP.fig')

filename = 'finalform/exp_summary_air_data.csv';
writetable(clean_avg_data, filename)
