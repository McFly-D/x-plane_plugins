#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdbool.h>

#include <XPLM/XPLMDefs.h>
#include <Widgets/XPWidgetDefs.h>
#include <Widgets/XPUIGraphics.h>
#include "utils.h"
#include "xppythontypes.h"
static PyObject *XPDrawWindowFun(PyObject *self, PyObject *args)
{
  (void) self;
  int inX1, inY1, inX2, inY2, inStyle;
  if(!PyArg_ParseTuple(args, "iiiii", &inX1, &inY1, &inX2, &inY2, &inStyle)){
    return NULL;
  }
  XPDrawWindow(inX1, inY1, inX2, inY2, inStyle);
  Py_RETURN_NONE;
}

static PyObject *XPGetWindowDefaultDimensionsFun(PyObject *self, PyObject *args)
{
  (void) self;
  int inStyle, outWidth, outHeight;
  if(!PyArg_ParseTuple(args, "i", &inStyle)) {
    return NULL;
  }
  XPGetWindowDefaultDimensions(inStyle, &outWidth, &outHeight);
  return Py_BuildValue("ii", outWidth, outHeight);
}

static PyObject *XPDrawElementFun(PyObject *self, PyObject *args)
{
  (void) self;
  int inX1, inY1, inX2, inY2, inStyle, inLit;
  if(!PyArg_ParseTuple(args, "iiiiii", &inX1, &inY1, &inX2, &inY2, &inStyle, &inLit)){
    return NULL;
  }
  XPDrawElement(inX1, inY1, inX2, inY2, inStyle, inLit);
  Py_RETURN_NONE;
}

static PyObject *XPGetElementDefaultDimensionsFun(PyObject *self, PyObject *args)
{
  (void) self;
  int inStyle, outWidth, outHeight, outCanBeLit;
  if(!PyArg_ParseTuple(args, "i", &inStyle)) {
    return NULL;
  }
  XPGetElementDefaultDimensions(inStyle, &outWidth, &outHeight, &outCanBeLit);
  return Py_BuildValue("iii", outWidth, outHeight, outCanBeLit);
}


static PyObject *XPDrawTrackFun(PyObject *self, PyObject *args)
{
  (void) self;
  int inX1, inY1, inX2, inY2, inMin, inMax, inValue, inStyle, inLit;
  if(!PyArg_ParseTuple(args, "iiiiiiiii", &inX1, &inY1, &inX2, &inY2, &inMin, &inMax, &inValue, &inStyle, &inLit)){
    return NULL;
  }
  XPDrawTrack(inX1, inY1, inX2, inY2, inMin, inMax, inValue, inStyle, inLit);
  Py_RETURN_NONE;
}

static PyObject *XPGetTrackDefaultDimensionsFun(PyObject *self, PyObject *args)
{
  (void) self;
  int inStyle, outWidth, outCanBeLit;
  if(!PyArg_ParseTuple(args, "i", &inStyle)) {
    return NULL;
  }
  XPGetTrackDefaultDimensions(inStyle, &outWidth, &outCanBeLit);
  return Py_BuildValue("ii", outWidth, outCanBeLit);
}


static PyObject *XPGetTrackMetricsFun(PyObject *self, PyObject *args)
{
  (void) self;
  int inX1, inY1, inX2, inY2, inMin, inMax, inValue, inStyle;
  int outIsVertical, outDownBtnSize, outDownPageSize, outThumbSize, outUpPageSize, outUpBtnSize;
  if(!PyArg_ParseTuple(args, "iiiiiiii", &inX1, &inY1, &inX2, &inY2, &inMin, &inMax, &inValue, &inStyle)){
    return NULL;
  }
  XPGetTrackMetrics(inX1, inY1, inX2, inY2, inMin, inMax, inValue, inStyle, &outIsVertical, &outDownBtnSize,
                    &outDownPageSize, &outThumbSize, &outUpPageSize, &outUpBtnSize);
  return PyTrackMetrics_New(outIsVertical, outDownBtnSize, outDownPageSize, outThumbSize, outUpPageSize, outUpBtnSize);
}

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  Py_RETURN_NONE;
}




static PyMethodDef XPUIGraphicsMethods[] = {
  {"XPDrawWindow", XPDrawWindowFun, METH_VARARGS, ""},
  {"XPGetWindowDefaultDimensions", XPGetWindowDefaultDimensionsFun, METH_VARARGS, ""},
  {"XPDrawElement", XPDrawElementFun, METH_VARARGS, ""},
  {"XPGetElementDefaultDimensions", XPGetElementDefaultDimensionsFun, METH_VARARGS, ""},
  {"XPDrawTrack", XPDrawTrackFun, METH_VARARGS, ""},
  {"XPGetTrackDefaultDimensions", XPGetTrackDefaultDimensionsFun, METH_VARARGS, ""},
  {"XPGetTrackMetrics", XPGetTrackMetricsFun, METH_VARARGS, ""},
  {"cleanup", cleanup, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
};


static struct PyModuleDef XPUIGraphicsModule = {
  PyModuleDef_HEAD_INIT,
  "XPUIGraphics",
  NULL,
  -1,
  XPUIGraphicsMethods,
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC
PyInit_XPUIGraphics(void)
{
  PyObject *mod = PyModule_Create(&XPUIGraphicsModule);
  if(mod){
    PyModule_AddIntConstant(mod, "xpWindow_Help", xpWindow_Help);
    PyModule_AddIntConstant(mod, "xpWindow_MainWindow", xpWindow_MainWindow);
    PyModule_AddIntConstant(mod, "xpWindow_SubWindow", xpWindow_SubWindow);
    PyModule_AddIntConstant(mod, "xpWindow_Screen", xpWindow_Screen);
    PyModule_AddIntConstant(mod, "xpWindow_ListView", xpWindow_ListView);
    PyModule_AddIntConstant(mod, "xpElement_TextField", xpElement_TextField);
    PyModule_AddIntConstant(mod, "xpElement_CheckBox", xpElement_CheckBox);
    PyModule_AddIntConstant(mod, "xpElement_CheckBoxLit", xpElement_CheckBoxLit);
    PyModule_AddIntConstant(mod, "xpElement_WindowCloseBox", xpElement_WindowCloseBox);
    PyModule_AddIntConstant(mod, "xpElement_WindowCloseBoxPressed", xpElement_WindowCloseBoxPressed);
    PyModule_AddIntConstant(mod, "xpElement_PushButton", xpElement_PushButton);
    PyModule_AddIntConstant(mod, "xpElement_PushButtonLit", xpElement_PushButtonLit);
    PyModule_AddIntConstant(mod, "xpElement_OilPlatform", xpElement_OilPlatform);
    PyModule_AddIntConstant(mod, "xpElement_OilPlatformSmall", xpElement_OilPlatformSmall);
    PyModule_AddIntConstant(mod, "xpElement_Ship", xpElement_Ship);
    PyModule_AddIntConstant(mod, "xpElement_ILSGlideScope", xpElement_ILSGlideScope);
    PyModule_AddIntConstant(mod, "xpElement_MarkerLeft", xpElement_MarkerLeft);
    PyModule_AddIntConstant(mod, "xpElement_Airport", xpElement_Airport);
    PyModule_AddIntConstant(mod, "xpElement_Waypoint", xpElement_Waypoint);
    PyModule_AddIntConstant(mod, "xpElement_NDB", xpElement_NDB);
    PyModule_AddIntConstant(mod, "xpElement_VOR", xpElement_VOR);
    PyModule_AddIntConstant(mod, "xpElement_RadioTower", xpElement_RadioTower);
    PyModule_AddIntConstant(mod, "xpElement_AircraftCarrier", xpElement_AircraftCarrier);
    PyModule_AddIntConstant(mod, "xpElement_Fire", xpElement_Fire);
    PyModule_AddIntConstant(mod, "xpElement_MarkerRight", xpElement_MarkerRight);
    PyModule_AddIntConstant(mod, "xpElement_CustomObject", xpElement_CustomObject);
    PyModule_AddIntConstant(mod, "xpElement_CoolingTower", xpElement_CoolingTower);
    PyModule_AddIntConstant(mod, "xpElement_SmokeStack", xpElement_SmokeStack);
    PyModule_AddIntConstant(mod, "xpElement_Building", xpElement_Building);
    PyModule_AddIntConstant(mod, "xpElement_PowerLine", xpElement_PowerLine);
    PyModule_AddIntConstant(mod, "xpElement_CopyButtons", xpElement_CopyButtons);
    PyModule_AddIntConstant(mod, "xpElement_CopyButtonsWithEditingGrid", xpElement_CopyButtonsWithEditingGrid);
    PyModule_AddIntConstant(mod, "xpElement_EditingGrid", xpElement_EditingGrid);
    PyModule_AddIntConstant(mod, "xpElement_ScrollBar", xpElement_ScrollBar);
    PyModule_AddIntConstant(mod, "xpElement_VORWithCompassRose", xpElement_VORWithCompassRose);
    PyModule_AddIntConstant(mod, "xpElement_Zoomer", xpElement_Zoomer);
    PyModule_AddIntConstant(mod, "xpElement_TextFieldMiddle", xpElement_TextFieldMiddle);
    PyModule_AddIntConstant(mod, "xpElement_LittleDownArrow", xpElement_LittleDownArrow);
    PyModule_AddIntConstant(mod, "xpElement_LittleUpArrow", xpElement_LittleUpArrow);
    PyModule_AddIntConstant(mod, "xpElement_WindowDragBar", xpElement_WindowDragBar);
    PyModule_AddIntConstant(mod, "xpElement_WindowDragBarSmooth", xpElement_WindowDragBarSmooth);
    PyModule_AddIntConstant(mod, "xpTrack_ScrollBar", xpTrack_ScrollBar);
    PyModule_AddIntConstant(mod, "xpTrack_Slider", xpTrack_Slider);
    PyModule_AddIntConstant(mod, "xpTrack_Progress", xpTrack_Progress);
  }

  return mod;
}
 

