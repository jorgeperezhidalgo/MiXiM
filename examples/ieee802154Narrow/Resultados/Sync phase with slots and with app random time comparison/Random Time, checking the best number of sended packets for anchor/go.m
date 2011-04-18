clc; clear;

fila_scalar = 1; fila_vector = 1; % File where the data begins, starts with 0
columna_scalar= 13; columna_vector = 0; % Column where the data begins, starts with 0

%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Variables for file name %
%%%%%%%%%%%%%%%%%%%%%%%%%%%
num = 0;
T=3;

for N = [30, 40, 50, 60, 70]
    
        T = T + 2;
        
    for P = 3:16
        
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

        data_paquetes = csvread(strcat('scalar-N=', num2str(N), '-P=', num2str(P), '-T=', num2str(T), '.csv'), fila_scalar, columna_scalar);
        
        paquetes = data_paquetes(:, 3) - data_paquetes(:, 4) + data_paquetes(:, 1) - data_paquetes(:, 2);
        
        paqmat = vec2mat(paquetes, 100);
        
        

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

        promedio_paquetes(num) = mean(mean(paqmat));
        desv_paquetes(num) = std(mean(paqmat));
        ci_promedio_paquetes(num) = desv_paquetes(num)/sqrt(size(paqmat,2))*norminv(.05/2);

    end
end


errorbar(3:16,promedio_paquetes(1:14),ci_promedio_paquetes(1:14))
hold on
errorbar(3:16,promedio_paquetes(15:28),ci_promedio_paquetes(15:28),'r')
errorbar(3:16,promedio_paquetes(29:42),ci_promedio_paquetes(29:42),'g')
errorbar(3:16,promedio_paquetes(43:56),ci_promedio_paquetes(43:56),'k')
errorbar(3:16,promedio_paquetes(57:70),ci_promedio_paquetes(57:70),'y')