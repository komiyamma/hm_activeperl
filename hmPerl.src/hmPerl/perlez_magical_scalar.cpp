/*
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#include <windows.h>
#include <tchar.h>

#include "convert_string.h"
#include "perlez_magical_scalar.h"
#include "hidemaruexe_export.h"
#include "output_debugstream.h"
#include "self_dll_info.h"
#include "dllfunc_interface.h"
#include "dllfunc_interface_internal.h"

#include "hm_original_encode_mapfunc.h"

// シンボルにアクセスする際にここに直前のものがかならずストックされる。
wstring CPerlEzMagicalScalar::stocked_macro_var_simbol = L"";

// LPCSTRを返すので実体を残すため、必ず、utf8_getvarofreturn.data()をリターンする。
string CPerlEzMagicalScalar::utf8_getvarofreturn = "";

string utf8_lastEvalResult = "";
string utf8_lastHmEncodeResult = "";
string utf8_lastHmLoadFileName = "";

LPCSTR CPerlEzMagicalScalar::GetHmComposedMagicScalarFunctions(LPVOID obj, LPCSTR p_utf8_SelfName)
{
	utf8_getvarofreturn.clear();

	string utf8_SelfName = p_utf8_SelfName;

	if (utf8_SelfName == szMagicalVarVersion) {
		utf8_getvarofreturn = Hm::version();
	}
	else if (utf8_SelfName == szMagicalVarEditTotalText) {
		utf8_getvarofreturn = Hm::Edit::Get::TotalText();
	}
	else if (utf8_SelfName == szMagicalVarEditSelectedText) {
		utf8_getvarofreturn = Hm::Edit::Get::SelectedText();
	}
	else if (utf8_SelfName == szMagicalVarEditLineText) {
		utf8_getvarofreturn = Hm::Edit::Get::LineText();
	}
	else if (utf8_SelfName == szMagicalVarEditCursorPos) {
		utf8_getvarofreturn = Hm::Edit::Get::CursorPos();
	}
	else if (utf8_SelfName == szMagicalVarEditCursorPosFromMousePos) {
		utf8_getvarofreturn = Hm::Edit::Get::CursorPosFromMousePos();
	}
	// hm->Macro->Var(...)へと読み取りの場合
	else if (utf8_SelfName == szMagicalVarMacroVarSimbol) {
		utf8_getvarofreturn = Hm::Macro::Get::VarSimbol();
	}

	// hm->Macro->Evalの結果
	else if (utf8_SelfName == szMagicalVarMacroEvalResult) {
		string ret = utf8_lastEvalResult.data();
		utf8_lastEvalResult.clear();
		utf8_getvarofreturn = ret;
	}

	// hm->File->HmEncodeの結果
	else if (utf8_SelfName == szMagicalVarFileHmEncodeResult) {
		string ret = utf8_lastHmEncodeResult.data();
		utf8_lastHmEncodeResult.clear();
		utf8_getvarofreturn = ret;
	}

	// hm->File->ReadAllTextの結果
	else if (utf8_SelfName == szMagicalVarFileLoadResult) {
		utf8_getvarofreturn = CPerlEzMagicalScalar::Hm::File::Get::ReadAllText();
	}

	return utf8_getvarofreturn.data();
}

string CPerlEzMagicalScalar::Hm::version() {
	wstring utf16_Text = to_wstring(CHidemaruExeExport::hm_version);
	return utf16_to_utf8(utf16_Text);
}

string CPerlEzMagicalScalar::Hm::Edit::Get::TotalText() {
	wstring utf16_Text = CHidemaruExeExport::GetTotalText();
	return utf16_to_utf8(utf16_Text);
}

string CPerlEzMagicalScalar::Hm::Edit::Get::SelectedText() {
	wstring utf16_Text = CHidemaruExeExport::GetSelectedText();
	return utf16_to_utf8(utf16_Text);
}

string CPerlEzMagicalScalar::Hm::Edit::Get::LineText() {
	wstring utf16_Text = CHidemaruExeExport::GetLineText();
	return utf16_to_utf8(utf16_Text);
}

string CPerlEzMagicalScalar::Hm::File::Get::ReadAllText() {
	string str_hmencode = utf8_lastHmEncodeResult.data();
	utf8_lastHmEncodeResult.clear();

	string filename = utf8_lastHmLoadFileName.data();
	utf8_lastHmLoadFileName.clear();

	UINT cnt;
	int hmencode = std::stoi(str_hmencode);
	bool success = false;
	wstring text = CHidemaruExeExport::LoadFileUnicode(utf8_to_utf16(filename), hmencode, &cnt, NULL, NULL, &success);
	if (!success) {
		text = wstring(L"-*-*-LoadFileUnicodeError-*-*-");
	}
	return utf16_to_utf8(text);
}

string CPerlEzMagicalScalar::Hm::Edit::Get::CursorPos() {
	auto pos = CHidemaruExeExport::GetCursorPos();
	return std::to_string(pos.lineno) + "," + std::to_string(pos.column);
}

string CPerlEzMagicalScalar::Hm::Edit::Get::CursorPosFromMousePos() {
	auto pos = CHidemaruExeExport::GetCursorPosFromMousePos();
	return std::to_string(pos.x) + "," + std::to_string(pos.y) + "," + std::to_string(pos.lineno) + "," + std::to_string(pos.column);
}


string CPerlEzMagicalScalar::Hm::Macro::Get::VarSimbol() {

	TestDynamicVar.Clear();
	auto dll_invocant = CSelfDllInfo::GetInvocantString();
	wstring cmd =
		L"##_tmp_dll_id_ret = dllfuncw( " + dll_invocant + L"\"SetDynamicVar\", " + stocked_macro_var_simbol + L");\n"
		L"##_tmp_dll_id_ret = 0;\n";
	BOOL success = CHidemaruExeExport::EvalMacro(cmd);

	// 数値なら
	if (TestDynamicVar.type == CDynamicValue::TDynamicType::TypeInteger)
	{
		return std::to_string(TestDynamicVar.num).data();
	}
	// 文字列なら
	else {
		return utf16_to_utf8(TestDynamicVar.wstr).data();
	}
}




string CPerlEzMagicalScalar::utf8_setvarofreturn = "";
LPCSTR CPerlEzMagicalScalar::SetHmComposedMagicScalarFunctions(LPVOID obj, LPCSTR p_utf8_SelfName, LPCSTR p_utf8_Value)
{
	utf8_setvarofreturn.clear();

	string utf8_SelfName = p_utf8_SelfName;
	wstring utf16_value = utf8_to_utf16(p_utf8_Value);

	// hm->debuginfo(...)を実行
	if (utf8_SelfName == szMagicalVarDebugInfo) {
		CPerlEzMagicalScalar::Hm::debuginfo(utf16_value);
		return p_utf8_Value;
	}

	// hm->File->HmEncode(...)へと代入（ファイル名を引数として渡す行為）
	else if (utf8_SelfName == szMagicalVarFileHmEncode) {
		utf8_lastHmEncodeResult.clear();

		int ret = CHidemaruExeExport::AnalyzeEncoding(utf16_value);
		// 関数抜けてポインタ消えないように、グローバルに乗っける
		utf8_lastHmEncodeResult = to_string(ret);
		return p_utf8_Value;
	}

	// hm->File->LoadEncode(...)へと代入 (秀丸のencode値を引数として渡す行為）
	else if (utf8_SelfName == szMagicalVarFileLoadEncode) {
		utf8_lastHmEncodeResult.clear();

		// 関数抜けてポインタ消えないように、グローバルに乗っける
		utf8_lastHmEncodeResult = p_utf8_Value;
		return p_utf8_Value;
	}

	// hm->File->LoadFileName(...)へと代入 (秀丸のファイル名を引数として渡す行為）
	else if (utf8_SelfName == szMagicalVarFileLoadFileName) {
		utf8_lastHmLoadFileName.clear();

		// 関数抜けてポインタ消えないように、グローバルに乗っける
		utf8_lastHmLoadFileName = p_utf8_Value;
		return p_utf8_Value;
	}

	// hm->Macro->Eval(...)を実行
	else if (utf8_SelfName == szMagicalVarMacroEval) {
		// 結果のクリア
		utf8_lastEvalResult.clear();

		int ret = CPerlEzMagicalScalar::Hm::Macro::Eval(utf16_value);
		if (ret) {
			utf8_lastEvalResult = "";
		}
		else {
			utf8_lastEvalResult = "HidemaruMacroEvalException";
		}
		return ret ? p_utf8_Value : "";
	}

	// hm->Edit->TodalText(...)へと代入
	else if (utf8_SelfName == szMagicalVarEditTotalText) {
		int ret = CPerlEzMagicalScalar::Hm::Edit::Set::TotalText(utf16_value);
		return ret ? p_utf8_Value : "";
	}

	// hm->Edit->SelectedText(...)へと代入
	else if (utf8_SelfName == szMagicalVarEditSelectedText) {
		int ret = CPerlEzMagicalScalar::Hm::Edit::Set::SelectedText(utf16_value);
		return ret ? p_utf8_Value : "";
	}
	// hm->Edit->LineText(...)へと代入
	else if (utf8_SelfName == szMagicalVarEditLineText) {
		int ret = CPerlEzMagicalScalar::Hm::Edit::Set::LineText(utf16_value);
		return ret ? p_utf8_Value : "";
	}

	// hm->Macro->Var(...)へと代入の前半
	else if (utf8_SelfName == szMagicalVarMacroVarSimbol) {
		int ret = CPerlEzMagicalScalar::Hm::Macro::Set::VarSimbol(utf16_value);
		return ret ? p_utf8_Value : "";
	}
	// hm->Macro->Var(...)へと代入の後半
	else if (utf8_SelfName == szMagicalVarMacroVarValue) {
		int ret = CPerlEzMagicalScalar::Hm::Macro::Set::VarValue(utf16_value);
		return ret ? p_utf8_Value : "";
	}

	else if (utf8_SelfName == szMagicalVarOutputPaneOutput) {
		int ret = Hm::OutputPane::Set::Output(utf16_value);
		return ret ? p_utf8_Value : "";
	}
	else if (utf8_SelfName == szMagicalVarOutputPanePush) {
		int ret = Hm::OutputPane::Set::Push();
		return ret ? p_utf8_Value : "";
	}
	else if (utf8_SelfName == szMagicalVarOutputPanePop) {
		int ret = Hm::OutputPane::Set::Pop();
		return ret ? p_utf8_Value : "";
	}
	else if (utf8_SelfName == szMagicalVarOutputPaneClear) {
		int ret = Hm::OutputPane::Set::Clear();
		return ret ? p_utf8_Value : "";
	}
	else if (utf8_SelfName == szMagicalVarOutputPaneSendMessage) {
		int ret = Hm::OutputPane::Set::SendMessage(utf16_value);
		return ret ? p_utf8_Value : "";
	}
	return p_utf8_Value;
}


BOOL CPerlEzMagicalScalar::Hm::debuginfo(wstring utf16_value) {
	OutputDebugStream(utf16_value);
	return TRUE;
}

BOOL CPerlEzMagicalScalar::Hm::Macro::Eval(wstring utf16_value) {
	BOOL success = CHidemaruExeExport::EvalMacro(utf16_value);
	if (success) {
		return TRUE;
	}
	OutputDebugStream(L"マクロの実行に失敗しました。\n");
	OutputDebugStream(L"マクロ内容:\n");
	OutputDebugStream(utf16_value);
	return FALSE;
}


BOOL CPerlEzMagicalScalar::Hm::Edit::Set::TotalText(wstring utf16_value) {
	BOOL success = 0;

	auto dll_invocant = CSelfDllInfo::GetInvocantString();

	PushStrVar(utf16_value.data());
	wstring cmd =
		L"begingroupundo;\n"
		L"selectall;\n"
		L"insert dllfuncstrw( " + dll_invocant + L"\"PopStrVar\" );\n"
		L"endgroupundo;\n";
	success = CHidemaruExeExport::EvalMacro(cmd);

	return success;
}

BOOL CPerlEzMagicalScalar::Hm::Edit::Set::SelectedText(wstring utf16_value) {
	BOOL success = 0;

	auto dll_invocant = CSelfDllInfo::GetInvocantString();

	PushStrVar(utf16_value.data());
	wstring cmd = 
		L"if (selecting) {\n"
		L"insert dllfuncstrw( " + dll_invocant + L"\"PopStrVar\" );\n"
		L"};\n";
	success = CHidemaruExeExport::EvalMacro(cmd);

	return success;
}

BOOL CPerlEzMagicalScalar::Hm::Edit::Set::LineText(wstring utf16_value) {
	BOOL success = 0;

	auto dll_invocant = CSelfDllInfo::GetInvocantString();

	auto pos = CHidemaruExeExport::GetCursorPos();
	PushStrVar(utf16_value.data());
	wstring cmd =
		L"begingroupundo;\n"
		L"selectline;\n"
		L"insert dllfuncstrw( " + dll_invocant + L"\"PopStrVar\" );\n"
		L"moveto2 " + std::to_wstring(pos.column) + L", " + std::to_wstring(pos.lineno) + L";\n" +
		L"endgroupundo;\n";
	success = CHidemaruExeExport::EvalMacro(cmd);

	return success;
}

BOOL CPerlEzMagicalScalar::Hm::Macro::Set::VarSimbol(wstring utf16_value) {
	stocked_macro_var_simbol = utf16_value;
	return TRUE;
}

BOOL CPerlEzMagicalScalar::Hm::Macro::Set::VarValue(wstring utf16_value) {
	BOOL success = 0;

	auto dll_invocant = CSelfDllInfo::GetInvocantString();

	wchar_t start = stocked_macro_var_simbol[0];
	if (start == L'#') {
		// 数字を数値にトライ。ダメなら0だよ。
		intHM_t n = 0;
		try {
			n = (intHM_t)std::stoll(utf16_value);
		}
		catch (...) {}

		PushNumVar(n);
		wstring cmd = L" = dllfuncw( " + dll_invocant + L"\"PopNumVar\" );\n";
		cmd = stocked_macro_var_simbol + cmd;
		success = CHidemaruExeExport::EvalMacro(cmd);
	}
	else if (start == L'$') {

		PushStrVar(utf16_value.data());
		wstring cmd = L" = dllfuncstrw( " + dll_invocant + L"\"PopStrVar\" );\n";
		cmd = stocked_macro_var_simbol + cmd;
		success = CHidemaruExeExport::EvalMacro(cmd);
	}

	return success;
}

BOOL CPerlEzMagicalScalar::Hm::OutputPane::Set::Output(wstring utf16_value) {
	// ちゃんと関数がある時だけ
	if (CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle) {
		HWND hHidemaruWindow = CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle();

		if (CHidemaruExeExport::HmOutputPane_OutputW) {
			BOOL result = CHidemaruExeExport::HmOutputPane_OutputW(hHidemaruWindow, (wchar_t*)(utf16_value.data()));
			return result;
		} else if (CHidemaruExeExport::HmOutputPane_Output) {
			auto encode_byte_data = EncodeWStringToOriginalEncodeVector(utf16_value);
			BOOL result = CHidemaruExeExport::HmOutputPane_Output(hHidemaruWindow, encode_byte_data.data());
			return result;
		}
	}
	return FALSE;
}

BOOL CPerlEzMagicalScalar::Hm::OutputPane::Set::Push() {
	if (CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle) {
		HWND hHidemaruWindow = CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle();
		if (CHidemaruExeExport::HmOutputPane_Push) {
			BOOL result = CHidemaruExeExport::HmOutputPane_Push(hHidemaruWindow);
			return result;
		}
	}
	return FALSE;
}

BOOL CPerlEzMagicalScalar::Hm::OutputPane::Set::Pop() {
	if (CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle) {
		HWND hHidemaruWindow = CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle();
		if (CHidemaruExeExport::HmOutputPane_Push) {
			BOOL result = CHidemaruExeExport::HmOutputPane_Pop(hHidemaruWindow);
			return result;
		}
	}
	return FALSE;
}

BOOL CPerlEzMagicalScalar::Hm::OutputPane::Set::SendMessage(wstring utf16_command_id) {
	HWND OutputWindowHandle = CHidemaruExeExport::OutputPane_GetWindowHanndle();
	if (OutputWindowHandle) {
		// (#h,0x111/*WM_COMMAND*/,1009,0);//1009=クリア
		int command_id = 0;
		LRESULT r = 0;
		try {
			command_id = std::stoi(utf16_command_id);
			r = ::SendMessageW(OutputWindowHandle, WM_COMMAND, command_id, 0);
		}
		catch (exception e) {
			wstring error = cp932_to_utf16(e.what());
			OutputDebugStream(error);
		}
		return r;
	}
	return FALSE;
}

BOOL CPerlEzMagicalScalar::Hm::OutputPane::Set::Clear() {
	// (#h,0x111/*WM_COMMAND*/,1009,0);//1009=クリア
	return CPerlEzMagicalScalar::Hm::OutputPane::Set::SendMessage(L"1009");
}



void CPerlEzMagicalScalar::BindMagicalScalarFunctions(CPerlEzEngine* module) {
	auto& engine = module->engine;

	// 全てのプロパティ(的なグローバル変数)にフックされる、GetとSetの関数を登録
	module->PerlEzSetMagicScalarFunctions(engine, CPerlEzMagicalScalar::GetHmComposedMagicScalarFunctions, CPerlEzMagicalScalar::SetHmComposedMagicScalarFunctions);

	// 上記関数を機能させるグローバル変数を登録していく。
	auto list = {
		szMagicalVarDebugInfo,
		szMagicalVarVersion,
		szMagicalVarFileHmEncode,
		szMagicalVarFileHmEncodeResult,
		szMagicalVarFileLoadEncode,
		szMagicalVarFileLoadFileName,
		szMagicalVarFileLoadResult,
		szMagicalVarEditTotalText,
		szMagicalVarEditSelectedText,
		szMagicalVarEditLineText,
		szMagicalVarEditCursorPos,
		szMagicalVarEditCursorPosFromMousePos,
		szMagicalVarMacroEval,
		szMagicalVarMacroEvalResult,
		szMagicalVarMacroVarSimbol,
		szMagicalVarMacroVarValue,
		szMagicalVarOutputPaneOutput,
		szMagicalVarOutputPanePush,
		szMagicalVarOutputPanePop,
		szMagicalVarOutputPaneClear,
		szMagicalVarOutputPaneSendMessage,
	};

	for (auto var_name : list) {
		module->PerlEzSetMagicScalarName(engine, var_name);
	}
}
