const true=1, false=0;
var x, y, m, n, pf;
procedure prime;
var i, f;
procedure mod;
x := x - x/y * y;
begin
  f := true;
  i := 3;
  while i<m do
  begin
    x := m;
    y := i;
    call mod;
    if x = 0 then f := false;
    i := i + 2;
  end;
  if f = true then
  begin
    write (m);
    pf := true;
  end;
end;
begin
  pf := false;
  read (n);
  while n >= 2 do
  begin
    write (2);
    if n = 2 then pf := true;
    m := 3;
    while m <= n do
    begin
      call prime;
      m := m + 2;
    end;
    read (n);
  end;
  if pf = false then write (0);
end;
