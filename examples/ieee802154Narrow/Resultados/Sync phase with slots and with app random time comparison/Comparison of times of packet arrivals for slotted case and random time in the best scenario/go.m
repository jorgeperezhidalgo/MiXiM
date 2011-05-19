clc; clear;

fila_vector = 1; % Row where the data begins, starts with 0
columna_vector = 0; % Column where the data begins, starts with 0

%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Variables for file name %
%%%%%%%%%%%%%%%%%%%%%%%%%%%
num = 0;
N = 30:10:70;
T = 5:2:13;
P = 10;

for nodes = 1:5
       
    num = num + 1;
    tiempo = 0;

%%%%%%%%%%%%%%%%%%%
% Vector Analysis %
%%%%%%%%%%%%%%%%%%%

    %data_tiempo = csvread(strcat('vector-N=', num2str(N(nodes)), '-P=', num2str(P), '-T=', num2str(T(nodes)), '.csv'), fila_vector, columna_vector);
    data_tiempo = csvread(strcat('vector-N=', num2str(N(nodes)), '.csv'), fila_vector, columna_vector);

    tiempos(nodes, size(data_tiempo,2)-1, 11) = 0; % 11 because it is the last 1, 2, ... 8 the first, the last and interpacket mean arrival
    numero_paquetes(nodes, size(data_tiempo,2)-1, 8) = 0; % 10 because it is the last 1, 2, ... 8 and the total
    
    for i=2:size(data_tiempo,2)
        num_paq_rec = 0;
        for j=1:size(data_tiempo,1)
            if (data_tiempo(j,i) > 0)
                num_paq_rec = num_paq_rec + 1;
                tiempo(num_paq_rec,1)=data_tiempo(j,1); % Time of packet arrival column
                tiempo(num_paq_rec,2)=data_tiempo(j,i); % Sender AN id column
                tiempo(num_paq_rec,3)=size(find(tiempo(:,2) == data_tiempo(j,i)), 1); % Number of received packet from an AN column
                if (tiempo(num_paq_rec,3) <= 8)
                    numero_paquetes(nodes,i-1,tiempo(num_paq_rec,3)) = numero_paquetes(nodes,i-1,tiempo(num_paq_rec,3)) + 1;
                end
            end
        end
        for k=1:size(tiempo,1)
            if (k >= 2)
                time_between(k) = tiempo(k,1) - tiempo(k-1,1);
            else
                time_between(k) = tiempo(k,1);
            end
            tiempos(nodes, i-1, tiempo(k, 3)) = tiempo(k, 1);
        end
        tiempos(nodes, i-1, 9) = tiempo(1, 1);
        tiempos(nodes, i-1, 10) = tiempo(size(tiempo,1), 1);
        tiempos(nodes, i-1, 11) = mean(time_between);
        time_between = 0;
        tiempo = 0;
    end
end

% Average from the 80 nodes 
for i=1:5
    for j=1:11
        for k=1:100 % Number of iterations
            tiempos_medios_nodos(i, k, j) = mean(tiempos(i, find(tiempos(i, (k-1)*80+1:k*80, j) ~= 0), j), 2);
        end
    end
    for j=1:8
        for k=1:100 % Number of iterations
            numero_medio_paquetes(i, k, j) = mean(numero_paquetes(i, find(numero_paquetes(i, (k-1)*80+1:k*80, j) ~= 0), j), 2);
        end
    end
end

% Average from the 100 repetitions
for i=1:5
    for j=1:11
        media_tiempos(i, j) = mean(tiempos_medios_nodos(i, find(tiempos_medios_nodos(i, :, j) ~= 0), j), 2);
        desv_tiempos(i, j) = std(tiempos_medios_nodos(i, find(tiempos_medios_nodos(i, :, j) ~= 0), j), 0, 2);
    end
    for j=1:8
        media_paquetes(i, j) = mean(numero_medio_paquetes(i, find(numero_medio_paquetes(i, :, j) ~= 0), j), 2);
        desv_paquetes(i, j) = std(numero_medio_paquetes(i, find(numero_medio_paquetes(i, :, j) ~= 0), j), 0, 2);
    end
end

ci_media_tiempos = desv_tiempos/sqrt(size(tiempos_medios_nodos, 2)) * norminv(.05/2);
ci_media_paquetes = desv_paquetes/sqrt(size(numero_medio_paquetes, 2)) * norminv(.05/2);

        
%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Mean and Var calculation %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        
%     temp = t_last_p(:,num);
%     mean_t_last_p(num) = mean(temp(temp ~= 0));
%     desv_t_last_p(num) = std(temp(temp ~= 0));
%     ci_mean_t_last_p(num) = desv_t_last_p(num)/sqrt(1000)*norminv(.05/2);
%     prctile_t_last_p(num) = prctile(temp(temp ~= 0), 98);
% 
%     temp = t_first_p(:,num);
%     mean_t_first_p(num) = mean(temp(temp ~= 0));
%     desv_t_first_p(num) = std(temp(temp ~= 0));
%     ci_mean_t_first_p(num) = desv_t_first_p(num)/sqrt(1000)*norminv(.05/2);
%     prctile_t_first_p(num) = prctile(temp(temp ~= 0), 98);
% 
%     temp = t_last_1_p(:,num);
%     mean_t_last_1_p(num) = mean(temp(temp ~= 0));
%     desv_t_last_1_p(num) = std(temp(temp ~= 0));
%     ci_mean_t_last_1_p(num) = desv_t_last_1_p(num)/sqrt(1000)*norminv(.05/2);
%     prctile_t_last_1_p(num) = prctile(temp(temp ~= 0), 98);
% 
%     temp = t_last_2_p(:,num);
%     mean_t_last_2_p(num) = mean(temp(temp ~= 0));
%     desv_t_last_2_p(num) = std(temp(temp ~= 0));
%     ci_mean_t_last_2_p(num) = desv_t_last_2_p(num)/sqrt(1000)*norminv(.05/2);
%     prctile_t_last_2_p(num) = prctile(temp(temp ~= 0), 98);
% 
%     temp = t_last_3_p(:,num);
%     mean_t_last_3_p(num) = mean(temp(temp ~= 0));
%     desv_t_last_3_p(num) = std(temp(temp ~= 0));
%     ci_mean_t_last_3_p(num) = desv_t_last_3_p(num)/sqrt(1000)*norminv(.05/2);
%     prctile_t_last_3_p(num) = prctile(temp(temp ~= 0), 98);
% 
%     temp = t_mean_between_p(:,num);
%     mean_t_mean_between_p(num) = mean(temp(temp ~= 0));
%     desv_t_mean_between_p(num) = std(temp(temp ~= 0));
%     ci_mean_t_mean_between_p(num) = desv_t_mean_between_p(num)/sqrt(1000)*norminv(.05/2);
%     prctile_t_mean_between_p(num) = prctile(temp(temp ~= 0), 98);
% 
%     promedio_paquetes(num) = mean(paquetes);
%     desv_paquetes(num) = std(paquetes);
%     ci_promedio_paquetes(num) = desv_paquetes(num)/sqrt(1000)*norminv(.05/2);
