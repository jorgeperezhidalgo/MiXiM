clc; clear;

fila_scalar = 1; % File where the data begins, starts with 0
columna_scalar= 9; % Column where the data begins, starts with 0, 9 and not 3 because there are a lot of , in the middle

%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Variables for file name %
%%%%%%%%%%%%%%%%%%%%%%%%%%%
num = 0;

n= [2:70];

for N = 2:70
       
        num = num + 1;

%%%%%%%%%%%%%%%%%%%        
% Scalar Analysis %
%%%%%%%%%%%%%%%%%%%

        num_slots = csvread(strcat('scalar-N=', num2str(N), '.csv'), fila_scalar, columna_scalar);
        
%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Mean and Var calculation %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        
        promedio_slots(num) = mean(num_slots);
        desv_slots(num) = std(num_slots);
        ci_promedio_slots(num) = desv_slots(num)/sqrt(size(num_slots,1))*norminv(.05/2);

end
