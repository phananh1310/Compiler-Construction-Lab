/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

void freeObject(Object* obj);
void freeScope(Scope* scope);
void freeObjectList(ObjectNode *objList);
void freeReferenceList(ObjectNode *objList);

SymTab* symtab;
Type* intType;
Type* charType;

/******************* Type utilities ******************************/

Type* makeIntType(void) {
  Type* type = (Type*) malloc(sizeof(Type));
  type->typeClass = TP_INT;
  return type;
}

Type* makeCharType(void) {
  Type* type = (Type*) malloc(sizeof(Type));
  type->typeClass = TP_CHAR;
  return type;
}

Type* makeArrayType(int arraySize, Type* elementType) {
  Type* type = (Type*) malloc(sizeof(Type));
  type->typeClass = TP_ARRAY;
  type->arraySize = arraySize;
  type->elementType = elementType;
  return type;
}

Type* duplicateType(Type* type) {
  Type* newType = (Type*) malloc(sizeof(Type));
  newType->typeClass = type->typeClass;
  if (type->typeClass==TP_ARRAY){
    newType->arraySize = type->arraySize;
    newType->elementType = type->elementType;
  }
  return newType;
}

int compareType(Type* type1, Type* type2) {
  // TODO
  // I don't know what it's for 

  return 0;
}

void freeType(Type* type) {
  free(type->elementType);
  free(type);
}

/******************* Constant utility ******************************/

ConstantValue* makeIntConstant(int i) {
  ConstantValue *constant = (ConstantValue*) malloc(sizeof(ConstantValue));
  constant->type = TP_INT;
  constant->intValue = i;
  return constant;
}

ConstantValue* makeCharConstant(char ch) {
  ConstantValue *constant = (ConstantValue*) malloc(sizeof(ConstantValue));
  constant->type = TP_CHAR;
  constant->charValue = ch;
  return constant;
}

ConstantValue* duplicateConstantValue(ConstantValue* v) {
  ConstantValue *constant = (ConstantValue*) malloc(sizeof(ConstantValue));
  constant->type = v->type;
  if (v->type == TP_INT)
    constant->intValue = v->intValue;
  else if (v->type == TP_CHAR)
    constant->charValue = v->charValue;
  return constant;
}

/******************* Object utilities ******************************/

Scope* createScope(Object* owner, Scope* outer) {
  Scope* scope = (Scope*) malloc(sizeof(Scope));
  scope->objList = NULL;
  scope->owner = owner;
  scope->outer = outer;
  return scope;
}

Object* createProgramObject(char *programName) {
  Object* program = (Object*) malloc(sizeof(Object));
  strcpy(program->name, programName);
  program->kind = OBJ_PROGRAM;
  program->progAttrs = (ProgramAttributes*) malloc(sizeof(ProgramAttributes));
  program->progAttrs->scope = createScope(program,NULL);
  symtab->program = program;

  return program;
}

Object* createConstantObject(char *name) {
  Object* constant = (Object*) malloc(sizeof(Object));
  strcpy(constant->name, name);
  constant->kind = OBJ_CONSTANT;
  constant->constAttrs = (ConstantAttributes*) malloc(sizeof(ConstantAttributes));
  
  return constant;
}

Object* createTypeObject(char *name) {
  Object* type = (Object*) malloc(sizeof(Object));
  strcpy(type->name, name);
  type->kind = OBJ_TYPE;
  type->typeAttrs = (TypeAttributes*) malloc(sizeof(TypeAttributes));
  
  return type;
}

Object* createVariableObject(char *name) {
  Object* variable = (Object*) malloc(sizeof(Object));
  strcpy(variable->name, name);
  variable->kind = OBJ_VARIABLE;
  variable->varAttrs = (VariableAttributes*) malloc(sizeof(VariableAttributes));
  variable->varAttrs->scope = createScope(variable,NULL);
  variable->varAttrs->scope->outer=symtab->currentScope;
  variable->varAttrs->scope->owner = variable;
  variable->varAttrs->scope->objList = NULL;
  return variable;
}

Object* createFunctionObject(char *name) {
  Object* function = (Object*) malloc(sizeof(Object));
  strcpy(function->name, name);
  function->kind = OBJ_FUNCTION;
  function->funcAttrs = (FunctionAttributes*) malloc(sizeof(FunctionAttributes));
  function->funcAttrs->scope = createScope(function,NULL);
  function->funcAttrs->scope->outer = symtab->currentScope;
  function->funcAttrs->scope->owner = function;
  function->funcAttrs->scope->objList = NULL;
  function->funcAttrs->paramList = NULL;

  return function;
}

Object* createProcedureObject(char *name) {
  Object* procedure = (Object*) malloc(sizeof(Object));
  strcpy(procedure->name, name);
  procedure->kind = OBJ_PROCEDURE;
  procedure->procAttrs = (ProcedureAttributes*) malloc(sizeof(ProcedureAttributes));
  procedure->procAttrs->scope = createScope(procedure,NULL);
  procedure->procAttrs->scope->outer = symtab->currentScope;
  procedure->procAttrs->scope->owner = procedure;
  procedure->procAttrs->scope->objList = NULL;
  procedure->procAttrs->paramList = NULL;
  return procedure;
}

Object* createParameterObject(char *name, enum ParamKind kind, Object* owner) {
  Object* parameter = (Object*) malloc(sizeof(Object));
  strcpy(parameter->name, name);
  parameter->kind = OBJ_PARAMETER;
  parameter->paramAttrs = (ParameterAttributes*) malloc(sizeof(ParameterAttributes));
  parameter->paramAttrs->kind = kind;
  parameter->paramAttrs->function = owner;
  return parameter;
}

void freeObject(Object* obj) {
  free(obj->constAttrs);
  free(obj->funcAttrs);
  free(obj->paramAttrs);
  free(obj->procAttrs);
  free(obj->procAttrs);
  free(obj->progAttrs);
  free(obj->typeAttrs);
  free(obj->varAttrs);
  free(obj);
}

void freeScope(Scope* scope) {
  freeObjectList(scope->objList);
  freeScope(scope->outer);
  freeObject(scope->owner);
  free(scope);
}

void freeObjectList(ObjectNode *objList) {
  if (objList->next!=NULL)
    freeObjectList(objList->next);
  freeObject(objList->object);
  free(objList->next);
  free(objList);
  
}

void freeReferenceList(ObjectNode *objList) {
  // not sure what this function for since I can't find where it's used
  freeObjectList(objList);
}

void addObject(ObjectNode **objList, Object* obj) {
  ObjectNode* node = (ObjectNode*) malloc(sizeof(ObjectNode));
  node->object = obj;
  node->next = NULL;

  if ((*objList) == NULL) 
    *objList = node;
  else {
    ObjectNode *n = *objList;
    while (n->next != NULL) 
      n = n->next;
    n->next = node;
  }
}

Object* findObject(ObjectNode *objList, char *name) {
  ObjectNode *n = objList;
  while (strcmp(n->object->name,name))
    n = n->next;
  return n->object;
}

/******************* others ******************************/

void initSymTab(void) {
  Object* obj;
  Object* param;

  symtab = (SymTab*) malloc(sizeof(SymTab));
  symtab->globalObjectList = NULL;
  
  obj = createFunctionObject("READC");
  obj->funcAttrs->returnType = makeCharType();
  addObject(&(symtab->globalObjectList), obj);
  
  obj = createFunctionObject("READI");
  obj->funcAttrs->returnType = makeIntType();
  addObject(&(symtab->globalObjectList), obj);

  obj = createProcedureObject("WRITEI");
  param = createParameterObject("i", PARAM_VALUE, obj);
  param->paramAttrs->type = makeIntType();
  addObject(&(obj->procAttrs->paramList),param);
  addObject(&(symtab->globalObjectList), obj);

  obj = createProcedureObject("WRITEC");
  param = createParameterObject("ch", PARAM_VALUE, obj);
  param->paramAttrs->type = makeCharType();
  addObject(&(obj->procAttrs->paramList),param);
  addObject(&(symtab->globalObjectList), obj);

  obj = createProcedureObject("WRITELN");
  addObject(&(symtab->globalObjectList), obj);

  intType = makeIntType();
  charType = makeCharType();
}

void cleanSymTab(void) {
  freeObject(symtab->program);
  freeObjectList(symtab->globalObjectList);
  free(symtab);
  freeType(intType);
  freeType(charType);
}

void enterBlock(Scope* scope) {
  symtab->currentScope = scope;
}

void exitBlock(void) {
  symtab->currentScope = symtab->currentScope->outer;
}

void declareObject(Object* obj) {
  if (obj->kind == OBJ_PARAMETER) {
    Object* owner = symtab->currentScope->owner;
    switch (owner->kind) {
    case OBJ_FUNCTION:
      addObject(&(owner->funcAttrs->paramList), obj);
      break;
    case OBJ_PROCEDURE:
      addObject(&(owner->procAttrs->paramList), obj);
      break;
    default:
      break;
    }
  }
  addObject(&(symtab->currentScope->objList), obj);
}


