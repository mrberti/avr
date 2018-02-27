clear variables;
more off;

%F_CPU = 14.7456e6;
F_CPU = 16e6;
BAUD_SET = [9600 14400 19200 28800 38400 57600 76800 115200 230400 250000 500000 1000000];

F_CPU_i = cast(F_CPU,"uint32");
BAUD_SET_i = cast(BAUD_SET, "uint32");

UBRR = ((F_CPU ./ BAUD_SET) / 16) - 1;
UBRR_i = (F_CPU_i ./ BAUD_SET_i / 16) - 1
UBRR_i2 = ((F_CPU_i+BAUD_SET_i*8)./(BAUD_SET_i*16))-1

diff1 = UBRR-cast(UBRR_i,"double");
diff2 = UBRR-cast(UBRR_i2,"double");
err1 = cast(diff1, "double")./UBRR*100;
err2 = cast(diff2, "double")./UBRR*100;

figure();
subplot(3,1,1);
stem([UBRR;cast(UBRR_i,"double");cast(UBRR_i2,"double")]');
subplot(3,1,2);
stem([diff1;diff2]');
subplot(3,1,3);
stem([err1;err2]');