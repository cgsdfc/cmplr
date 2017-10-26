#define _GNU_SOURCE
#include <utillib/test.h>

UTILLIB_TEST_DECLARE(Utillib_Logging);
UTILLIB_TEST_DECLARE(Utillib_Test);
UTILLIB_TEST_DECLARE(Utillib_Vector);
UTILLIB_TEST_DECLARE(Utillib_Slist);

int main() {
  UTILLIB_TEST_RUN_ALL(
      Utillib_Test,
      Utillib_Vector,
      Utillib_Slist
  );
}
