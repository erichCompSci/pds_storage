SET(CTEST_PROJECT_NAME "Pds")
SET(CTEST_NIGHTLY_START_TIME "17:36:6 GMT")

IF(NOT DEFINED CTEST_DROP_METHOD)
  SET(CTEST_DROP_METHOD "http")
ENDIF(NOT DEFINED CTEST_DROP_METHOD)

IF(CTEST_DROP_METHOD STREQUAL "http")
  SET(CTEST_DROP_SITE "cdash.cercs.gatech.edu")
  SET(CTEST_DROP_LOCATION "/submit.php?project=Pds")
  SET(CTEST_TRIGGER_SITE "cdash.cercs.gatech.edu")
ENDIF(CTEST_DROP_METHOD STREQUAL "http")

