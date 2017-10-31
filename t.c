#define a(1,2) a

int main(){
  a;
  void test() {
    printf("%s", __func__);
  }
  test();
}
