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

// Ytterligare en Singleton, den h�r beh�vde d�remot verkligen vara det, eftersom funktionerna som kan kallas fr�n python beh�ver vara static
class PyEngine 
{
public:
	static PyEngine* GetInstance();
	HRESULT Initialize();
	void ShutDown();

	PyObject* CallFunction(PyObject* func, PyObject* args); // Kan kalla p� funktioner i python
	PyObject* GetFunction(const char* funcName); // Returerar en PyObject* med funktionen i givet namnet p� funktionen
	PyObject* GetModule() const { return this->mModule; }

	void Print(std::string msg); // Aldrig anv�nt denna, men den �r r�tt sj�lvf�rklarande

	void Tick(float dt); // En update f�r motorn.. Kan d�pas om till update f�r att vara konsekvent

	HRESULT LoadModule(std::string scriptName); // M�ste kallas p� med r�tt namn p� scriptet som ska k�ras innan ett nytt script ska k�ras
	bool CheckReturns() const; // Anv�nds f�r att se om det ligger n�got i returvektorn
	void ClearReturns() { this->mFuncReturns.clear(); } // Rensar ur tidigare n�mnda vektor, ska kallas p� n�r alla argument h�mtats ur den

	// De tre funktionerna h�r ska kallas p� f�r att fylla upp eventuella vektorer med saker som C++ f�rst�r ifr�n returvektorn
	void ConvertInts(std::vector<int> &r_vec);
	void ConvertStrings(std::vector<std::string> &r_vec);
	void ConvertDoubles(std::vector<double> &r_vec);

	void AddTimerEvent(TimerEvent ev); // Dessa beh�ver vara public s� att static-funktionerna kan komma �t dem
	void AddStatusEvent(StatusEvent ev);

private:
	static PyEngine* mInstance;
	PyEngine();
	PyEngine(PyEngine const&) {}
	void operator=(PyEngine const&) {}
	~PyEngine();

	PyObject* mModule;

	std::string mCurrScript; // Variabel med namnet p� det script som k�rs av motorn atm, anv�ndbart f�r att slippa on�diga byten till samma script

	std::vector<TimerEvent> mTimer; // Vektor med alla timer events i, denna fylls p� n�r man kallar p� NotifyAfter i Python och kan inte kalla p� funktioner med returer
	std::vector<StatusEvent> mStatus; // Vektor med alla status events i, denna fylls p� n�r man kallar p� NotifyWhen i Python och kan kalla p� funktioner med eller utan returer
	std::vector<PyObject*> mFuncReturns; // N�r de funktioner som mStatus kallar p� returerar n�got s� l�ggs returen in i ett PyObject* h�r
	
	HRESULT InitScriptInterface(); // K�rs i initialize
	void MakeTuple(PyObject* &args); // Anv�nds av CallFunction n�r argumentet man skickar in kommer fr�n CreateArg, d� dessa inte �r tupler fr�n b�rjan och Python <3 Tupler

public:
	// Extremt fulhaxade funktioner f�r att g�ra om parametrar till Python-lingo s� att de kan skickas in i CallFunction
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
Exempel f�r att kalla p� funktion i Python utanf�r motorn:
PyEngine::GetInstance()->CallFunction(
	PyEngine::GetInstance()->GetFunction("FunktionsNamn"),
	PyEngine::GetInstance()->CreateArg(var1, var2));

Exempel f�r att f� tillbaka returer i en loop ifr�n Python:
std::vector<std::string> vNames;
std::vector<int> vPositions;
PyEngine::GetInstance()->Tick( dt ); <- Viktigt att kalla p� denna innan man tror att man ska f� returer eftersom de skapas i Tick av NotifyWhen
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