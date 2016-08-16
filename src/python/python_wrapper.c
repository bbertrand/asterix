 /*
 *  Copyright (c) 2013 Croatia Control Ltd. (www.crocontrol.hr)
 *
 *  This file is part of Asterix.
 *
 *  Asterix is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Asterix is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Asterix.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * AUTHORS: Damir Salantic, Croatia Control Ltd.
 *
 */

#include "python_parser.h"

static int bInitialized = 0;

//static python_definitions* pListOfDefinitions = NULL;

//static python_data* getList(python_data* p, PyObject* head);

static char* ini_filename;
PyObject *my_callback = NULL;

PyObject*
say_hello(PyObject* self, PyObject* args, PyObject *kwargs)
{
    const char* name;

    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;

	/* Time to call the callback */
	PyObject *arglist;
	PyObject *result;
	arglist = Py_BuildValue("(s)", "Hello world!!!");
	result = PyObject_CallObject(my_callback, arglist);
	Py_DECREF(arglist);
	if (result == NULL)
	    return NULL; /* Pass error back */
	/// use result...
	Py_DECREF(result);

    Py_RETURN_NONE;
}

PyObject*
init(PyObject* self, PyObject* args, PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, "s", &ini_filename))
        return Py_BuildValue("i", -1);

	int ret = python_init(ini_filename);
	if (ret == 0)
	{
		bInitialized = 1;
        return Py_BuildValue("i", 0);
	}

    return Py_BuildValue("i", -2);
}


PyObject*
describe(PyObject* self, PyObject* args, PyObject *kwargs)
{
    int category;
    const char* item = NULL;
    const char* field = NULL;
    const char* value = NULL;

    Py_ssize_t TupleSize = PyTuple_Size(args);
    if (TupleSize == 1)
    {
        if (!PyArg_ParseTuple(args, "i", &category))
            return NULL;
    }
    else if (TupleSize == 2)
    {
        if (!PyArg_ParseTuple(args, "is", &category, &item))
            return NULL;
    }
    else if (TupleSize == 3)
    {
        if (!PyArg_ParseTuple(args, "iss", &category, &item, &field))
            return NULL;
    }
    else
    {
        if (!PyArg_ParseTuple(args, "isss", &category, &item, &field, &value))
            return NULL;
    }

	return python_describe(category, item, field, value);
}


PyObject*
parse(PyObject* self, PyObject* args, PyObject *kwargs)
{
/*
	const char* filename;
	if (!PyArg_ParseTuple(args, "s", &filename))
	        return NULL;

	asterix_start(ini_filename, filename);
	return NULL;
*/
    const char* data;
    Py_ssize_t len;
    //int len;

    if (!PyArg_ParseTuple(args, "s#", &data, &len))
        return NULL;

	if (!bInitialized)
	{
	    printf("Not initialized!");
	    return NULL;
	}

	PyObject *lstBlocks = python_parse((const unsigned char*) data, len);
	if (lstBlocks == NULL)
		return PyList_New(0);
	return lstBlocks;
}


PyObject *
set_callback(PyObject* self, PyObject *args)
{
    PyObject *result = NULL;
    PyObject *temp;

    if (PyArg_ParseTuple(args, "O:set_callback", &temp)) {
        if (!PyCallable_Check(temp)) {
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }
        Py_XINCREF(temp);         /* Add a reference to new callback */
        Py_XDECREF(my_callback);  /* Dispose of previous callback */
        my_callback = temp;       /* Remember new callback */
        /* Boilerplate to return "None" */
        Py_INCREF(Py_None);
        result = Py_None;
    }
    return result;
}
