/**
 * util.c : utility functions for pyalpm
 *
 *  Copyright 2008 Imanol Celaya <ilcra1989@gmail.com>
 *  Copyright 2011 Rémy Oudompheng <remy@archlinux.org>
 *
 *  This file is part of pyalpm.
 *
 *  pyalpm is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  pyalpm is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with pyalpm.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "util.h"

static unsigned short init = 0;

/** Converts a Python list of strings to an alpm_list_t linked list.
 * return 0 on success, -1 on failure
 */
int pylist_string_to_alpmlist(PyObject *list, alpm_list_t* *result)
{
  alpm_list_t *ret = NULL;
  PyObject *iterator = PyObject_GetIter(list);
  PyObject *item;

  if(iterator == NULL) {
    PyErr_SetString(PyExc_TypeError, "object is not iterable");
    return -1;
  }

  while((item = PyIter_Next(iterator)))
  {
    if (PyBytes_Check(item)) {
      ret = alpm_list_add(ret, strdup(PyBytes_AS_STRING(item)));
    } else if (PyUnicode_Check(item)) {
      PyObject* utf8 = PyUnicode_AsUTF8String(item);
      ret = alpm_list_add(ret, strdup(PyBytes_AS_STRING(utf8)));
      Py_DECREF(utf8);
    } else {
      PyErr_SetString(PyExc_TypeError, "list must contain only strings");
      FREELIST(ret);
      return -1;
    }
    Py_DECREF(item);
  }
  Py_DECREF(iterator);

  *result = ret;
  return 0;
}

PyObject* pyobject_from_string(void *s) {
  return Py_BuildValue("s", (char*)s);
}

PyObject* alpmlist_to_pylist(alpm_list_t *prt, pyobjectbuilder pybuilder)
{
  PyObject *output, *stritem;
  alpm_list_t *tmp;

  output = PyList_New(0);
  if(output == NULL) {
    PyErr_SetString(PyExc_RuntimeError, "unable to create list object");
    return NULL;
  }

  for(tmp = prt; tmp; tmp = alpm_list_next(tmp)) {
    stritem = pybuilder(alpm_list_getdata(tmp));
    PyList_Append(output, stritem);
    Py_DECREF(stritem);
  }

  return output;
}

/*alpm_list_t related functions*/
void add_alpm_list_t(alpm_list_t *prt)
{
  alpm_list_t *new;
  new = (alpm_list_t*) malloc(sizeof(alpm_list_t));
  
  prt->next = new;
  new->prev = prt;
}

void remove_alpm_list_t(alpm_list_t *prt)
{
  alpm_list_t *old;
  
  old = prt->prev;
  old->next = NULL;
  
  free(prt->data);
  free(prt);
}

void set_init(unsigned short i) {
  init = i;
}

unsigned short check_init(void)
{
  switch(init)
  {
    case 0:
      return 0;
    case 1:
      return 1;
   default:
      return -1;
  }
}

/* vim: set ts=2 sw=2 et: */
