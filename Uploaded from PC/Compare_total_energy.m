%% ENERGY
clear;
close all;
clc;

predicted = readtable('finalform/exp_predicted_en_per_minute.csv');
peak_date = table2array(predicted(1:height(predicted(:,1)),'datetime'));
minute_diff = table2array(predicted(1:height(predicted(:,1)),'minute_diff'));
en_start = table2array(predicted(1:height(predicted(:,1)),'energy_start'));
en_stop = table2array(predicted(1:height(predicted(:,1)),'energy_stop'));
cal_en_diff = table2array(predicted(1:height(predicted(:,1)),'cal_energy_diff'));


%table_height = height(predicted);
i = 1;
first_peak = peak_date(1);
last_peak = peak_date(height(predicted));
[y,m,d] = ymd(first_peak);
[y_last, m_last, d_last] = ymd(last_peak);

[year, month, day] = ymd(peak_date);
day_list = table(year, month, day);
day_list = unique(day_list);

% compare_energy = zeros(height(predicted), 4);
% compare_energy_table = array2table(compare_energy, 'VariableNames',...
%     {'first_peak_each_day','last_peak_each_day', 'real_en_sum', 'cal_en_sum'});
compare_energy_table = table();

while(true)
    if d > d_last || i > height(day_list)
        break;
    end
    first_index = find(peak_date(:,1) >= ...
        datetime(day_list.year(i), day_list.month(i), day_list.day(i), 0, 0, 0),1,'first');
    last_index = find(peak_date(:,1) <= ...
        datetime(day_list.year(i), day_list.month(i), day_list.day(i), 23, 59, 59),1,'last');
    
    first_peak_on_each_day = peak_date(first_index);
    last_peak_on_each_day = peak_date(last_index);
    real_en_sum = en_stop(last_index) - en_start(first_index);
%     real_en_sum = en_stop(find(peak_date(:,1) == last_peak_on_each_day, 1, 'last')) - ...
%         en_start(find(peak_date(:,1) == first_peak_on_each_day, 1, 'first'));
    
    cal_en_sum = 0;
    for j = first_index:last_index
        cal_en_sum = cal_en_sum + cal_en_diff(j);
    end
    time_span = seconds(last_peak_on_each_day + minute_diff(last_index)/(24*60) ...
        - first_peak_on_each_day)/3600;
    
    buffer = table(...
        first_peak_on_each_day, ...
        last_peak_on_each_day, ...
        real_en_sum, ...
        cal_en_sum, ...
        time_span);
    compare_energy_table = [compare_energy_table; buffer];
    
    i = i + 1;
    d = d + 1;
end

predicted.reduction = (predicted.cal_energy_diff - predicted.energy_diff)./predicted.cal_energy_diff *100;
toDelete = predicted.reduction < -25;
predicted(toDelete,:) = [];

mean_reduction = mean(predicted.reduction, 1);
std_reduction = std(predicted.reduction, 1);
stdError_reduction = std(predicted.reduction, 1)/sqrt(height(predicted.reduction));
percentReduction_summary = table();
percentReduction_summary.mean = mean_reduction;
percentReduction_summary.std = std_reduction;
percentReduction_summary.stdError = stdError_reduction;
percentReduction_summary.max = max(predicted.reduction);
percentReduction_summary.min = min(predicted.reduction);
percentReduction_summary.up95 = percentReduction_summary.mean + percentReduction_summary.std*1.96;
percentReduction_summary.low95 = percentReduction_summary.mean - percentReduction_summary.std*1.96;

figure
plot(predicted.datetime, predicted.energy_diff./predicted.minute_diff, '*')
hold on
plot(predicted.datetime, predicted.predicted_en_per_minute, 'r*')
ylabel('Energy rate (kWh/min)')
xlabel('Experiment date')
legend({'Optimized Usage', 'Predicted Normal Usage'})
title('Opimized vs Predicted normal energy rate')
grid
saveas(gcf,'Figures/Energy-Opimized vs Predicted normal energy rate.fig')
saveas(gcf,'Figures/Energy-Opimized vs Predicted normal energy rate.png')

figure
plot(predicted.datetime, predicted.reduction,'*k')
hold on
%yline(percentReduction_summary.max,'--r','Max');
yline(percentReduction_summary.mean + percentReduction_summary.std*1.96,'--b','95% Upper bound');
yline(percentReduction_summary.mean,'--r','Mean');
yline(percentReduction_summary.mean - percentReduction_summary.std*1.96,'--b','95% Lower bound');
%yline(percentReduction_summary.min,'--r','Min');
ylabel('Reduction (%)')
xlabel('Experiment date')
title('Percent Reduction of energy rate')
grid
saveas(gcf,'Figures/Energy-Percent Reduction of energy rate.fig')
saveas(gcf,'Figures/Energy-Percent Reduction of energy rate.png')


compare_energy_table.day_reduction = ((compare_energy_table.cal_en_sum ...
    - compare_energy_table.real_en_sum)./compare_energy_table.cal_en_sum) * 100;
compare_summary = table();
compare_summary.mean = mean(compare_energy_table.day_reduction);
compare_summary.std = std(compare_energy_table.day_reduction);
compare_summary.stdError = compare_summary.std/sqrt(height(compare_energy_table));
compare_summary.max = max(compare_energy_table.day_reduction);
compare_summary.min = min(compare_energy_table.day_reduction);
compare_summary.up95 = compare_summary.mean + compare_summary.std*1.96;
compare_summary.low95 = compare_summary.mean - compare_summary.std*1.96;

figure
plot(compare_energy_table.first_peak_on_each_day, compare_energy_table.day_reduction,'*k')
ylabel('Reduction (%)')
xlabel('Experiment date')
title('Percent Reduction of TOTAL Energy on each day')
grid
yline(mean(compare_energy_table.day_reduction),'--r','Mean');
yline(mean(compare_energy_table.day_reduction) + std(compare_energy_table.day_reduction)*1.96,'--b','95% Upper bound');
yline(mean(compare_energy_table.day_reduction) - std(compare_energy_table.day_reduction)*1.96,'--b','95% Lower bound');
saveas(gcf,'Figures/Energy-Percent Reduction of TOTAL Energy on each day.fig')
saveas(gcf,'Figures/Energy-Percent Reduction of TOTAL Energy on each day.png')


