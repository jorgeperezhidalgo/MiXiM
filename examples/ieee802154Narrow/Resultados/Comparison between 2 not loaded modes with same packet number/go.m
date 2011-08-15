clc; clear;

AN_Packets_with_ACK  = vec2mat(csvread('graphic1.csv', 1, 3) , 25)';
AN_Erased_Packets  = csvread('graphic2.csv', 1, 3);
MN_BackOff_and_ACK = csvread('graphic3.csv', 1, 3);
Avg_Backoff = csvread('graphic4.csv',1,3);
Energy = csvread('graphic5.csv',1,3);
KindOfReport = csvread('graphic6.csv',1,3);
 
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

eConf1M1 = eConf1(1:15,:);
eConf1M2 = eConf1(16:30,:);
eConf1M3 = eConf1(31:45,:);
eConf1M4 = eConf1(46:60,:);

eConf2M1 = eConf2(1:15,:);
eConf2M2 = eConf2(16:30,:);
eConf2M3 = eConf2(31:45,:);
eConf2M4 = eConf2(46:60,:);

e_mean_M1 = [mean(eConf1M1)' mean(eConf2M1)'];
e_mean_M2 = [mean(eConf1M2)' mean(eConf2M2)'];
e_mean_M3 = [mean(eConf1M3)' mean(eConf2M3)'];
e_mean_M4 = [mean(eConf1M4)' mean(eConf2M4)'];
 
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

fConf1M1 = fConf1(1:15,:);
fConf1M2 = fConf1(16:30,:);
fConf1M3 = fConf1(31:45,:);
fConf1M4 = fConf1(46:60,:);

fConf2M1 = fConf2(1:15,:);
fConf2M2 = fConf2(16:30,:);
fConf2M3 = fConf2(31:45,:);
fConf2M4 = fConf2(46:60,:);

f_mean_M1 = [mean(fConf1M1)' mean(fConf2M1)'];
f_mean_M2 = [mean(fConf1M2)' mean(fConf2M2)'];
f_mean_M3 = [mean(fConf1M3)' mean(fConf2M3)'];
f_mean_M4 = [mean(fConf1M4)' mean(fConf2M4)'];
 
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

gConf1AN = gConf1(1:25,:);
gConf1M1 = gConf1(26:40,:);
gConf1M2 = gConf1(41:55,:);
gConf1M3 = gConf1(56:70,:);
gConf1M4 = gConf1(71:85,:);

gConf2AN = gConf2(1:25,:);
gConf2M1 = gConf2(26:40,:);
gConf2M2 = gConf2(41:55,:);
gConf2M3 = gConf2(56:70,:);
gConf2M4 = gConf2(71:85,:);

g_mean_AN = [mean(gConf1AN)' mean(gConf2AN)'];
g_mean_M1 = [mean(gConf1M1)' mean(gConf2M1)'];
g_mean_M2 = [mean(gConf1M2)' mean(gConf2M2)'];
g_mean_M3 = [mean(gConf1M3)' mean(gConf2M3)'];
g_mean_M4 = [mean(gConf1M4)' mean(gConf2M4)'];
 
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


% Energy consumption per mobile node
h = vec2mat(Energy, 6000)'; % energy

hConf1 = vec2mat(h(:,1), 60)'; % each column has each 100 repetitions
hConf2 = vec2mat(h(:,2), 60)'; % each column has each 100 repetitions

hConf1M1 = hConf1(1:15,:);
hConf1M2 = hConf1(16:30,:);
hConf1M3 = hConf1(31:45,:);
hConf1M4 = hConf1(46:60,:);

hConf2M1 = hConf2(1:15,:);
hConf2M2 = hConf2(16:30,:);
hConf2M3 = hConf2(31:45,:);
hConf2M4 = hConf2(46:60,:);

h_mean_M1 = [mean(hConf1M1)' mean(hConf2M1)'];
h_mean_M2 = [mean(hConf1M2)' mean(hConf2M2)'];
h_mean_M3 = [mean(hConf1M3)' mean(hConf2M3)'];
h_mean_M4 = [mean(hConf1M4)' mean(hConf2M4)'];
 
h_mean_mean_M1 = mean(h_mean_M1);
h_mean_mean_M2 = mean(h_mean_M2);
h_mean_mean_M3 = mean(h_mean_M3);
h_mean_mean_M4 = mean(h_mean_M4);
h_desv_mean_M1 = std(h_mean_M1);
h_desv_mean_M2 = std(h_mean_M2);
h_desv_mean_M3 = std(h_mean_M3);
h_desv_mean_M4 = std(h_mean_M4);
h_ci_mean_M1 = h_desv_mean_M1/sqrt(100)*norminv(.05/2);
h_ci_mean_M2 = h_desv_mean_M2/sqrt(100)*norminv(.05/2);
h_ci_mean_M3 = h_desv_mean_M3/sqrt(100)*norminv(.05/2);
h_ci_mean_M4 = h_desv_mean_M4/sqrt(100)*norminv(.05/2);

% Number of reports sent by AN depending of origin
j = KindOfReport(:,1);
k = KindOfReport(:,2);
l = KindOfReport(:,3);
m = KindOfReport(:,4);
jj = vec2mat(j, 25)';
kk = vec2mat(k, 25)';
ll = vec2mat(l, 25)';
mm = vec2mat(m, 25)';
mean_j = vec2mat(mean(jj), 100)';
mean_mean_j = mean(mean_j);
desv_j = std(mean_j);
ci_j = desv_j/sqrt(100)*norminv(.05/2);
mean_k = vec2mat(mean(kk), 100)';
mean_mean_k = mean(mean_k);
desv_k = std(mean_k);
ci_k = desv_k/sqrt(100)*norminv(.05/2);
mean_l = vec2mat(mean(ll), 100)';
mean_mean_l = mean(mean_l);
desv_l = std(mean_l);
ci_l = desv_l/sqrt(100)*norminv(.05/2);
mean_m = vec2mat(mean(mm), 100)';
mean_mean_m = mean(mean_m);
desv_m = std(mean_m);
ci_m = desv_m/sqrt(100)*norminv(.05/2);




%%%%%%%%%%%%
% PLOTTING %
%%%%%%%%%%%%

figure(1);
bar([mean_all_iterations_all_AN_Packets_with_ACK' mean_mean_c' mean_mean_d'],'hist');
hold on
errorbar([0.78 1.78],mean_all_iterations_all_AN_Packets_with_ACK, ci_mean_all_iterations_all_AN_Packets_with_ACK,'.k');
errorbar(mean_mean_c, ci_c,'.k');
errorbar([1.22 2.22],mean_mean_d, ci_d,'.b');
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
errorbar([0.7275 1.7275],e_mean_mean_M1, e_ci_mean_M1,'.k');
errorbar([0.91 1.91],e_mean_mean_M2, e_ci_mean_M2,'.k');
errorbar([1.09 2.09],e_mean_mean_M3, e_ci_mean_M3,'.k');
errorbar([1.2725 2.2725],e_mean_mean_M4, e_ci_mean_M4,'.k');
colormap summer

figure(3);
bar([f_mean_mean_M1' f_mean_mean_M2' f_mean_mean_M3' f_mean_mean_M4'],'hist');
hold on
errorbar([0.7275 1.7275],f_mean_mean_M1, f_ci_mean_M1,'.k');
errorbar([0.91 1.91],f_mean_mean_M2, f_ci_mean_M2,'.k');
errorbar([1.09 2.09],f_mean_mean_M3, f_ci_mean_M3,'.k');
errorbar([1.2725 2.2725],f_mean_mean_M4, f_ci_mean_M4,'.k');
colormap summer


figure(4);
bar([g_mean_mean_M1' g_mean_mean_M2' g_mean_mean_M3' g_mean_mean_M4'],'hist');
hold on
errorbar([0.7275 1.7275],g_mean_mean_M1, g_ci_mean_M1,'.k');
errorbar([0.91 1.91],g_mean_mean_M2, g_ci_mean_M2,'.k');
errorbar([1.09 2.09],g_mean_mean_M3, g_ci_mean_M3,'.k');
errorbar([1.2725 2.2725],g_mean_mean_M4, g_ci_mean_M4,'.k');
colormap summer


figure(5);
bar([h_mean_mean_M1' h_mean_mean_M2' h_mean_mean_M3' h_mean_mean_M4'],'hist');
hold on
errorbar([0.7275 1.7275],h_mean_mean_M1, h_ci_mean_M1,'.k');
errorbar([0.91 1.91],h_mean_mean_M2, h_ci_mean_M2,'.k');
errorbar([1.09 2.09],h_mean_mean_M3, h_ci_mean_M3,'.k');
errorbar([1.2725 2.2725],h_mean_mean_M4, h_ci_mean_M4,'.k');
colormap summer

figure(6);
bar([mean_mean_j' mean_mean_k' mean_mean_l' mean_mean_m'],'hist');
hold on
errorbar([0.7275 1.7275],mean_mean_j, ci_j,'.k');
errorbar([0.91 1.91],mean_mean_k, ci_k,'.k');
errorbar([1.09 2.09],mean_mean_l, ci_l,'.k');
errorbar([1.2725 2.2725],mean_mean_m, ci_m, '.k');
colormap summer
