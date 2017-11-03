const int i=0;

int main()
{
  int& ii=const_cast<int&> (i);
  ii=2;
}
