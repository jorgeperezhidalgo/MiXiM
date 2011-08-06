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
e = vec2mat(MN_BackOff_and_ACK(:,1), 6000)'; % nbBackoffs
f = vec2mat(MN_BackOff_and_ACK(:,2), 6000)'; % nbMissedAcks

eConf1 = vec2mat(e(:,1), 60)'; % each column has each 100 repetitions
eConf2 = vec2mat(e(:,2), 60)'; % each column has each 100 repetitions
eConf3 = vec2mat(e(:,3), 60)'; % each column has each 100 repetitions
eConf4 = vec2mat(e(:,4), 60)'; % each column has each 100 repetitions
eConf5 = vec2mat(e(:,5), 60)'; % each column has each 100 repetitions
eConf6 = vec2mat(e(:,6), 60)'; % each column has each 100 repetitions
eConf7 = vec2mat(e(:,7), 60)'; % each column has each 100 repetitions
eConf8 = vec2mat(e(:,8), 60)'; % each column has each 100 repetitions
eConf9 = vec2mat(e(:,9), 60)'; % each column has each 100 repetitions
eConf10 = vec2mat(e(:,10), 60)'; % each column has each 100 repetitions

eConf1M1 = eConf1(1:15,:);
eConf1M2 = eConf1(16:30,:);
eConf1M3 = eConf1(31:45,:);
eConf1M4 = eConf1(46:60,:);

eConf2M1 = eConf2(1:6,:);
eConf2M2 = eConf2(7:12,:);
eConf2M3 = eConf2(13:27,:);
eConf2M4 = eConf2(28:60,:);

eConf3M1 = eConf3(1:6,:);
eConf3M2 = eConf3(7:12,:);
eConf3M3 = eConf3(28:60,:);
eConf3M4 = eConf3(13:27,:);

eConf4M1 = eConf4(28:60,:);
eConf4M2 = eConf4(13:27,:);
eConf4M3 = eConf4(1:6,:);
eConf4M4 = eConf4(7:12,:);

eConf5M1 = eConf5(13:27,:);
eConf5M2 = eConf5(28:60,:);
eConf5M3 = eConf5(1:6,:);
eConf5M4 = eConf5(7:12,:);

eConf6M1 = eConf6(1:15,:);
eConf6M2 = eConf6(16:30,:);
eConf6M3 = eConf6(31:45,:);
eConf6M4 = eConf6(46:60,:);

eConf7M1 = eConf7(1:6,:);
eConf7M2 = eConf7(7:12,:);
eConf7M3 = eConf7(13:27,:);
eConf7M4 = eConf7(28:60,:);

eConf8M1 = eConf8(1:6,:);
eConf8M2 = eConf8(7:12,:);
eConf8M3 = eConf8(28:60,:);
eConf8M4 = eConf8(13:27,:);

eConf9M1 = eConf9(28:60,:);
eConf9M2 = eConf9(13:27,:);
eConf9M3 = eConf9(1:6,:);
eConf9M4 = eConf9(7:12,:);

eConf10M1 = eConf10(13:27,:);
eConf10M2 = eConf10(28:60,:);
eConf10M3 = eConf10(1:6,:);
eConf10M4 = eConf10(7:12,:);

e_mean_M1 = [mean(eConf1M1)' mean(eConf2M1)' mean(eConf3M1)' mean(eConf4M1)' mean(eConf5M1)' mean(eConf6M1)' mean(eConf7M1)' mean(eConf8M1)' mean(eConf9M1)' mean(eConf10M1)'];
e_mean_M2 = [mean(eConf1M2)' mean(eConf2M2)' mean(eConf3M2)' mean(eConf4M2)' mean(eConf5M2)' mean(eConf6M2)' mean(eConf7M2)' mean(eConf8M2)' mean(eConf9M2)' mean(eConf10M2)'];
e_mean_M3 = [mean(eConf1M3)' mean(eConf2M3)' mean(eConf3M3)' mean(eConf4M3)' mean(eConf5M3)' mean(eConf6M3)' mean(eConf7M3)' mean(eConf8M3)' mean(eConf9M3)' mean(eConf10M3)'];
e_mean_M4 = [mean(eConf1M4)' mean(eConf2M4)' mean(eConf3M4)' mean(eConf4M4)' mean(eConf5M4)' mean(eConf6M4)' mean(eConf7M4)' mean(eConf8M4)' mean(eConf9M4)' mean(eConf10M4)'];
 
e_mean_mean_M1 = mean(e_mean_M1);
e_mean_mean_M2 = mean(e_mean_M2);
e_mean_mean_M3 = mean(e_mean_M3);
e_mean_mean_M4 = mean(e_mean_M4);
e_desv_mean_M1 = std(e_mean_M1);
e_desv_mean_M2 = std(e_mean_M2);
e_desv_mean_M3 = std(e_mean_M3);
e_desv_mean_M4 = std(e_mean_M4);
e_ci_mean_M1 = e_desv_mean_M1/sqrt(100)*norminv(.05/2);
e_ci_mean_M2 = e_desv_mean_M2/sqrt(100)*norminv(.05/2);
e_ci_mean_M3 = e_desv_mean_M3/sqrt(100)*norminv(.05/2);
e_ci_mean_M4 = e_desv_mean_M4/sqrt(100)*norminv(.05/2);

fConf1 = vec2mat(f(:,1), 60)'; % each column has each 100 repetitions
fConf2 = vec2mat(f(:,2), 60)'; % each column has each 100 repetitions
fConf3 = vec2mat(f(:,3), 60)'; % each column has each 100 repetitions
fConf4 = vec2mat(f(:,4), 60)'; % each column has each 100 repetitions
fConf5 = vec2mat(f(:,5), 60)'; % each column has each 100 repetitions
fConf6 = vec2mat(f(:,6), 60)'; % each column has each 100 repetitions
fConf7 = vec2mat(f(:,7), 60)'; % each column has each 100 repetitions
fConf8 = vec2mat(f(:,8), 60)'; % each column has each 100 repetitions
fConf9 = vec2mat(f(:,9), 60)'; % each column has each 100 repetitions
fConf10 = vec2mat(f(:,10), 60)'; % each column has each 100 repetitions

fConf1M1 = fConf1(1:15,:);
fConf1M2 = fConf1(16:30,:);
fConf1M3 = fConf1(31:45,:);
fConf1M4 = fConf1(46:60,:);

fConf2M1 = fConf2(1:6,:);
fConf2M2 = fConf2(7:12,:);
fConf2M3 = fConf2(13:27,:);
fConf2M4 = fConf2(28:60,:);

fConf3M1 = fConf3(1:6,:);
fConf3M2 = fConf3(7:12,:);
fConf3M3 = fConf3(28:60,:);
fConf3M4 = fConf3(13:27,:);

fConf4M1 = fConf4(28:60,:);
fConf4M2 = fConf4(13:27,:);
fConf4M3 = fConf4(1:6,:);
fConf4M4 = fConf4(7:12,:);

fConf5M1 = fConf5(13:27,:);
fConf5M2 = fConf5(28:60,:);
fConf5M3 = fConf5(1:6,:);
fConf5M4 = fConf5(7:12,:);

fConf6M1 = fConf6(1:15,:);
fConf6M2 = fConf6(16:30,:);
fConf6M3 = fConf6(31:45,:);
fConf6M4 = fConf6(46:60,:);

fConf7M1 = fConf7(1:6,:);
fConf7M2 = fConf7(7:12,:);
fConf7M3 = fConf7(13:27,:);
fConf7M4 = fConf7(28:60,:);

fConf8M1 = fConf8(1:6,:);
fConf8M2 = fConf8(7:12,:);
fConf8M3 = fConf8(28:60,:);
fConf8M4 = fConf8(13:27,:);

fConf9M1 = fConf9(28:60,:);
fConf9M2 = fConf9(13:27,:);
fConf9M3 = fConf9(1:6,:);
fConf9M4 = fConf9(7:12,:);

fConf10M1 = fConf10(13:27,:);
fConf10M2 = fConf10(28:60,:);
fConf10M3 = fConf10(1:6,:);
fConf10M4 = fConf10(7:12,:);

f_mean_M1 = [mean(fConf1M1)' mean(fConf2M1)' mean(fConf3M1)' mean(fConf4M1)' mean(fConf5M1)' mean(fConf6M1)' mean(fConf7M1)' mean(fConf8M1)' mean(fConf9M1)' mean(fConf10M1)'];
f_mean_M2 = [mean(fConf1M2)' mean(fConf2M2)' mean(fConf3M2)' mean(fConf4M2)' mean(fConf5M2)' mean(fConf6M2)' mean(fConf7M2)' mean(fConf8M2)' mean(fConf9M2)' mean(fConf10M2)'];
f_mean_M3 = [mean(fConf1M3)' mean(fConf2M3)' mean(fConf3M3)' mean(fConf4M3)' mean(fConf5M3)' mean(fConf6M3)' mean(fConf7M3)' mean(fConf8M3)' mean(fConf9M3)' mean(fConf10M3)'];
f_mean_M4 = [mean(fConf1M4)' mean(fConf2M4)' mean(fConf3M4)' mean(fConf4M4)' mean(fConf5M4)' mean(fConf6M4)' mean(fConf7M4)' mean(fConf8M4)' mean(fConf9M4)' mean(fConf10M4)'];
 
f_mean_mean_M1 = mean(f_mean_M1);
f_mean_mean_M2 = mean(f_mean_M2);
f_mean_mean_M3 = mean(f_mean_M3);
f_mean_mean_M4 = mean(f_mean_M4);
f_desv_mean_M1 = std(f_mean_M1);
f_desv_mean_M2 = std(f_mean_M2);
f_desv_mean_M3 = std(f_mean_M3);
f_desv_mean_M4 = std(f_mean_M4);
f_ci_mean_M1 = f_desv_mean_M1/sqrt(100)*norminv(.05/2);
f_ci_mean_M2 = f_desv_mean_M2/sqrt(100)*norminv(.05/2);
f_ci_mean_M3 = f_desv_mean_M3/sqrt(100)*norminv(.05/2);
f_ci_mean_M4 = f_desv_mean_M4/sqrt(100)*norminv(.05/2);



% e1 = vec2mat(e, 60)';
% f1 = vec2mat(f, 60)';
% mean_e1 = vec2mat(mean(e1), 100)';
% mean_f1 = vec2mat(mean(f1), 100)';
% mean_mean_e1 = mean(mean_e1);
% mean_mean_f1 = mean(mean_f1);
% desv_mean_e1 = std(mean_e1);
% desv_mean_f1 = std(mean_f1);
% ci_mean_e1 = desv_mean_e1/sqrt(100)*norminv(.05/2);
% ci_mean_f1 = desv_mean_f1/sqrt(100)*norminv(.05/2);
 
% Average Backoff waiting time
g = vec2mat(Avg_Backoff, 8500)'; %each column has each configuration
gConf1 = vec2mat(g(:,1), 85)'; % each column has each 100 repetitions
gConf2 = vec2mat(g(:,2), 85)'; % each column has each 100 repetitions
gConf3 = vec2mat(g(:,3), 85)'; % each column has each 100 repetitions
gConf4 = vec2mat(g(:,4), 85)'; % each column has each 100 repetitions
gConf5 = vec2mat(g(:,5), 85)'; % each column has each 100 repetitions
gConf6 = vec2mat(g(:,6), 85)'; % each column has each 100 repetitions
gConf7 = vec2mat(g(:,7), 85)'; % each column has each 100 repetitions
gConf8 = vec2mat(g(:,8), 85)'; % each column has each 100 repetitions
gConf9 = vec2mat(g(:,9), 85)'; % each column has each 100 repetitions
gConf10 = vec2mat(g(:,10), 85)'; % each column has each 100 repetitions

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

gConf6AN = gConf6(1:25,:);
gConf6M1 = gConf6(26:40,:);
gConf6M2 = gConf6(41:55,:);
gConf6M3 = gConf6(56:70,:);
gConf6M4 = gConf6(71:85,:);

gConf7AN = gConf7(1:25,:);
gConf7M1 = gConf7(26:31,:);
gConf7M2 = gConf7(32:37,:);
gConf7M3 = gConf7(38:52,:);
gConf7M4 = gConf7(53:85,:);

gConf8AN = gConf8(1:25,:);
gConf8M1 = gConf8(26:31,:);
gConf8M2 = gConf8(32:37,:);
gConf8M3 = gConf8(53:85,:);
gConf8M4 = gConf8(38:52,:);

gConf9AN = gConf9(1:25,:);
gConf9M1 = gConf9(53:85,:);
gConf9M2 = gConf9(38:52,:);
gConf9M3 = gConf9(26:31,:);
gConf9M4 = gConf9(32:37,:);

gConf10AN = gConf10(1:25,:);
gConf10M1 = gConf10(38:52,:);
gConf10M2 = gConf10(53:85,:);
gConf10M3 = gConf10(26:31,:);
gConf10M4 = gConf10(32:37,:);

g_mean_AN = [mean(gConf1AN)' mean(gConf2AN)' mean(gConf3AN)' mean(gConf4AN)' mean(gConf5AN)' mean(gConf6AN)' mean(gConf7AN)' mean(gConf8AN)' mean(gConf9AN)' mean(gConf10AN)'];
g_mean_M1 = [mean(gConf1M1)' mean(gConf2M1)' mean(gConf3M1)' mean(gConf4M1)' mean(gConf5M1)' mean(gConf6M1)' mean(gConf7M1)' mean(gConf8M1)' mean(gConf9M1)' mean(gConf10M1)'];
g_mean_M2 = [mean(gConf1M2)' mean(gConf2M2)' mean(gConf3M2)' mean(gConf4M2)' mean(gConf5M2)' mean(gConf6M2)' mean(gConf7M2)' mean(gConf8M2)' mean(gConf9M2)' mean(gConf10M2)'];
g_mean_M3 = [mean(gConf1M3)' mean(gConf2M3)' mean(gConf3M3)' mean(gConf4M3)' mean(gConf5M3)' mean(gConf6M3)' mean(gConf7M3)' mean(gConf8M3)' mean(gConf9M3)' mean(gConf10M3)'];
g_mean_M4 = [mean(gConf1M4)' mean(gConf2M4)' mean(gConf3M4)' mean(gConf4M4)' mean(gConf5M4)' mean(gConf6M4)' mean(gConf7M4)' mean(gConf8M4)' mean(gConf9M4)' mean(gConf10M4)'];
 
g_mean_mean_AN = mean(g_mean_AN);
g_mean_mean_M1 = mean(g_mean_M1);
g_mean_mean_M2 = mean(g_mean_M2);
g_mean_mean_M3 = mean(g_mean_M3);
g_mean_mean_M4 = mean(g_mean_M4);
g_desv_mean_AN = std(g_mean_AN);
g_desv_mean_M1 = std(g_mean_M1);
g_desv_mean_M2 = std(g_mean_M2);
g_desv_mean_M3 = std(g_mean_M3);
g_desv_mean_M4 = std(g_mean_M4);
g_ci_mean_AN = g_desv_mean_AN/sqrt(100)*norminv(.05/2);
g_ci_mean_M1 = g_desv_mean_M1/sqrt(100)*norminv(.05/2);
g_ci_mean_M2 = g_desv_mean_M2/sqrt(100)*norminv(.05/2);
g_ci_mean_M3 = g_desv_mean_M3/sqrt(100)*norminv(.05/2);
g_ci_mean_M4 = g_desv_mean_M4/sqrt(100)*norminv(.05/2);


%%%%%%%%%%%%
% PLOTTING %
%%%%%%%%%%%%

figure(1);
bar([mean_all_iterations_all_AN_Packets_with_ACK' mean_mean_c' mean_mean_d'],'hist');
hold on
errorbar([0.78 1.78 2.78 3.78 4.78 5.78 6.78 7.78 8.78 9.78],mean_all_iterations_all_AN_Packets_with_ACK, ci_mean_all_iterations_all_AN_Packets_with_ACK,'.k');
errorbar(mean_mean_c, ci_c,'.k');
errorbar([1.22 2.22 3.22 4.22 5.22 6.22 7.22 8.22 9.22 10.22],mean_mean_d, ci_d,'.b');
colormap summer

% figure(2);
% bar([mean_mean_e1' mean_mean_f1'],'hist');
% hold on
% errorbar([0.86 1.86 2.86 3.86 4.86 5.86 6.86 7.86 8.86 9.86],mean_mean_e1, ci_mean_e1,'.k');
% errorbar([1.14 2.14 3.14 4.14 5.14 6.14 7.14 8.14 9.14 10.14],mean_mean_f1, ci_mean_f1,'.b');
% colormap summer

figure(2);
bar([e_mean_mean_M1' e_mean_mean_M2' e_mean_mean_M3' e_mean_mean_M4'],'hist');
hold on
errorbar([0.7275 1.7275 2.7275 3.7275 4.7275 5.7275 6.7275 7.7275 8.7275 9.7275],e_mean_mean_M1, e_ci_mean_M1,'.k');
errorbar([0.91 1.91 2.91 3.91 4.91 5.91 6.91 7.91 8.91 9.91],e_mean_mean_M2, e_ci_mean_M2,'.k');
errorbar([1.09 2.09 3.09 4.09 5.09 6.09 7.09 8.09 9.09 10.09],e_mean_mean_M3, e_ci_mean_M3,'.k');
errorbar([1.2725 2.2725 3.2725 4.2725 5.2725 6.2725 7.2725 8.2725 9.2725 10.2725],e_mean_mean_M4, e_ci_mean_M4,'.k');
colormap summer

figure(3);
bar([f_mean_mean_M1' f_mean_mean_M2' f_mean_mean_M3' f_mean_mean_M4'],'hist');
hold on
errorbar([0.7275 1.7275 2.7275 3.7275 4.7275 5.7275 6.7275 7.7275 8.7275 9.7275],f_mean_mean_M1, f_ci_mean_M1,'.k');
errorbar([0.91 1.91 2.91 3.91 4.91 5.91 6.91 7.91 8.91 9.91],f_mean_mean_M2, f_ci_mean_M2,'.k');
errorbar([1.09 2.09 3.09 4.09 5.09 6.09 7.09 8.09 9.09 10.09],f_mean_mean_M3, f_ci_mean_M3,'.k');
errorbar([1.2725 2.2725 3.2725 4.2725 5.2725 6.2725 7.2725 8.2725 9.2725 10.2725],f_mean_mean_M4, f_ci_mean_M4,'.k');
colormap summer


figure(4);
bar([g_mean_mean_AN' g_mean_mean_M1' g_mean_mean_M2' g_mean_mean_M3' g_mean_mean_M4'],'hist');
hold on
errorbar([0.69 1.69 2.69 3.69 4.69 5.69 6.69 7.69 8.69 9.69],g_mean_mean_AN, g_ci_mean_AN,'.k');
errorbar([0.845 1.845 2.845 3.845 4.845 5.845 6.845 7.845 8.845 9.845],g_mean_mean_M1, g_ci_mean_M1,'.k');
errorbar(g_mean_mean_M2, g_ci_mean_M2,'.k');
errorbar([1.155 2.155 3.155 4.155 5.155 6.155 7.155 8.155 9.155 10.155],g_mean_mean_M3, g_ci_mean_M3,'.k');
errorbar([1.31 2.31 3.31 4.31 5.31 6.31 7.31 8.31 9.31 10.31],g_mean_mean_M4, g_ci_mean_M4,'.k');
colormap summer
