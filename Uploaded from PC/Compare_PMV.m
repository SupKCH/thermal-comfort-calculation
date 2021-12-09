%% PMV
clear;
close all;
clc;

air_data = readtable('finalform/exp_real_and_predicted_pmv.csv');
air_data.datetime = datetime(air_data.year, air_data.month, air_data.day, air_data.hour, air_data.minute, 0);

toDelete = air_data.PMV < -0.8;
air_data(toDelete,:) = [];

PMV_opt = table();
PMV_opt.mean = mean(air_data.PMV);
PMV_opt.std = std(air_data.PMV);
PMV_opt.stdError = (PMV_opt.std)./sqrt(height(air_data));
PMV_opt.max = max(air_data.PMV);
PMV_opt.min = min(air_data.PMV);
PMV_opt.up95 = PMV_opt.mean + PMV_opt.std*1.96;
PMV_opt.low95 = PMV_opt.mean - PMV_opt.std*1.96;

PMV_prd = table();
PMV_prd.mean = mean(air_data.predicted_PMV);
PMV_prd.std = std(air_data.predicted_PMV);
PMV_prd.stdError = (PMV_prd.std)./sqrt(height(air_data));
PMV_prd.max = max(air_data.predicted_PMV);
PMV_prd.min = min(air_data.predicted_PMV);
PMV_prd.up95 = PMV_prd.mean + PMV_prd.std*1.96;
PMV_prd.low95 = PMV_prd.mean - PMV_prd.std*1.96;

figure
hold on
plot(air_data.datetime, air_data.PMV, '*')

plot(air_data.datetime, air_data.predicted_PMV, '*')
pmvUp = yline(0.5,'-r','PMV +0.5');
pmvLow = yline(-0.5,'-r','PMV -0.5');

%pmvUp.LabelHorizontalAlignment = 'center';
%pmvUp.LabelVerticalAlignment = 'bottom';
%pmvLow.LabelHorizontalAlignment = 'center';
%pmvLow.LabelVerticalAlignment = 'bottom';

yOpt_mean = yline(PMV_opt.mean,'--k','OPT Mean');
yPrd_mean = yline(PMV_prd.mean,'--k','PRD Mean');
yOpt_up95 = yline(PMV_opt.up95,'--b','OPT 95% Upper');
yOpt_low95 = yline(PMV_opt.low95,'--b','OPT 95% Lower');
yPrd_up95 = yline(PMV_prd.up95,'--b','PRD 95% Upper');
yPrd_low95 = yline(PMV_prd.low95,'--b','PRD 95% Lower');

%yOpt_mean.LabelHorizontalAlignment = 'center';
yOpt_mean.LabelVerticalAlignment = 'bottom';
%yOpt_up95.LabelHorizontalAlignment = 'center';
yOpt_up95.LabelVerticalAlignment = 'bottom';
%yOpt_low95.LabelHorizontalAlignment = 'center';
yOpt_low95.LabelVerticalAlignment = 'bottom';

%yPrd_mean.LabelHorizontalAlignment = 'center';
%yPrd_mean.LabelVerticalAlignment = 'bottom';
%yPrd_up95.LabelHorizontalAlignment = 'center';
%yPrd_up95.LabelVerticalAlignment = 'bottom';
%yPrd_low95.LabelHorizontalAlignment = 'center';
%yPrd_low95.LabelVerticalAlignment = 'bottom';

ylabel('PMV')
xlabel('Experiment Date')
legend({'Optimized PMV', 'Predicted Normal PMV'})
title('Opimized PMV vs Predicted normal usage PMV')
grid

saveas(gcf,'Figures/PMV-Opimized PMV vs Predicted normal usage PMV.fig')
saveas(gcf,'Figures/PMV-Opimized PMV vs Predicted normal usage PMV.png')
