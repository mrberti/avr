format short eng
more off

F_CPU = 16e6;
N_DIV = [1 8 64 256 1024];

T = 50e-6

f_div = F_CPU./N_DIV
t_div = 1./f_div

OCRA = (T./t_div)-1