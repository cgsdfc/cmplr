const a = 45, b = 27;
var x, y, g, m;
procedure swap;
var temp;
begin
  temp := x;
  x:=y;
  y:=temp;
end;

procedure mod;
x := x - x / y * y;
begin
  x := a;
  y := b;
  call mod;
  while x <> 0 do
    begin
      call swap;
      call mod
    end;
  end;
  g := y;
  m := a * b / g;
  write (g, m);
end;

