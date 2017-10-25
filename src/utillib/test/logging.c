#include <utillib/logging.h>
#include <utillib/test.h>

UTILLIB_TEST_GLOBAL_SETUP() {
  utillib_logging_init(NULL);
}

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

UTILLIB_TEST_GLOBAL_TEARDOWN() {
  utillib_logging_destroy();
}

UTILLIB_TEST_BEGIN(logging)
  UTILLIB_TEST_ELEM(helloworld)
UTILLIB_TEST_END(logging)
