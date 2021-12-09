%% ENERGY PER MINUTE
clear;
close all;
clc;


energy = readtable('finalform/collected_predicted_en_per_minute.csv');
energy.en_per_minute = energy.energy_diff ./ energy.minute_diff;
energy.deviation = energy.predicted_en_per_minute - energy.en_per_minute;
energy.percent = energy.deviation./energy.en_per_minute * 100;

start_plot = energy.datetime(1);
stop_plot = datetime(2021, 10, 3, 0, 0, 0);

stat_en = table();
stat_en.mean = mean(energy.en_per_minute);
stat_en.std = std(energy.en_per_minute);
stat_en.up95 = stat_en.mean + stat_en.std*1.96;
stat_en.low95 = stat_en.mean - stat_en.std*1.96;

stat_enP = table();
stat_enP.mean = mean(energy.predicted_en_per_minute);
stat_enP.std = std(energy.predicted_en_per_minute);
stat_enP.up95 = stat_enP.mean + stat_enP.std*1.96;
stat_enP.low95 = stat_enP.mean - stat_enP.std*1.96;

stat_dev = table();
stat_dev.mean = mean(energy.deviation);
stat_dev.std = std(energy.deviation);
stat_dev.up95 = stat_dev.mean + stat_dev.std*1.96;
stat_dev.low95 = stat_dev.mean - stat_dev.std*1.96;

stat_percent = table();
stat_percent.mean = mean(energy.percent);
stat_percent.std = std(energy.percent);
stat_percent.up95 = stat_percent.mean + stat_percent.std*1.96;
stat_percent.low95 = stat_percent.mean - stat_percent.std*1.96;

figure
hold on
plot(energy.datetime, energy.en_per_minute, '*');
plot(energy.datetime, energy.predicted_en_per_minute, '*');
ylabel('Energy rate (kWh/min)')
xlabel('Date')
title('Real vs Predicted Energy Rate')
xlim([start_plot, stop_plot])
ylim([0 0.03])
yEn_mean = yline(stat_en.mean,'--k','Real Mean');
yEn_up95 = yline(stat_en.up95,'--b','Real 95% Upper');
yEn_low95 = yline(stat_en.low95,'--b','Real 95% Lower');
yEnp_mean = yline(stat_enP.mean,'--k','Predicted Mean');
yEnp_up95 = yline(stat_enP.up95,'--r','Predicted 95% Upper');
yEnp_low95 = yline(stat_enP.low95,'--r','Predicted 95% Lower');
yEn_mean.LabelVerticalAlignment = 'bottom';
legend({'Real', 'Predicted'})
grid
saveas(gcf,'Figures/EVAL-Real vs Predicted Energy Rate.fig')
saveas(gcf,'Figures/EVAL-Real vs Predicted Energy Rate.png')

figure
hold on
plot(energy.datetime, energy.deviation, '*r');
ylabel('Deviation (kWh/min)')
xlabel('Date')
title('Deviation of Predicted Energy Rate')
xlim([start_plot, stop_plot])
ylim([-0.015 0.015])
yEnD_mean = yline(stat_dev.mean,'--k','Mean');
yEnD_up95 = yline(stat_dev.up95,'--b','95% Upper');
yEnD_low95 = yline(stat_dev.low95,'--b','95% Lower');
grid
saveas(gcf,'Figures/EVAL-Deviation of Predicted Energy Rate.fig')
saveas(gcf,'Figures/EVAL-Deviation of Predicted Energy Rate.png')

figure
hold on
plot(energy.datetime, energy.percent, '*k');
xlim([start_plot, stop_plot])
ylabel('Percentage (%)')
xlabel('Date')
title('Percentage Deviation of Predicted Energy Rate')
xlim([start_plot, stop_plot])
yEnT_mean = yline(stat_percent.mean,'--r','Mean');
yEnT_up95 = yline(stat_percent.up95,'--b','95% Upper');
yEnT_low95 = yline(stat_percent.low95,'--b','95% Lower');
grid
saveas(gcf,'Figures/EVAL-Percentage Deviation of Predicted Energy Rate.fig')
saveas(gcf,'Figures/EVAL-Percentage Deviation of Predicted Energy Rate.png')
%}

%% PMV

pmv = readtable('finalform/collected_real_and_predicted_pmv.csv');
pmv.datetime = datetime(pmv.year, pmv.month, pmv.day, pmv.hour, pmv.minute, 0);
pmv.deviation = pmv.PMV - pmv.predicted_PMV;
pmv.percent = pmv.deviation./pmv.PMV * 100;
pmv = pmv(~any(isinf(pmv.percent), 2),:);

start_plot = pmv.datetime(1);
stop_plot = datetime(2021, 10, 3, 0, 0, 0);

stat_pmv = table();
stat_pmv.mean = mean(pmv.PMV);
stat_pmv.std = std(pmv.PMV);
stat_pmv.up95 = stat_pmv.mean + stat_pmv.std*1.96;
stat_pmv.low95 = stat_pmv.mean - stat_pmv.std*1.96;

stat_pmvP = table();
stat_pmvP.mean = mean(pmv.predicted_PMV);
stat_pmvP.std = std(pmv.predicted_PMV);
stat_pmvP.up95 = stat_pmvP.mean + stat_pmvP.std*1.96;
stat_pmvP.low95 = stat_pmvP.mean - stat_pmvP.std*1.96;

stat_pmvdev = table();
stat_pmvdev.mean = mean(pmv.deviation);
stat_pmvdev.std = std(pmv.deviation);
stat_pmvdev.up95 = stat_pmvdev.mean + stat_pmvdev.std*1.96;
stat_pmvdev.low95 = stat_pmvdev.mean - stat_pmvdev.std*1.96;

stat_pmvpercent = table();
stat_pmvpercent.mean = mean(pmv.percent);
stat_pmvpercent.std = std(pmv.percent);
stat_pmvpercent.up95 = stat_pmvpercent.mean + stat_pmvpercent.std*1.96;
stat_pmvpercent.low95 = stat_pmvpercent.mean - stat_pmvpercent.std*1.96;



figure
hold on
plot(pmv.datetime, pmv.PMV, '*');
plot(pmv.datetime, pmv.predicted_PMV, '*');
ylabel('PMV')
xlabel('Date')
title('Real vs Predicted PMV')
xlim([start_plot, stop_plot])
ylim([-0.3 0.5])
yEnpr_mean = yline(stat_pmv.mean,'--k','Real Mean');
yEnpr_up95 = yline(stat_pmv.up95,'--b','Real 95% Upper');
yEnpr_low95 = yline(stat_pmv.low95,'--b','Real 95% Lower');
yEnpp_mean = yline(stat_pmvP.mean,'--k','Predicted Mean');
yEnpp_up95 = yline(stat_pmvP.up95,'--r','Predicted 95% Upper');
yEnpp_low95 = yline(stat_pmvP.low95,'--r','Predicted 95% Lower');
yEnpp_mean.LabelVerticalAlignment = 'bottom';
yEnpp_up95.LabelVerticalAlignment = 'bottom';
yEnpp_low95.LabelVerticalAlignment = 'bottom';
legend({'Real', 'Predicted'})
grid
saveas(gcf,'Figures/EVAL-Real vs Predicted PMV.fig')
saveas(gcf,'Figures/EVAL-Real vs Predicted PMV.png')

figure
hold on
plot(pmv.datetime, pmv.deviation, '*r');
ylabel('Deviation')
xlabel('Date')
title('Deviation of Predicted PMV')
xlim([start_plot, stop_plot])
%ylim([-0.015 0.015])
yEnDp_mean = yline(stat_pmvdev.mean,'--k','Mean');
yEnDp_up95 = yline(stat_pmvdev.up95,'--b','95% Upper');
yEnDp_low95 = yline(stat_pmvdev.low95,'--b','95% Lower');
grid
saveas(gcf,'Figures/EVAL-Deviation of Predicted PMV.fig')
saveas(gcf,'Figures/EVAL-Deviation of Predicted PMV.png')

figure
hold on
plot(pmv.datetime, pmv.percent, '*k');
xlim([start_plot, stop_plot])
ylabel('Percentage (%)')
xlabel('Date')
title('Percentage Deviation of Predicted PMV')
xlim([start_plot, stop_plot])
ylim([-300 300])
yEnTp_mean = yline(stat_pmvpercent.mean,'--r','Mean');
yEnTp_up95 = yline(stat_pmvpercent.up95,'--b','95% Upper');
yEnTp_low95 = yline(stat_pmvpercent.low95,'--b','95% Lower');
grid
saveas(gcf,'Figures/EVAL-Percentage Deviation of Predicted PMV.fig')
saveas(gcf,'Figures/EVAL-Percentage Deviation of Predicted PMV.png')