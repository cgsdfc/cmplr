#include <utillib/logging.h>
#include <utillib/test.h>

UTILLIB_TEST(helloworld) {
  UTILLIB_LOG(INFO, "helloworld");
  UTILLIB_LOG(INFO, "helloworld");
  UTILLIB_LOG(INFO, "helloworld");
  UTILLIB_LOG(INFO, "helloworld");
  UTILLIB_LOG(INFO, "helloworld");
  UTILLIB_LOG(INFO, "helloworld");
  UTILLIB_LOG(INFO, "helloworld");
  UTILLIB_LOG(INFO, "helloworld");
  UTILLIB_LOG(INFO, "helloworld");
  UTILLIB_LOG(INFO, "helloworld");
  UTILLIB_LOG(INFO, "helloworld");
  UTILLIB_LOG(INFO, "helloworld");
  UTILLIB_LOG(INFO, "helloworld");
  UTILLIB_LOG(INFO, "helloworld");
  UTILLIB_LOG(INFO, "helloworld");
}

UTILLIB_TEST_BEGIN(Utillib_Logging)
UTILLIB_TEST_RUN(helloworld)
UTILLIB_TEST_END(logging)
