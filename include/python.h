/*
 * python.h: python-based face recognition
 */

#ifndef __PYTHON_H
#define __PYTHON_H

#include <python3.8/Python.h>
#include <stdlib.h>

PyObject *module,
         *dict,

         *module_pil,
         *module_np,

         *func_encodings,
         *func_cmpfaces,

         *str_modulename,
         *str_imagepath,
         *str_testimage,

         *args_encodings,
         *args_loadtest,
         *args_testenc,
         *args_cmpfaces,

         *out_loadimage,
         *out_encodings,
         *out_loadtest,
         *out_testenc,
         *out_cmpfaces;

extern char _binary_img_user_jpg_start;
extern char _binary_img_user_jpg_end;

/*
 * This is wasteful but everything else I've tried segfaults
 */
PyObject *load_image(char *filename){
  PyObject *str_pil, *str_np, *str_filename, *args_pil, *args_np, *im;
  PyObject *dict_pil, *dict_np, *func_pil, *func_np;

  str_pil = PyUnicode_FromString("PIL.Image");
  str_np = PyUnicode_FromString("numpy");
  str_filename = PyUnicode_FromString(filename);

  module_pil = PyImport_Import(str_pil);
  module_np = PyImport_Import(str_np);

  dict_pil = PyModule_GetDict(module_pil);
  dict_np = PyModule_GetDict(module_np);

  func_pil = PyDict_GetItemString(dict_pil, "open");
  func_np = PyDict_GetItemString(dict_np, "array");

  args_pil = PyTuple_Pack(1, str_filename);

  im = PyObject_CallObject(func_pil, args_pil);

  args_np = PyTuple_Pack(1, im);

  return PyObject_CallObject(func_np, args_np);
}

/*
 * TODO: Error checking
 */
int python_init(){
  FILE *up = fopen("/tmp/user_image", "w");
  char *uc;
  for(uc=&_binary_img_user_jpg_start;
      uc!=&_binary_img_user_jpg_end;
      uc++
     ){
    fprintf(up, "%c", *uc);
  }
  fclose(up);

  Py_Initialize();

  str_modulename = PyUnicode_FromString("face_recognition");
  str_imagepath = PyUnicode_FromString("/tmp/user_image");
  str_testimage = PyUnicode_FromString("/tmp/faceid_lock.png");

  module = PyImport_Import(str_modulename);
  dict = PyModule_GetDict(module);

  func_encodings = PyDict_GetItemString(dict, "face_encodings");
  func_cmpfaces = PyDict_GetItemString(dict, "compare_faces");

  out_loadimage = load_image("/tmp/user_image");

  args_encodings = PyTuple_Pack(1, out_loadimage);
  out_encodings = PyObject_CallObject(func_encodings, args_encodings);

  args_loadtest = PyTuple_Pack(1, str_testimage);

  Py_DECREF(str_modulename);
  Py_DECREF(str_imagepath);

  remove("/tmp/user_image");

  return 0;
}

int python_scan(){
  int out = 0;

  out_loadtest = load_image("/tmp/faceid_lock.png");
  if(out_loadtest == NULL){
    return out;
  }

  args_testenc = PyTuple_Pack(1, out_loadtest);
  out_testenc = PyObject_CallObject(func_encodings, args_testenc);

  if(out_testenc != NULL && PyList_GetItem(out_testenc, 0) != NULL){
    args_cmpfaces = PyTuple_Pack(2, out_encodings, PyList_GetItem(out_testenc, 0));
    out_cmpfaces = PyObject_CallObject(func_cmpfaces, args_cmpfaces);

    if(out_cmpfaces != NULL){
      for(int i=0;i<PyList_Size(out_cmpfaces);i++){
        if(PyObject_RichCompareBool(PyList_GetItem(out_cmpfaces, i), Py_True, Py_EQ)){
          out = 1;
        }
      }
    }
  }

  return out;
}

void python_shutdown(){
  Py_DECREF(module);
  Py_DECREF(dict);

  Py_DECREF(func_encodings);
  Py_DECREF(func_cmpfaces);

  Py_DECREF(args_encodings);
  Py_DECREF(args_loadtest);
  Py_DECREF(args_testenc);
  Py_DECREF(args_cmpfaces);

  Py_DECREF(out_loadimage);
  Py_DECREF(out_encodings);
  Py_DECREF(out_loadtest);
  Py_DECREF(out_testenc);

  Py_DECREF(str_testimage);

  Py_Finalize();
}

#endif
