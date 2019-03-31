//
//  DJson.c
//  CalendarPickView
//
//  Created by duha on 2019/3/31.
//  Copyright © 2019 www.microlink.im. All rights reserved.
//

#include "DJson.h"
#include <string.h>
#include <stdlib.h>

JsonValue *parse(char *jsonString);
static inline JsonValue *parseArray(char *jsonString, int *len);
static inline JsonValue *parseObject(char *jsonString, int *len);
//释放json数据
void freeJson(JsonValue *value);
//打印json数据
void dump(JsonValue *value);

static inline JsonValue *parseString(char *jsonString, int *len) {
    JsonValue *value = NULL;
    char *ptr = jsonString + 1;
    
    while (*ptr != '"') {
        if(*ptr == '\\') {
            ptr++;
        }
        
        ptr++;
    }
    
    value = (JsonValue *)malloc(sizeof(JsonValue));
    value->type = DJsonTypeString;
    value->value = (void *)(jsonString + 1);
    value->next = NULL;
    *ptr = '\0';
    *len = (int)(ptr - jsonString);
    value->value = (char *)malloc(sizeof(char) * (*len - 2));
    memcpy(value->value, jsonString + 1, *len - 1);
    return value;
}

static inline JsonValue *parseNumber(char *jsonString, int *len) {
    JsonValue *value = NULL;
    int64_t *integer = (int64_t *)malloc(sizeof(int64_t));
    char *ptr = jsonString;
    value = (JsonValue *)malloc(sizeof(JsonValue));
    value->next = NULL;
    *integer = 0;
    
    while (*ptr <= '9' && *ptr >= '0') {
        *integer = *integer * 10 + (*ptr - '0');
        ptr++;
    }
    
    if(*ptr == '.') {
        double *number = (double *)malloc(sizeof(double));
        *number = *integer;
        free(integer);
        ptr++;
        double fraction = 1;
        value->type = DJsonTypeFloat;
        value->value = (void *)number;
        
        while (*ptr <= '9' && *ptr >= '0') {
            fraction *= 0.1;
            *number += (*ptr - '0') * fraction;
            ptr++;
        }
    }else {
        value->type = DJsonTypeInt;
        value->value = (void *)integer;
    }
    
    *len = (int)(ptr - jsonString);
    
    return value;
}

static inline JsonValue *parseBool(char *jsonString, int *len) {
    unsigned char *bValue = (unsigned char *)malloc(sizeof(unsigned char));
    JsonValue *value = (JsonValue *)malloc(sizeof(JsonValue));
    value->next = NULL;
    value->type = DJsonTypeBool;
    value->value = (void *)bValue;
    
    if(*jsonString == 'f') {
        *bValue = 0;
        *len = 5;
        return value;
    }else {
        *bValue = 1;
        *len = 4;
        return value;
    }
}

static inline JsonValue *parseNull(char *jsonString, int *len) {
    JsonValue *value = (JsonValue *)malloc(sizeof(JsonValue));
    value->type = DJsonTypeNull;
    *len = 4;
    
    return value;
}

static inline JsonValue *parseObject(char *jsonString, int *len) {
    JsonValue *value = (JsonValue *)malloc(sizeof(JsonValue));
    JsonValue *originValue = value, *tmp = NULL, *previous = NULL;
    value->next = NULL;
    value->type = DJsonTypeObject;
    value->key = NULL;
    value->value = NULL;
    int length = 0;
    char *startPtr = jsonString + 1;
    
    do {
        tmp = parseString(startPtr, &length);
        startPtr += length + 2;
        value->key = tmp;
        
        if(*startPtr == '"') {
            tmp = parseString(startPtr, &length);
        }else if(*startPtr <= '9' && *startPtr >= '0') {
            tmp = parseNumber(startPtr, &length);
        }else if(*startPtr == 'f' || *startPtr == 't') {
            tmp = parseBool(startPtr, &length);
        }else if(*startPtr == 'n') {
            tmp = parseNull(startPtr, &length);
        }else if(*startPtr == '['){
            tmp = parseArray(startPtr, &length);
        }else if(*startPtr == '{') {
            tmp = parseObject(startPtr, &length);
        }else {
            length++;
        }
        
        startPtr += length + 1;
        value->value = (void *)tmp;
        
        previous = value;
        value->next = (JsonValue *)malloc(sizeof(JsonValue));
        value = value->next;
        value->key = NULL;
        value->value = NULL;
        
        if(*startPtr == ',') {
            startPtr++;
        }
    }while (*startPtr != '}' && *startPtr != '\0');
    
    if(value->value == NULL && originValue != value) {
        previous->next = NULL;
        free(value);
    }
    
    *len = (int)(startPtr - jsonString);
    return originValue;
}

static inline JsonValue *parseArray(char *jsonString, int *len) {
    JsonValue *value = (JsonValue *)malloc(sizeof(JsonValue));
    JsonValue *originValue = value, *tmp = NULL, *previous = NULL;
    value->type = DJsonTypeArray;
    value->next = NULL;
    char *startPtr = jsonString + 1;
    int length = 0;
    
    do {
        if(*startPtr == '"') {
            tmp = parseString(startPtr, &length);
        }else if(*startPtr <= '9' && *startPtr >= '0') {
            tmp = parseNumber(startPtr, &length);
        }else if(*startPtr == 'f' || *startPtr == 't') {
            tmp = parseBool(startPtr, &length);
        }else if(*startPtr == 'n') {
            tmp = parseNull(startPtr, &length);
        }else if(*startPtr == '['){
            tmp = parseArray(startPtr, &length);
        }else if(*startPtr == '{') {
            tmp = parseObject(startPtr, &length);
        }else {
            length++;
        }
        
        value->value = (void *)tmp;
        previous = value;
        value->next = (JsonValue *)malloc(sizeof(JsonValue));
        value = value->next;
        value->value = NULL;
        
        startPtr += length + 1;
        
        if(*startPtr == ',') {
            startPtr++;
        }
    }while (*startPtr != ']' && *startPtr != '\0');
    
    if(value->value == NULL && originValue != value) {
        previous->next = NULL;
        free(value);
    }
    
    *len = (int)(startPtr - jsonString);
    return originValue;
}

JsonClass *newJClass(char *jsonString) {
    char *jsonV = (char *)malloc(strlen(jsonString));
    memcpy(jsonV, jsonString, strlen(jsonString));
    JsonClass *v = (JsonClass *)malloc(sizeof(JsonClass));
    v->dump = (void *)&dump;
    v->free = (void *)&freeJson;
    v->value = parse(jsonV);
    free(jsonV);
    
    return v;
}

void dumpValue(JsonValue *value) {
    if(value->type == DJsonTypeString) {
        printf("%s\n", (char *)value->value);
    }else if(value->type == DJsonTypeFloat) {
        printf("%f\n", *((double *)value->value));
    }else if(value->type == DJsonTypeInt) {
        printf("%lld\n", *((int64_t *)value->value));
    }
}

//打印json数据
void dump(JsonValue *value) {
    if(value->type == DJsonTypeObject) {
        while (value && value->key != NULL) {
            dumpValue(value->key);
            JsonValue *tv = value->value;
            
            if(tv->type == DJsonTypeArray || tv->type == DJsonTypeObject) {
                dump(tv);
            }else {
                dumpValue(tv);
            }
            
            value = value->next;
        }
    }else if(value->type == DJsonTypeArray){
        while (value && value->value != NULL) {
            JsonValue *tv = value->value;
            
            if(tv->type == DJsonTypeArray || tv->type == DJsonTypeObject) {
                dump(tv);
            }else {
                dumpValue(tv);
            }
            
            value = value->next;
        }
    }
}

JsonValue *parse(char *jsonString) {
//    int jsonLength = (int)strlen(jsonString),
    int length;
    JsonValue *value = NULL;
//    char *startPtr = jsonString, *endPtr = jsonString + jsonLength;
    
    if(*jsonString == '[') {
        value = parseArray(jsonString, &length);
    }else if(*jsonString == '{') {
        value = parseObject(jsonString, &length);
    }
    
    return value;
}

//释放json数据
void freeJson(JsonValue *value) {
    if(value->type == DJsonTypeObject) {
        while (value != NULL && value->value != NULL && value->key != NULL) {
            JsonValue *key = value->key;
            JsonValue *tvalue = value->value;
            JsonValue *next = value->next;
            
            free(key->value);
            free(key);
            
            if(tvalue->type == DJsonTypeArray || tvalue->type == DJsonTypeObject) {
                freeJson(tvalue);
            }else {
                if(tvalue->type != DJsonTypeNull) {
                    free(tvalue->value);
                }
                
                free(tvalue);
            }
            
            free(value);
            value = next;
        }
    }else if(value->type == DJsonTypeArray){
        while (value && value->value != NULL) {
            JsonValue *tvalue = value->value;
            JsonValue *next = value->next;
            
            if(tvalue->type == DJsonTypeArray || tvalue->type == DJsonTypeObject) {
                freeJson(tvalue);
            }else {
                if(tvalue->type != DJsonTypeNull) {
                    free(tvalue->value);
                }
                
                free(tvalue);
            }
            
            free(value);
            value = next;
        }
    }
}
