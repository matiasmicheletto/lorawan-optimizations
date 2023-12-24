%Heuristica. A ver si sale

fid = fopen("100_25.dat");
S = dlmread(fid);
A = zeros(1,length(S(:,1)));
G = zeros(1,25);
E = zeros(1,25);
GW = zeros(25,100);

% Cálculo de asociatividad por nodos y por gws
for i = 1:100
    for j = 1:25
      if (S(i,j) < 100 && (2^(S(i,j)-7)/S(i,26)) <= 0.01)
        A(i) = A(i)+1;
      else
        S(i,j) = 100;
      end
    end
end

for j = 1:25
    for i = 1:100
      if (S(i,j) < 100)
        G(j) = G(j)+1;
      end
    end
end

% Selección de gws esenciales.
for i = 1:100
  if A(i) == 1
    for j = 1:25
      if S(i,j) < 100
        E(j) = 1;
      end
    end
  end
end


for j = 1:25
  if E(j) == 1
    for i = 1:100
      if S(i,j) < 100
        GW(j,i) = S(i,j);
        S(i,1:25) = ones(1,25)*100;
      end
    end
  end
end

while length(find( GW > 0)) < 100
  maxG = 0;
  longitud = 0;
  for j = 1:25
    if E(j) == 0
      if longitud < length(find(S(:,j) < 100))
        maxG = j;
        longitud = length(find(S(:,j) < 100));
      end
    end
  end
  E(maxG) = 1;
  for i = 1:100
    if (S(i,maxG) < 100)
      GW(maxG,i) = S(i,maxG);
      S(i,1:25) = ones(1,25)*100;
    end
  end
end

% costo de la FO
CG = length(find(E))
eng = 0;
for j = 1:25
  for i = 1:100
     if GW(j,i)>0
      eng = eng+2^(GW(j,i)-7);
     end
  end
end

eng
