#include <stdlib.h>
#include <string.h>
#include "DJson.h"

int main() {
	char *json = "{\"test1\":\"wx9fdb8ble7ce3c68f\",\"test2\":\"123456789\",\"testData1\":{\"testdatason1\":\"{\\\"test1\\\":\\\"wx9fdb8ble7ce3c68f\\\",\\\"test2\\\":\\\"123456789\\\",\\\"testData1\\\":{\\\"testdatason1\\\":\\\"97895455\\\",\\\"testdatason2\\\":3,\\\"testData2\\\":[{\\\"testshuzu1\\\":\\\"12\\\",\\\"testshuzu2\\\":\\\"11\\\"},{\\\"testshuzu2\\\":\\\"13\\\",\\\"testshuzu3\\\":\\\"14\\\"},{\\\"testshuzu3\\\":\\\"15\\\",\\\"testshuzu4\\\":\\\"16\\\"}]}}\",\"testdatason2\":3,\"testData2\":[{\"testshuzu1\":\"12\",\"testshuzu2\":\"11\"},{\"testshuzu2\":\"13\",\"testshuzu3\":\"14\"},{\"testshuzu3\":\"15\",\"testshuzu4\":\"16\"}]}}";
    
//    for(int i = 0; i < 100000; i++) {
    JsonClass *jClass = NewJsonClass(json);
    jClass->dump(jClass->value);
        jClass->free(jClass->value);
        free(jClass);
	return 1;
}
