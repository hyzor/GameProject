#ifndef PYENGINE_H
#define PYENGINE_H

#include <string>
#include <vector>
#include <Windows.h>
#include <typeinfo>

#if defined(_DEBUG)
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif

typedef struct _object PyObject;

struct TimerEvent
{
	float mTime;
	PyObject* mFunc;
	PyObject* mArgs;

	TimerEvent(float time, PyObject* func, PyObject* args)
	{
		this->mTime = time;
		this->mFunc = func;
		this->mArgs = args;
	}
};

struct StatusEvent
{
	std::string mStatus;
	PyObject* mFunc;
	PyObject* mArgs;
	PyObject* mReturns;

	StatusEvent(std::string status, PyObject* func, PyObject* args)
	{
		this->mStatus = status;
		this->mFunc = func;
		this->mArgs = args;
	}
};

// Ytterligare en Singleton, den här behövde däremot verkligen vara det, eftersom funktionerna som kan kallas från python behöver vara static
class PyEngine 
{
public:
	static PyEngine* GetInstance();
	HRESULT Initialize();
	void ShutDown();

	PyObject* CallFunction(PyObject* func, PyObject* args); // Kan kalla på funktioner i python
	PyObject* GetFunction(const char* funcName); // Returerar en PyObject* med funktionen i givet namnet på funktionen
	PyObject* GetModule() const { return this->mModule; }

	void Print(std::string msg); // Aldrig använt denna, men den är rätt självförklarande

	void Tick(float dt); // En update för motorn.. Kan döpas om till update för att vara konsekvent

	HRESULT LoadModule(std::string scriptName); // Måste kallas på med rätt namn på scriptet som ska köras innan ett nytt script ska köras
	bool CheckReturns() const; // Används för att se om det ligger något i returvektorn
	void ClearReturns() { this->mFuncReturns.clear(); } // Rensar ur tidigare nämnda vektor, ska kallas på när alla argument hämtats ur den

	// De tre funktionerna här ska kallas på för att fylla upp eventuella vektorer med saker som C++ förstår ifrån returvektorn
	void ConvertInts(std::vector<int> &r_vec);
	void ConvertStrings(std::vector<std::string> &r_vec);
	void ConvertDoubles(std::vector<double> &r_vec);

	void AddTimerEvent(TimerEvent ev); // Dessa behöver vara public så att static-funktionerna kan komma åt dem
	void AddStatusEvent(StatusEvent ev);

private:
	static PyEngine* mInstance;
	PyEngine();
	PyEngine(PyEngine const&) {}
	void operator=(PyEngine const&) {}
	~PyEngine();

	PyObject* mModule;

	std::string mCurrScript; // Variabel med namnet på det script som körs av motorn atm, användbart för att slippa onödiga byten till samma script

	std::vector<TimerEvent> mTimer; // Vektor med alla timer events i, denna fylls på när man kallar på NotifyAfter i Python och kan inte kalla på funktioner med returer
	std::vector<StatusEvent> mStatus; // Vektor med alla status events i, denna fylls på när man kallar på NotifyWhen i Python och kan kalla på funktioner med eller utan returer
	std::vector<PyObject*> mFuncReturns; // När de funktioner som mStatus kallar på returerar något så läggs returen in i ett PyObject* här
	
	HRESULT InitScriptInterface(); // Körs i initialize
	void MakeTuple(PyObject* &args); // Används av CallFunction när argumentet man skickar in kommer från CreateArg, då dessa inte är tupler från början och Python <3 Tupler

public:
	// Extremt fulhaxade funktioner för att göra om parametrar till Python-lingo så att de kan skickas in i CallFunction
	template<typename A>
	PyObject* CreateArg(A arg1);
	template<typename A, typename B>
	PyObject* CreateArg(A arg1, B arg2);
	template<typename A, typename B, typename C>
	PyObject* CreateArg(A arg1, B arg2, C arg3);
};

template<typename A>
PyObject* PyEngine::CreateArg(A arg1)
{
	std::string types = "";
	// 1
	if(typeid(arg1) == typeid(int))
		types += "i";
	else if(typeid(arg1) == typeid(const char*))
		types += "s";
	else if(typeid(arg1) == typeid(double))
		types += "d";
	return Py_BuildValue(types.c_str(), arg1);
}

template<typename A, typename B>
PyObject* PyEngine::CreateArg(A arg1, B arg2)
{
	std::string types = "";
	// 1
	if(typeid(arg1) == typeid(int))
		types += "i";
	else if(typeid(arg1) == typeid(const char*))
		types += "s";
	else if(typeid(arg1) == typeid(double))
		types += "d";
	// 2
	if(typeid(arg2) == typeid(int))
		types += "i";
	else if(typeid(arg2) == typeid(const char*))
		types += "s";
	else if(typeid(arg2) == typeid(double))
		types += "d";
	return Py_BuildValue(types.c_str(), arg1, arg2);
}

template<typename A, typename B, typename C>
PyObject* PyEngine::CreateArg(A arg1, B arg2, C arg3)
{
	std::string types = "";
	// 1
	if(typeid(arg1) == typeid(int))
		types += "i";
	else if(typeid(arg1) == typeid(const char*))
		types += "s";
	else if(typeid(arg1) == typeid(double))
		types += "d";
	// 2
	if(typeid(arg2) == typeid(int))
		types += "i";
	else if(typeid(arg2) == typeid(const char*))
		types += "s";
	else if(typeid(arg2) == typeid(double))
		types += "d";
	// 3
	if(typeid(arg3) == typeid(int))
		types += "i";
	else if(typeid(arg3) == typeid(const char*))
		types += "s";
	else if(typeid(arg3) == typeid(double))
		types += "d";

	return Py_BuildValue(types.c_str(), arg1, arg2, arg3);
}
#endif

/* 
Exempel för att kalla på funktion i Python utanför motorn:
PyEngine::GetInstance()->CallFunction(
	PyEngine::GetInstance()->GetFunction("FunktionsNamn"),
	PyEngine::GetInstance()->CreateArg(var1, var2));

Exempel för att få tillbaka returer i en loop ifrån Python:
std::vector<std::string> vNames;
std::vector<int> vPositions;
PyEngine::GetInstance()->Tick( dt ); <- Viktigt att kalla på denna innan man tror att man ska få returer eftersom de skapas i Tick av NotifyWhen
if(PyEngine::GetInstance()->CheckReturns())
{
	PyEngine::GetInstance()->ConvertInts(vPositions);
	PyEngine::GetInstance()->ConvertStrings(vNames);
	PyEngine::GetInstance()->ClearReturns();
	this->mName = vNames[0];
	XMFLOAT3 Pos = XMFLOAT3(vPositions[0], vPositions[1], vPositions[2]);
	vNames.clear();
	vPositions.clear();
}
*/