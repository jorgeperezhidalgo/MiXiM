clc; clear;

fila_scalar_random1 = 1; % File where the data begins, starts with 0
columna_scalar_random1= 13; % Column where the data begins, starts with
fila_scalar_random2 = 1; % File where the data begins, starts with 0
columna_scalar_random2= 11; % Column where the data begins, starts with
fila_scalar_slotted = 1;
columna_scalar_slotted = 9;

%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Variables for file name %
%%%%%%%%%%%%%%%%%%%%%%%%%%%
num = 0;
T=3;

for N = [30, 40, 50, 60, 70]
    
        T = T + 2;
        
    for P = 10:10
        
        num = num + 1;
        tiempo = 0;
        

%%%%%%%%%%%%%%%%%%%
% Vector Analysis %
%%%%%%%%%%%%%%%%%%%

%         data_tiempo = csvread(strcat('vector-N=', num2str(N), '-T=', num2str(T), '.csv'), fila_vector, columna_vector);
% 
%         for i=2:size(data_tiempo,2)
%             t_last_p(i-1,num) = 0;
%             t_first_p(i-1,num) = 0;
%             t_last_1_p(i-1,num) = 0;
%             t_last_2_p(i-1,num) = 0;
%             t_last_3_p(i-1,num) = 0;
%             t_between_p = 0;
%             t_mean_between_p(i-1,num) = 0;
%             num_paq_rec = 0;
%             for j=1:size(data_tiempo,1)
%                 if (data_tiempo(j,i) > 0)
%                     num_paq_rec = num_paq_rec + 1;
%                     tiempo(num_paq_rec,1,i-1)=data_tiempo(j,1); % Time of packet arrival column
%                     tiempo(num_paq_rec,2,i-1)=data_tiempo(j,i); % Sender AN id column
%                     tiempo(num_paq_rec,3,i-1)=size(find(tiempo(:,2,i-1) == data_tiempo(j,i)), 1); % Number of received packet from an AN column
%                 end
%             end
%             for k=size(tiempo,1):-1:1
%                 if (tiempo(k,2,i-1) ~= 0)
%                     if (t_last_p(i-1,num) == 0)
%                         t_last_p(i-1,num) = tiempo(k,1,i-1);
%                     end
%                 end
%                 if (tiempo(k,3,i-1) == 3)
%                     if (t_last_3_p(i-1,num) == 0)
%                         t_last_3_p(i-1,num) = tiempo(k,1,i-1);
%                     end
%                 end
%                 if (tiempo(k,3,i-1) == 2)
%                     if (t_last_2_p(i-1,num) == 0)
%                         t_last_2_p(i-1,num) = tiempo(k,1,i-1);
%                     end
%                 end
%                 if (tiempo(k,3,i-1) == 1)
%                     if (t_last_1_p(i-1,num) == 0)
%                         t_last_1_p(i-1,num) = tiempo(k,1,i-1);
%                     end
%                 end
%                 if (tiempo(k,1,i-1) ~= 0)
%                     if (k >= 2)
%                         t_between_p(k) = tiempo(k,1,i-1) - tiempo(k-1,1,i-1);
%                     else
%                         t_between_p(k) = tiempo(k,1,i-1);
%                     end
%                 end
%             end
%             t_first_p(i-1,num) = tiempo(1,1,i-1);
%             t_mean_between_p(i-1,num) = mean(t_between_p);
%         end
        
%%%%%%%%%%%%%%%%%%%        
% Scalar Analysis %
%%%%%%%%%%%%%%%%%%%

        data_paquetes_random1 = csvread(strcat('scalar-N=', num2str(N), '-P=', num2str(P), '-T=', num2str(T), '.csv'), fila_scalar_random1, columna_scalar_random1);
        data_paquetes_random2 = csvread(strcat('scalar-N=', num2str(N), '-T=1.csv'), fila_scalar_random2, columna_scalar_random2);
        data_paquetes_slotted = csvread(strcat('scalar-N=', num2str(N), '.csv'), fila_scalar_slotted, columna_scalar_slotted);
        
        paquetes_random1 = data_paquetes_random1(:, 3) - data_paquetes_random1(:, 4) + data_paquetes_random1(:, 1) - data_paquetes_random1(:, 2);
        paquetes_random2 = data_paquetes_random2(:, 3) - data_paquetes_random2(:, 4) + data_paquetes_random2(:, 1) - data_paquetes_random2(:, 2);
        paquetes_slotted = data_paquetes_slotted(:, 3) - data_paquetes_slotted(:, 4) + data_paquetes_slotted(:, 1) - data_paquetes_slotted(:, 2);
        
        paqmat_random1 = vec2mat(paquetes_random1, 100);
        paqmat_random2 = vec2mat(paquetes_random2, 100);
        paqmat_slotted = vec2mat(paquetes_slotted, 100);
        
        

%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Mean and Var calculation %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        
%         temp = t_last_p(:,num);
%         mean_t_last_p(num) = mean(temp(temp ~= 0));
%         desv_t_last_p(num) = std(temp(temp ~= 0));
%         ci_mean_t_last_p(num) = desv_t_last_p(num)/sqrt(1000)*norminv(.05/2);
%         prctile_t_last_p(num) = prctile(temp(temp ~= 0), 98);
% 
%         temp = t_first_p(:,num);
%         mean_t_first_p(num) = mean(temp(temp ~= 0));
%         desv_t_first_p(num) = std(temp(temp ~= 0));
%         ci_mean_t_first_p(num) = desv_t_first_p(num)/sqrt(1000)*norminv(.05/2);
%         prctile_t_first_p(num) = prctile(temp(temp ~= 0), 98);
% 
%         temp = t_last_1_p(:,num);
%         mean_t_last_1_p(num) = mean(temp(temp ~= 0));
%         desv_t_last_1_p(num) = std(temp(temp ~= 0));
%         ci_mean_t_last_1_p(num) = desv_t_last_1_p(num)/sqrt(1000)*norminv(.05/2);
%         prctile_t_last_1_p(num) = prctile(temp(temp ~= 0), 98);
% 
%         temp = t_last_2_p(:,num);
%         mean_t_last_2_p(num) = mean(temp(temp ~= 0));
%         desv_t_last_2_p(num) = std(temp(temp ~= 0));
%         ci_mean_t_last_2_p(num) = desv_t_last_2_p(num)/sqrt(1000)*norminv(.05/2);
%         prctile_t_last_2_p(num) = prctile(temp(temp ~= 0), 98);
% 
%         temp = t_last_3_p(:,num);
%         mean_t_last_3_p(num) = mean(temp(temp ~= 0));
%         desv_t_last_3_p(num) = std(temp(temp ~= 0));
%         ci_mean_t_last_3_p(num) = desv_t_last_3_p(num)/sqrt(1000)*norminv(.05/2);
%         prctile_t_last_3_p(num) = prctile(temp(temp ~= 0), 98);
% 
%         temp = t_mean_between_p(:,num);
%         mean_t_mean_between_p(num) = mean(temp(temp ~= 0));
%         desv_t_mean_between_p(num) = std(temp(temp ~= 0));
%         ci_mean_t_mean_between_p(num) = desv_t_mean_between_p(num)/sqrt(1000)*norminv(.05/2);
%         prctile_t_mean_between_p(num) = prctile(temp(temp ~= 0), 98);

        promedio_paquetes_random1(num) = mean(mean(paqmat_random1));
        desv_paquetes_random1(num) = std(mean(paqmat_random1));
        ci_promedio_paquetes_random1(num) = desv_paquetes_random1(num)/sqrt(size(paqmat_random1,2))*norminv(.05/2);
        promedio_paquetes_random2(num) = mean(mean(paqmat_random2));
        desv_paquetes_random2(num) = std(mean(paqmat_random2));
        ci_promedio_paquetes_random2(num) = desv_paquetes_random2(num)/sqrt(size(paqmat_random2,2))*norminv(.05/2);
        promedio_paquetes_slotted(num) = mean(mean(paqmat_slotted));
        desv_paquetes_slotted(num) = std(mean(paqmat_slotted));
        ci_promedio_paquetes_slotted(num) = desv_paquetes_slotted(num)/sqrt(size(paqmat_slotted,2))*norminv(.05/2);

    end
end


errorbar([30 40 50 60 70],promedio_paquetes_random1,ci_promedio_paquetes_random1)
hold on
errorbar([30 40 50 60 70],promedio_paquetes_slotted,ci_promedio_paquetes_slotted,'r')
errorbar([30 40 50 60 70],promedio_paquetes_random2,ci_promedio_paquetes_random2,'g')
