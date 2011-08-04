clc; clear;

AN_Packets_with_ACK  = vec2mat(csvread('graphic1.csv', 1, 3) , 25)';
AN_Erased_Packets  = csvread('graphic2.csv', 1, 3);
MN_BackOff_and_ACK = csvread('graphic3.csv', 1, 3);
Avg_Backoff = csvread('graphic4.csv',1,3);
 
% Number of total recived ACKs (total received packets)
mean_all_AN_Packets_with_ACK = vec2mat(mean(AN_Packets_with_ACK) , 100)';
mean_all_iterations_all_AN_Packets_with_ACK = mean(mean_all_AN_Packets_with_ACK);
desv_mean_all_iterations_all_AN_Packets_with_ACK = std(mean_all_AN_Packets_with_ACK);
ci_mean_all_iterations_all_AN_Packets_with_ACK = desv_mean_all_iterations_all_AN_Packets_with_ACK/sqrt(100)*norminv(.05/2);

% Number of total erased reports due to maximum retransmission tries
% (BackOff + No ACK received)
a = AN_Erased_Packets(:,1)+AN_Erased_Packets(:,2)+AN_Erased_Packets(:,3);
c = vec2mat(a, 25)';
mean_c = vec2mat(mean(c), 100)';
mean_mean_c = mean(mean_c);
desv_c = std(mean_c);
ci_c = desv_c/sqrt(100)*norminv(.05/2);

% Number of total discarted reports at the end of phases due to lack of
% time
b = AN_Erased_Packets(:,4)+AN_Erased_Packets(:,5);
d = vec2mat(b, 25)';
mean_d = vec2mat(mean(d), 100)';
mean_mean_d = mean(mean_d);
desv_d = std(mean_d);
ci_d = desv_d/sqrt(100)*norminv(.05/2);

% Number of missed ACKs in MNs and number Backoffs in MN
e = MN_BackOff_and_ACK(:,1); % nbBackoffs
f = MN_BackOff_and_ACK(:,2); % nbMissedAcks
e1 = vec2mat(e, 60)';
f1 = vec2mat(f, 60)';
mean_e1 = vec2mat(mean(e1), 100)';
mean_f1 = vec2mat(mean(f1), 100)';
mean_mean_e1 = mean(mean_e1);
mean_mean_f1 = mean(mean_f1);
desv_mean_e1 = std(mean_e1);
desv_mean_f1 = std(mean_f1);
ci_mean_e1 = desv_mean_e1/sqrt(100)*norminv(.05/2);
ci_mean_f1 = desv_mean_f1/sqrt(100)*norminv(.05/2);

% Average Backoff waiting time
g = vec2mat(Avg_Backoff, 8500)'; %each column has each configuration
gConf1 = vec2mat(g(:,1), 85)'; % each column has each 100 repetitions
gConf2 = vec2mat(g(:,2), 85)'; % each column has each 100 repetitions
gConf3 = vec2mat(g(:,3), 85)'; % each column has each 100 repetitions
gConf4 = vec2mat(g(:,4), 85)'; % each column has each 100 repetitions
gConf5 = vec2mat(g(:,5), 85)'; % each column has each 100 repetitions

gConf1AN = gConf1(1:25,:);
gConf1M1 = gConf1(26:40,:);
gConf1M2 = gConf1(41:55,:);
gConf1M3 = gConf1(56:70,:);
gConf1M4 = gConf1(71:85,:);

gConf2AN = gConf2(1:25,:);
gConf2M1 = gConf2(26:31,:);
gConf2M2 = gConf2(32:37,:);
gConf2M3 = gConf2(38:52,:);
gConf2M4 = gConf2(53:85,:);

gConf3AN = gConf3(1:25,:);
gConf3M1 = gConf3(26:31,:);
gConf3M2 = gConf3(32:37,:);
gConf3M3 = gConf3(53:85,:);
gConf3M4 = gConf3(38:52,:);

gConf4AN = gConf4(1:25,:);
gConf4M1 = gConf4(53:85,:);
gConf4M2 = gConf4(38:52,:);
gConf4M3 = gConf4(26:31,:);
gConf4M4 = gConf4(32:37,:);

gConf5AN = gConf5(1:25,:);
gConf5M1 = gConf5(38:52,:);
gConf5M2 = gConf5(53:85,:);
gConf5M3 = gConf5(26:31,:);
gConf5M4 = gConf5(32:37,:);

mean_AN = [mean(gConf1AN)' mean(gConf2AN)' mean(gConf3AN)' mean(gConf4AN)' mean(gConf5AN)'];
mean_M1 = [mean(gConf1M1)' mean(gConf2M1)' mean(gConf3M1)' mean(gConf4M1)' mean(gConf5M1)'];
mean_M2 = [mean(gConf1M2)' mean(gConf2M2)' mean(gConf3M2)' mean(gConf4M2)' mean(gConf5M2)'];
mean_M3 = [mean(gConf1M3)' mean(gConf2M3)' mean(gConf3M3)' mean(gConf4M3)' mean(gConf5M3)'];
mean_M4 = [mean(gConf1M4)' mean(gConf2M4)' mean(gConf3M4)' mean(gConf4M4)' mean(gConf5M4)'];

mean_mean_AN = mean(mean_AN);
mean_mean_M1 = mean(mean_M1);
mean_mean_M2 = mean(mean_M2);
mean_mean_M3 = mean(mean_M3);
mean_mean_M4 = mean(mean_M4);
desv_mean_AN = std(mean_AN);
desv_mean_M1 = std(mean_M1);
desv_mean_M2 = std(mean_M2);
desv_mean_M3 = std(mean_M3);
desv_mean_M4 = std(mean_M4);
ci_mean_AN = desv_mean_AN/sqrt(100)*norminv(.05/2);
ci_mean_M1 = desv_mean_M1/sqrt(100)*norminv(.05/2);
ci_mean_M2 = desv_mean_M2/sqrt(100)*norminv(.05/2);
ci_mean_M3 = desv_mean_M3/sqrt(100)*norminv(.05/2);
ci_mean_M4 = desv_mean_M4/sqrt(100)*norminv(.05/2);


%%%%%%%%%%%%
% PLOTTING %
%%%%%%%%%%%%

figure(1);
bar([mean_all_iterations_all_AN_Packets_with_ACK' mean_mean_c' mean_mean_d'],'hist');
hold on
errorbar([0.78 1.78 2.78 3.78 4.78],mean_all_iterations_all_AN_Packets_with_ACK, ci_mean_all_iterations_all_AN_Packets_with_ACK,'.k');
errorbar(mean_mean_c, ci_c,'.k');
errorbar([1.22 2.22 3.22 4.22 5.22],mean_mean_d, ci_d,'.b');
colormap summer

figure(2);
bar([mean_mean_e1' mean_mean_f1'],'hist');
hold on
errorbar([0.86 1.86 2.86 3.86 4.86],mean_mean_e1, ci_mean_e1,'.k');
errorbar([1.14 2.14 3.14 4.14 5.14],mean_mean_f1, ci_mean_f1,'.b');
colormap summer

figure(3);
bar([mean_mean_AN' mean_mean_M1' mean_mean_M2' mean_mean_M3' mean_mean_M4'],'hist');
hold on
errorbar([0.69 1.69 2.69 3.69 4.69],mean_mean_AN, ci_mean_AN,'.k');
errorbar([0.845 1.845 2.845 3.845 4.845],mean_mean_M1, ci_mean_M1,'.k');
errorbar(mean_mean_M2, ci_mean_M2,'.k');
errorbar([1.155 2.155 3.155 4.155 5.155],mean_mean_M3, ci_mean_M3,'.k');
errorbar([1.31 2.31 3.31 4.31 5.31],mean_mean_M4, ci_mean_M4,'.k');
colormap summer
