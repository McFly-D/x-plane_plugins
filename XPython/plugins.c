#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdbool.h>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMPlugin.h>
#include "utils.h"
#include "xppythontypes.h"

PyObject *XPLMGetMyIDFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  return PyLong_FromLong(XPLMGetMyID());
}

PyObject *XPLMCountPluginsFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  return PyLong_FromLong(XPLMCountPlugins());
}

PyObject *XPLMGetNthPluginFun(PyObject *self, PyObject *args)
{
  (void) self;
  int inIndex;
  if(!PyArg_ParseTuple(args, "i", &inIndex)){
    return NULL;
  }
  return PyLong_FromLong(XPLMGetNthPlugin(inIndex));
}

PyObject *XPLMFindPluginByPathFun(PyObject *self, PyObject *args)
{
  (void) self;
  const char *inPath;
  if(!PyArg_ParseTuple(args, "s", &inPath)){
    return NULL;
  }
  return PyLong_FromLong(XPLMFindPluginByPath(inPath));
}

PyObject *XPLMFindPluginBySignatureFun(PyObject *self, PyObject *args)
{
  (void) self;
  const char *inSignature;
  if(!PyArg_ParseTuple(args, "s", &inSignature)){
    return NULL;
  }
  return PyLong_FromLong(XPLMFindPluginBySignature(inSignature));
}

PyObject *XPLMGetPluginInfoFun(PyObject *self, PyObject *args)
{
  (void) self;
  int inPlugin;
  if(!PyArg_ParseTuple(args, "i", &inPlugin)){
    return NULL;
  }
  char outName[512];
  char outFilePath[512];
  char outSignature[512];
  char outDescription[512];
  XPLMGetPluginInfo(inPlugin, outName, outFilePath, outSignature, outDescription);
  return PyPluginInfo_New(outName, outFilePath, outSignature, outDescription);
}

PyObject *XPLMIsPluginEnabledFun(PyObject *self, PyObject *args)
{
  (void) self;
  int inPluginID;
  if(!PyArg_ParseTuple(args, "i", &inPluginID)){
    return NULL;
  }
  return PyLong_FromLong(XPLMIsPluginEnabled(inPluginID));
}

PyObject *XPLMEnablePluginFun(PyObject *self, PyObject *args)
{
  (void) self;
  int inPluginID;
  if(!PyArg_ParseTuple(args, "i", &inPluginID)){
    return NULL;
  }
  return PyLong_FromLong(XPLMEnablePlugin(inPluginID));
}

PyObject *XPLMDisablePluginFun(PyObject *self, PyObject *args)
{
  (void) self;
  int inPluginID;
  if(!PyArg_ParseTuple(args, "i", &inPluginID)){
    return NULL;
  }
  XPLMDisablePlugin(inPluginID);
  Py_RETURN_NONE;
}

PyObject *XPLMReloadPluginsFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  XPLMReloadPlugins();
  Py_RETURN_NONE;
}

PyObject *XPLMSendMessageToPluginFun(PyObject *self, PyObject *args)
{
  (void) self;
  long inPluginID;
  long inMessage;
  PyObject* inParam;
  if(!PyArg_ParseTuple(args, "llO", &inPluginID, &inMessage, &inParam)){
    return NULL;
  }
  void *msgParam;
  if(PyLong_Check(inParam)) {
    msgParam = (void*)PyLong_AsLong(inParam);
    XPLMSendMessageToPlugin(inPluginID, inMessage, msgParam);
  } else if (PyUnicode_Check(inParam)) {
    msgParam = objToStr(inParam);
    XPLMSendMessageToPlugin(inPluginID, inMessage, msgParam);
    free(msgParam);
  } else {
    printf("Unknown data type for XPLMSendMessageToPlugin(... inParam). Cannot convert\n");
  }
  Py_RETURN_NONE;
}

PyObject *XPLMHasFeatureFun(PyObject *self, PyObject *args)
{
  (void) self;
  const char *inFeature;
  if(!PyArg_ParseTuple(args, "s", &inFeature)){
    return NULL;
  }
  return PyLong_FromLong(XPLMHasFeature(inFeature));
}

PyObject *XPLMIsFeatureEnabledFun(PyObject *self, PyObject *args)
{
  (void) self;
  const char *inFeature;
  if(!PyArg_ParseTuple(args, "s", &inFeature)){
    return NULL;
  }
  return PyLong_FromLong(XPLMIsFeatureEnabled(inFeature));
}

PyObject *XPLMEnableFeatureFun(PyObject *self, PyObject *args)
{
  (void) self;
  const char *inFeature;
  int inEnable;
  if(!PyArg_ParseTuple(args, "si", &inFeature, &inEnable)){
    return NULL;
  }
  if (!inEnable && ! (strcmp(inFeature, "XPLM_USE_NATIVE_PATHS") && strcmp(inFeature, "XPLM_USE_NATIVE_WIDGET_WINDOWS"))) {
    PyErr_SetString(PyExc_RuntimeError, "A Python plugins is attempting to disable XPLM_USE_NATIVE_PATHS or XPLM_USE_NATIVE_WIDGET_WINDOWS feature, not allowed");
  } else {
    XPLMEnableFeature(inFeature, inEnable);
  }
  Py_RETURN_NONE;
}

static PyObject *feDict;
static intptr_t feCntr;

static void featureEnumerator(const char *inFeature, void *inRef)
{
  PyObject *key = PyLong_FromVoidPtr(inRef);
  PyObject *callbackInfo = PyDict_GetItem(feDict, key);
  Py_DECREF(key);
  if(callbackInfo == NULL){
    printf("Unknown feature enumeration callback requested! (inFeature = '%s' inRef = %p)\n", inFeature, inRef);
    return;
  }
  PyObject *inFeatureObj = PyUnicode_FromString(inFeature);
  PyObject *res = PyObject_CallFunctionObjArgs(PyTuple_GetItem(callbackInfo, 1),
                                        inFeatureObj, PyTuple_GetItem(callbackInfo, 2), NULL);
  PyObject *err = PyErr_Occurred();
  Py_DECREF(inFeatureObj);
  if(err){
    printf("Error occured during the feature enumeration callback(inFeature = '%s' inRef = %p):\n", inFeature, inRef);
    PyErr_Print();
  }
  Py_XDECREF(res);
}


PyObject *XPLMEnumerateFeaturesFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *fun;
  PyObject *ref;
  PyObject *pluginSelf;
  if(!PyArg_ParseTuple(args, "OO", &fun, &ref))
    return NULL;
  pluginSelf = get_pluginSelf();

  PyObject *argsObj = Py_BuildValue("(OOO)", pluginSelf, fun, ref);
  PyObject *key = PyLong_FromLong(feCntr++);
  void *inRef = PyLong_AsVoidPtr(key);
  PyDict_SetItem(feDict, key, argsObj);
  Py_DECREF(argsObj);
  XPLMEnumerateFeatures(featureEnumerator, inRef);
  Py_RETURN_NONE;
}

void plugins_cleanup(void) {
  if (! (feDict && PyDict_Check(feDict))) {
    return;
  }
  PyDict_Clear(feDict);
  Py_DECREF(feDict);
}

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  if (! (feDict && PyDict_Check(feDict))) {
    Py_RETURN_NONE;
  }
  PyDict_Clear(feDict);
  Py_DECREF(feDict);
  Py_RETURN_NONE;
}

static PyMethodDef XPLMPluginMethods[] = {
  {"XPLMGetMyID", XPLMGetMyIDFun, METH_VARARGS, ""},
  {"XPLMCountPlugins", XPLMCountPluginsFun, METH_VARARGS, ""},
  {"XPLMGetNthPlugin", XPLMGetNthPluginFun, METH_VARARGS, ""},
  {"XPLMFindPluginByPath", XPLMFindPluginByPathFun, METH_VARARGS, ""},
  {"XPLMFindPluginBySignature", XPLMFindPluginBySignatureFun, METH_VARARGS, ""},
  {"XPLMGetPluginInfo", XPLMGetPluginInfoFun, METH_VARARGS, ""},
  {"XPLMIsPluginEnabled", XPLMIsPluginEnabledFun, METH_VARARGS, ""},
  {"XPLMEnablePlugin", XPLMEnablePluginFun, METH_VARARGS, ""},
  {"XPLMDisablePlugin", XPLMDisablePluginFun, METH_VARARGS, ""},
  {"XPLMReloadPlugins", XPLMReloadPluginsFun, METH_VARARGS, ""},
  {"XPLMSendMessageToPlugin", XPLMSendMessageToPluginFun, METH_VARARGS, ""},
  {"XPLMHasFeature", XPLMHasFeatureFun, METH_VARARGS, ""},
  {"XPLMIsFeatureEnabled", XPLMIsFeatureEnabledFun, METH_VARARGS, ""},
  {"XPLMEnableFeature", XPLMEnableFeatureFun, METH_VARARGS, ""},
  {"XPLMEnumerateFeatures", XPLMEnumerateFeaturesFun, METH_VARARGS, ""},
  {"cleanup", cleanup, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
};

static struct PyModuleDef XPLMPluginModule = {
  PyModuleDef_HEAD_INIT,
  "XPLMPlugin",
  NULL,
  -1,
  XPLMPluginMethods,
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC
PyInit_XPLMPlugin(void)
{
  if(!(feDict = PyDict_New())){
    return NULL;
  }
  PyObject *mod = PyModule_Create(&XPLMPluginModule);
  if(mod){
    PyModule_AddIntConstant(mod, "XPLM_MSG_PLANE_CRASHED", XPLM_MSG_PLANE_CRASHED);
    PyModule_AddIntConstant(mod, "XPLM_MSG_PLANE_LOADED", XPLM_MSG_PLANE_LOADED);
    PyModule_AddIntConstant(mod, "XPLM_MSG_AIRPORT_LOADED", XPLM_MSG_AIRPORT_LOADED);
    PyModule_AddIntConstant(mod, "XPLM_MSG_SCENERY_LOADED", XPLM_MSG_SCENERY_LOADED);
    PyModule_AddIntConstant(mod, "XPLM_MSG_AIRPLANE_COUNT_CHANGED", XPLM_MSG_AIRPLANE_COUNT_CHANGED);
    PyModule_AddIntConstant(mod, "XPLM_MSG_PLANE_UNLOADED", XPLM_MSG_PLANE_UNLOADED);
    PyModule_AddIntConstant(mod, "XPLM_MSG_WILL_WRITE_PREFS", XPLM_MSG_WILL_WRITE_PREFS);
    PyModule_AddIntConstant(mod, "XPLM_MSG_LIVERY_LOADED", XPLM_MSG_LIVERY_LOADED);
    PyModule_AddIntConstant(mod, "XPLM_MSG_ENTERED_VR", XPLM_MSG_ENTERED_VR);
    PyModule_AddIntConstant(mod, "XPLM_MSG_EXITING_VR", XPLM_MSG_EXITING_VR);
    PyModule_AddIntConstant(mod, "XPLM_MSG_RELEASE_PLANES", XPLM_MSG_RELEASE_PLANES);
  }
  return mod;
}

