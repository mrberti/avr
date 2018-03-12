clear variables;
more off;
clc;

i_start = 569;
b = 10;

i = i_start

div = 1;
while div < i
  div = div * 10;
end

%div = div/b
while div > 0
  d = floor(i/div)
  i = i - div*d
  div = floor(div/b)
end