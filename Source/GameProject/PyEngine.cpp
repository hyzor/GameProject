#include "PyEngine.h"
#include <iostream>

static PyEngine* GetPyEnginePointer()
{
	return (PyEngine*)PyCapsule_Import("PyEngine._C_API", 0);
}

static PyObject* WrapPyEnginePrint(PyObject* self, PyObject* args)
{
	PyEngine* lpPyEngine = GetPyEnginePointer();
	if(!lpPyEngine)
	{
		std::cout << "Failed to get the game pointer" << std::endl;
		return NULL;
	}

	const char* lpSrc;
	if(!PyArg_ParseTuple(args, "s", &lpSrc))
	{
		PyErr_SetString(PyExc_RuntimeError, "PyEngine.Print wants a single string argument");
		return NULL;
	}

	lpPyEngine->Print(std::string(lpSrc));

	Py_INCREF(Py_None);

	return Py_None;
}

static PyObject* PyEngineNotifyAfter(PyObject* self, PyObject* args)
{
	PyEngine* lpPyEngine = GetPyEnginePointer();
	if(!lpPyEngine)
	{
		std::cout << "Failed to get the game pointer" << std::endl;
		return NULL;
	}

	float lTime;
	PyObject* lpFunc;
	PyObject* PyArgs;
	if(!PyArg_ParseTuple(args, "fOO", &lTime, &lpFunc, &PyArgs))
	{
		return NULL;
	}
	
	if(!PyCallable_Check(lpFunc))
	{
		return NULL;
	}

	PyObject* lpArgs = NULL;
	if(PyArgs != Py_None)
	{
		lpArgs = PyList_New(1);
		PyList_SetItem(lpArgs, 0, PyArgs);
	}

	lpPyEngine->AddTimerEvent(TimerEvent(float(lTime), lpFunc, lpArgs));
	
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* PyEngineNotifyWhen(PyObject* self, PyObject* args)
{
	PyEngine* lpPyEngine = GetPyEnginePointer();
	if(!lpPyEngine)
	{
		std::cout << "Failed to get the game pointer" << std::endl;
		return NULL;
	}

	const char* status;
	PyObject* lpFunc;
	PyObject* PyArgs;
	if (!PyArg_ParseTuple(args, "sOO", &status, &lpFunc, &PyArgs))
	{
		if(PyErr_Occurred())
			PyErr_Print();
		return NULL;
	}

	if(!PyCallable_Check(lpFunc))
	{
		return NULL;
	}

	std::string s_copy = status;

	Py_INCREF(lpFunc);

	PyObject* lpArgs = NULL;
	if(PyArgs != Py_None)
	{
		lpArgs = PyList_New(1);
		PyList_SetItem(lpArgs, 0, PyArgs);
	}

	lpPyEngine->AddStatusEvent(StatusEvent(s_copy, lpFunc, lpArgs));
	
	Py_INCREF(Py_None);
	return Py_None;
}

static PyMethodDef PyEngineMethods[] =
{
	{ "Print",			WrapPyEnginePrint,		METH_VARARGS,	"Use PyEngine to print a message" },
	{ "NotifyAfter",	PyEngineNotifyAfter,	METH_VARARGS,	"Notify after a specific amount of time" },
	{ "NotifyWhen",		PyEngineNotifyWhen,		METH_VARARGS,	"Notify when a specific trigger has been met" },
	{ NULL,				NULL,					0,				NULL },
};

PyEngine* PyEngine::mInstance = 0;

PyEngine::PyEngine()
	: mModule(NULL)
{
}

PyEngine::~PyEngine()
{
	Py_XDECREF(this->mModule);
}

PyEngine* PyEngine::GetInstance()
{
	if(!mInstance)
	{
		mInstance = new PyEngine();
	}
	return mInstance;
}

HRESULT PyEngine::Initialize()
{
	if(!InitScriptInterface())
	{
		if(PyErr_Occurred())
			PyErr_Print();
		return E_FAIL;
	}

	if(!mModule)
	{
		if(PyErr_Occurred())
			PyErr_Print();
		return E_FAIL;
	}

	return S_OK;
}

void PyEngine::ShutDown()
{
	Py_Finalize();
	if(mInstance)
		delete mInstance;
}

HRESULT PyEngine::InitScriptInterface()
{
	Py_Initialize();

	mModule = Py_InitModule("PyEngine", PyEngineMethods);

	PyObject* capsule = PyCapsule_New((void*) this, "PyEngine._C_API", NULL);
	if (NULL == capsule)
	{
		std::cout << "Failed to capsule PyEngine module" << std::endl;
		return E_FAIL;
	}

	PyModule_AddObject(mModule, "_C_API", capsule);

	return S_OK;
}

HRESULT PyEngine::LoadModule(std::string scriptName)
{
	if(mCurrScript != scriptName)
	{
		mCurrScript = scriptName;
		PyObject* lScriptName = PyString_FromString(mCurrScript.c_str());
		if(!lScriptName) // Standard koll för null, så att vårt program inte kraschar
		{
			return E_FAIL;
		}
		
		this->mModule = PyImport_Import(lScriptName);
		if(!mModule)
		{
			PyErr_Print();
			return E_FAIL;
		}
		Py_DECREF(lScriptName); // Plocka bort referensen av vår python string
	}
	return S_OK;
}

PyObject* PyEngine::GetFunction(const char* funcName)
{
	PyObject* lpFunction = PyObject_GetAttrString(mModule, funcName);

	if(!lpFunction || (!PyCallable_Check(lpFunction)))
	{
		if(PyErr_Occurred())
			PyErr_Print();

		std::cout << "Can not find " << funcName << " or its not a callable function" << std::endl;
		Py_XDECREF(lpFunction);
		lpFunction = NULL;
	}

	return lpFunction;
}

void PyEngine::Print(std::string msg)
{
	std::cout << "PyEngine: " << msg << std::endl;
}

void PyEngine::AddTimerEvent(TimerEvent ev)
{
	mTimer.push_back(ev);
}

void PyEngine::AddStatusEvent(StatusEvent ev)
{
	mStatus.push_back(ev);
}

bool PyEngine::CheckReturns() const
{
	return mFuncReturns.size() > 0;
}

void PyEngine::Update(float dt)
{
	//NotifyAfter
	for(UINT i(0); i != mTimer.size(); ++i)
	{
		mTimer[i].mTime -= dt;

		if(mTimer[i].mTime <= 0) //Om tiden har gått ut, så anropa funktionen och plocka bort från vectorn
		{
			if(mTimer[i].mArgs)
				CallFunction(mTimer[i].mFunc, PyList_AsTuple(mTimer[i].mArgs));
			else
				CallFunction(mTimer[i].mFunc, NULL);
			mTimer.erase(mTimer.begin()+i);
		}
	}

	//NotifyWhen
	if(mStatus.size() > 0)
	{
		for(UINT i(0); i != mStatus.size(); ++i)
		{
			if(mStatus[i].mArgs)
				mStatus[i].mReturns = CallFunction(mStatus[i].mFunc, PyList_AsTuple(mStatus[i].mArgs));
			else
				mStatus[i].mReturns = CallFunction(mStatus[i].mFunc, NULL);

			if(mStatus[i].mReturns != Py_None && mStatus[i].mReturns != NULL)
				mFuncReturns.push_back(mStatus[i].mReturns);
		}
		mStatus.clear();
	}
}

void PyEngine::MakeTuple(PyObject* &args)
{
	int numArgs = PyObject_Length(args);
	if(numArgs == -1)
		numArgs = 1;
	else if(numArgs == 2 && PyString_Check(args))
		numArgs--;

	PyObject* tArgs = PyTuple_New(numArgs);
	for(int i(0); i != numArgs; ++i)
	{
		PyTuple_SetItem(tArgs, i, args);
	}
	if(tArgs)
		args = tArgs;
}

PyObject* PyEngine::CallFunction(PyObject* func, PyObject* args)
{
	if(args != NULL && !PyTuple_Check(args))
		this->MakeTuple(args);

	PyObject* lpReturns;
	if(args)
		lpReturns = PyObject_CallObject(func, args);
	else
		lpReturns = PyObject_CallObject(func, NULL);

	if(!lpReturns)
	{
		std::cout << "Something went wrong with calling the function!" << std::endl;
		PyErr_Print();
		return NULL;
	}

	return lpReturns;
}

void PyEngine::ConvertInts(std::vector<int> &r_vec)
{
	for(int i(0); i != mFuncReturns.size(); ++i)
	{
		int lNumReturns = PyTuple_Size(mFuncReturns[i]);

		if(PyInt_Check(mFuncReturns[i]))
		{
			r_vec.push_back(PyInt_AsLong(mFuncReturns[i]));
			continue;
		}

		if(lNumReturns > 0)
		{
			for(int j(0); j != lNumReturns; ++j)
			{
				PyObject* item = PyTuple_GetItem(mFuncReturns[i], j);
				if(!PyInt_Check(item))
				{
					PyErr_Print();
					continue;
				}
				r_vec.push_back(PyInt_AsLong(item));
			}
		}
	}
}

void PyEngine::ConvertStrings(std::vector<std::string> &r_vec)
{
	for(int i(0); i != mFuncReturns.size(); ++i)
	{
		int lNumReturns = PyTuple_Size(mFuncReturns[i]);

		if(PyString_Check(mFuncReturns[i])) 
		{
			r_vec.push_back(PyString_AsString(mFuncReturns[i]));
			continue;
		}

		if(lNumReturns > 0)
		{
			for(int j(0); j != lNumReturns; ++j)
			{
				PyObject* item = PyTuple_GetItem(mFuncReturns[i], j);
				if(!PyString_Check(item))
				{
					PyErr_Print();
					continue;
				}
				r_vec.push_back(PyString_AsString(item));
			}
		}
	}
}

void PyEngine::ConvertDoubles(std::vector<double> &r_vec)
{
	for(int i(0); i != mFuncReturns.size(); ++i)
	{
		int lNumReturns = PyTuple_Size(mFuncReturns[i]);

		if(PyFloat_Check(mFuncReturns[i]))
		{
			r_vec.push_back(PyFloat_AsDouble(mFuncReturns[i]));
			continue;
		}

		if(lNumReturns > 0)
		{
			for(int j(0); j != lNumReturns; ++j)
			{
				PyObject* item = PyTuple_GetItem(mFuncReturns[i], j);
				if(!PyFloat_Check(item))
				{
					PyErr_Print();
					continue;
				}
				r_vec.push_back(PyFloat_AsDouble(item));
			}
		}
	}
}