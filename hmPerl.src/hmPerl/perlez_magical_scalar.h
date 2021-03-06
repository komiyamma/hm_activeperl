/*
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#pragma once

#include "perlez_engine.h"

#define szMagicalVarDebugInfo		    "hm_debuginfo"
#define szMagicalVarVersion			    "hm_version"
#define szMagicalVarMacroVarSimbol      "hm_Macro_Var_Simbol"
#define szMagicalVarMacroVarValue       "hm_Macro_Var_Value"
#define szMagicalVarMacroEval		    "hm_Macro_Eval"
#define szMagicalVarMacroEvalResult	    "hm_Macro_Eval_Result"
#define szMagicalVarEditTotalText	    "hm_Edit_TotalText"
#define szMagicalVarEditSelectedText    "hm_Edit_SelectedText"
#define szMagicalVarEditLineText        "hm_Edit_LineText"
#define szMagicalVarEditCursorPos       "hm_Edit_CursorPos"
#define szMagicalVarEditCursorPosFromMousePos    "hm_Edit_CursorPosFromMousePos"
#define szMagicalVarFileHmEncode        "hm_File_HmEncode"
#define szMagicalVarFileHmEncodeResult  "hm_File_HmEncode_Result"
#define szMagicalVarFileLoadEncode      "hm_File_Load_Encode"
#define szMagicalVarFileLoadFileName    "hm_File_Load_FileName"
#define szMagicalVarFileLoadResult      "hm_File_Load_Result"
#define szMagicalVarOutputPaneOutput          "hm_OutputPane_Output"
#define szMagicalVarOutputPanePush            "hm_OutputPane_Push"
#define szMagicalVarOutputPanePop             "hm_OutputPane_Pop"
#define szMagicalVarOutputPaneClear           "hm_OutputPane_Clear"
#define szMagicalVarOutputPaneSendMessage     "hm_OutputPane_SendMessage"


//========================================================================
// PerlEzで特定のシンボルを一種の「プロパティ」のように見立てて、Get/Set用の関数をフックする。
//========================================================================
class CPerlEzMagicalScalar {
	// シンボルにアクセスする際にここに直前のものがかならずストックされる。
private:
	static wstring stocked_macro_var_simbol;

	//-------------------------------------------------------------------------
	// 返り値はstringで持っておく。PerlScopeへと値を返した時に、インスタンスの消滅を防ぐため。
	static string utf8_getvarofreturn;

	
	//-------------------------------------------------------------------------
	// プロパティに見立てたPerlScope上のグローバル変数に対して、「値を取得」の操作が行われた時、この関数が呼ばれる。
	// p_utf8_SelfName には、対象のグローバル変数のシンボル名そのものが渡ってくるので、これで処理を振り分け
	static LPCSTR GetHmComposedMagicScalarFunctions(LPVOID obj, LPCSTR p_utf8_SelfName);

	//-------------------------------------------------------------------------
	// 返り値はstringで持っておく。PerlScopeへと値を返した時に、インスタンスの消滅を防ぐため。
	static string utf8_setvarofreturn;

	//-------------------------------------------------------------------------
	// プロパティに見立てたPerlScope上のグローバル変数に対して、「値を設定」の操作が行われた時、この関数が呼ばれる。
	// p_utf8_SelfName には、対象のグローバル変数のシンボル名そのものが渡ってくるので、これで処理を振り分け
	static LPCSTR SetHmComposedMagicScalarFunctions(LPVOID obj, LPCSTR p_utf8_SelfName, LPCSTR p_utf8_Value);

public:
	//-------------------------------------------------------------------------
	// Get/Set用の関数をフックの共通関数の登録と、一種の「プロパティ」のように見立たてるシンボル名の登録
	//-------------------------------------------------------------------------
	static void BindMagicalScalarFunctions(CPerlEzEngine* module);


private:
	//-------------------------------------------------------------------------
	// フック用関数の各種イベントハンドラ
	//-------------------------------------------------------------------------
	struct Hm {

		static BOOL debuginfo(wstring utf16_value);
		static string version();

		struct File {
			struct Get {
				static int HmEncode();
				static string ReadAllText();
			};
			struct Set {
				static int HmEncode(wstring utf16_value);
			};
		};

		struct Edit {
			struct Get {
				static string TotalText();
				static string SelectedText();
				static string LineText();
				static string CursorPos();
				static string CursorPosFromMousePos();
			};
			struct Set {
				static BOOL TotalText(wstring utf16_value);
				static BOOL SelectedText(wstring utf16_value);
				static BOOL LineText(wstring utf16_value);
			};

		};

		struct Macro {

			static BOOL Eval(wstring utf16_value);

			struct Get {
				static string VarSimbol();
			};
			struct Set {
				static BOOL VarSimbol(wstring utf16_value);
				static BOOL VarValue(wstring utf16_value);
			};
		};

		struct OutputPane {

			struct Set {
				static BOOL Output(wstring utf16_value);
				static BOOL Push();
				static BOOL Pop();
				static BOOL Clear();
				static BOOL SendMessage(wstring utf16_commannd_id);
			};
		};

	};
};


