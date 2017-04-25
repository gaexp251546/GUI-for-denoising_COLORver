#include "MyForm.h"


using namespace System;
using namespace 測試視窗;



[STAThread]
int main(array<System::String ^> ^args)
{
	//建立任何控制項之前，先啟用windows的視覺化效果
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);

	//建立主視窗並執行
	Application::Run(gcnew MyForm());

	Console::WriteLine(L"Hello World");

	return 0;
}
