//
//  DJson.h
//  CalendarPickView
//
//  Created by duha on 2019/3/31.
//  Copyright © 2019 www.microlink.im. All rights reserved.
//

#ifndef DJson_h
#define DJson_h

#include <stdio.h>

typedef enum {
    DJsonTypeInt,
    DJsonTypeFloat,
    DJsonTypeString,
    DJsonTypeObject,
    DJsonTypeArray,
    DJsonTypeBool,
    DJsonTypeNull,
    DJsonTypeEnd
}DJsonType;

typedef struct JValue {
    DJsonType type;
    void *value;
    struct JValue *next;
    struct JValue *key;
}JsonValue;

typedef struct jsonClass {
    void (*dump)(JsonValue *);
    void (*free)(JsonValue *);
    JsonValue *value;
}JsonClass;

JsonClass *newJClass(char *jsonString);

#define NewJsonClass(str) newJClass(str);
#endif /* DJson_h */
