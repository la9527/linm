#include "cmd_common_imp.h"
#include "mainframe.h"

using namespace MLS;
using namespace MLSUTIL;

void	CmdCommonImp::About()
{
	vector<string>	vAbout;
	String			str;

	str.Printf( ChgEngKor("LinM %s - User-Interfaced File Manager ", 
				          "LinM %s - 심플한 파일 관리자 ").c_str(), VERSION);
	vAbout.push_back( str.c_str() );
	vAbout.push_back( "" );
	vAbout.push_back( ChgEngKor("Author : ",
								"작성자  : " ) );
	vAbout.push_back( ChgEngKor("  Project Manager : Byoungyoung, La (la9527@daum.net)",
					            "  프로젝트관리자   : 라병영 (la9527@daum.net)") );
	vAbout.push_back( ChgEngKor("  Develper        : IOKLO (leesjung@nownuri.net)",
								"  개발자           : IOKLO  (leesjung@nownuri.net)") );
	vAbout.push_back( ChgEngKor("  Develper        : Sangkoung, Bae (baesg@cntek.co.kr)",
								"  개발자           : 배상경 (baesg@cntek.co.kr)") );
	vAbout.push_back( ChgEngKor("  Develper        : Youngseop, Seo (seobi49@naver.com)",
								"  개발자           : 서영섭 (seobi49@naver.com)") );
	vAbout.push_back( ChgEngKor("  Develper        : kayws426 (kayws426@gmail.com)",
								"  개발자           : 강우성 (kayws426@gmail.com)") );
	vAbout.push_back( ChgEngKor("  Tester          : Leem ChaeHoon (infiniterun@gmail.com)",
								"  테스터           : 임채훈 (infiniterun@gmail.com)") );
	vAbout.push_back( ChgEngKor("  Translator      : Eunseo, Choi (eunseo.choi@gmail.com)",
								"  영작             : 최은서 (eunseo.choi@gmail.com)") );
	vAbout.push_back( ChgEngKor("  Designer        : Kitea, Kim (bluetux@gmail.com)",
								"  디자인           : 김기태 (bluetux@gmail.com)") );
	vAbout.push_back( "" );
	vAbout.push_back( ChgEngKor("  * Project homepage : http://kldp.net/projects/mls", 
								"  * 프로젝트 홈페이지 : http://kldp.net/projects/mls") );
	vAbout.push_back( "" );
	vAbout.push_back( "(c) 2007 The LinM Developers" );
	TextListBox( _("About"), vAbout );
}

void	CmdCommonImp::Help()
{
	vector<string>	vMsgData;
	String			sMsg;
	string			sCmd;
	string 			sView;
	
	vMsgData.push_back("-----------------------------------------------------------------");
    vMsgData.push_back(_("LinM Key List"));
	vMsgData.push_back("-----------------------------------------------------------------");
    vMsgData.push_back("");

	vMsgData.push_back( _("File Panel Key List") );

	map<TypeInfo*, string>::iterator i;

	for (i = g_tKeyCfg._mapKeyHelp.begin(); i != g_tKeyCfg._mapKeyHelp.end(); ++i)
	{
		if ( i->first->eType == PANEL || i->first->eType == COMMON )
		{
			string sKeyName = g_tKeyCfg.CmdToKeyName( i->first->sValue, i->first->eType );

			if (sKeyName.size() > 1 )
				if ( sKeyName[0] == 'F' && (sKeyName[1] > '0' && sKeyName[1] <= '9'))
					continue;

			if ( !sKeyName.empty() && sKeyName != i->second)
			{
				sView = g_tKeyCfg.GetHelp( i->first->sValue, i->first->eType );
				LOG("Key List :: [%s]", sView.c_str());
				
				sMsg.Printf("    %-10s : %s", sKeyName.c_str(), _(sView.c_str()));
				vMsgData.push_back(sMsg.c_str());
			}
		}
	}

	vMsgData.push_back("");
	vMsgData.push_back( _("LinM Change Directory Key List") );

	for (i = g_tKeyCfg._mapKeyHelp.begin(); i != g_tKeyCfg._mapKeyHelp.end(); ++i)
	{
		if ( i->first->eType == MCD || i->first->eType == COMMON )
		{
			string sKeyName = g_tKeyCfg.CmdToKeyName( i->first->sValue, i->first->eType );

			if (sKeyName.size() > 1 )
				if ( sKeyName[0] == 'F' && (sKeyName[1] > '0' && sKeyName[1] <= '9'))
					continue;

			if ( !sKeyName.empty() && sKeyName != i->second)
			{
				sView = g_tKeyCfg.GetHelp( i->first->sValue, i->first->eType );	

				sMsg.Printf("    %-10s : %s", sKeyName.c_str(), _(sView.c_str()));
				vMsgData.push_back(sMsg.c_str());
			}
		}
	}

	vMsgData.push_back("");
	vMsgData.push_back( _("Simple Editor Key List") );

	for (i = g_tKeyCfg._mapKeyHelp.begin(); i != g_tKeyCfg._mapKeyHelp.end(); ++i)
	{
		if ( i->first->eType == EDITOR || i->first->eType == COMMON )
		{
			string sKeyName = g_tKeyCfg.CmdToKeyName( i->first->sValue, i->first->eType );

			if (sKeyName.size() > 1 )
				if ( sKeyName[0] == 'F' && (sKeyName[1] > '0' && sKeyName[1] <= '9'))
					continue;

			if ( !sKeyName.empty() && sKeyName != i->second)
			{
				sView = g_tKeyCfg.GetHelp( i->first->sValue, i->first->eType );

				sMsg.Printf("    %-10s : %s", sKeyName.c_str(), _(sView.c_str()));
				vMsgData.push_back(sMsg.c_str());
			}
		}
	}

	vMsgData.push_back("");

	vMsgData.push_back("-----------------------------------------------------------------");
    vMsgData.push_back( _("LinM Function Key List") );
	vMsgData.push_back("-----------------------------------------------------------------");

	vMsgData.push_back( "" );
	vMsgData.push_back( _("File Panel Function Key List") );
	vMsgData.push_back( "" );

	for (int i=1; i<= 12; i++)
	{
		sMsg.Printf("F%d", i);
		sCmd = g_tKeyCfg.GetCommand( sMsg.c_str(), PANEL );
		sCmd = g_tKeyCfg.GetHelp( sCmd, PANEL );
		if ( !sCmd.empty() )
		{
			sMsg.Printf("    %-10s : %s", sMsg.c_str(), _(sCmd.c_str()));
			vMsgData.push_back(sMsg.c_str());
		}
	}

	vMsgData.push_back( "" );
	vMsgData.push_back( _("LinM Change Directory Function Key List") );

	for (int i=1; i<= 12; i++)
	{
		sMsg.Printf("F%d", i);
		sCmd = g_tKeyCfg.GetCommand( sMsg.c_str(), MCD );
		sCmd = g_tKeyCfg.GetHelp( sCmd, MCD );
		if ( !sCmd.empty() )
		{
			sMsg.Printf("    %-10s : %s", sMsg.c_str(), _(sCmd.c_str()));
			vMsgData.push_back(sMsg.c_str());
		}
	}

	vMsgData.push_back( "" );
	vMsgData.push_back( _("Simple Editor Function Key List") );
	vMsgData.push_back( "" );

	for (int i=1; i<= 12; i++)
	{
		sMsg.Printf("F%d", i);
		sCmd = g_tKeyCfg.GetCommand( sMsg.c_str(), EDITOR );
		sCmd = g_tKeyCfg.GetHelp( sCmd, EDITOR );
		if ( !sCmd.empty() )
		{
			sMsg.Printf("    %-10s : %s", sMsg.c_str(), _(sCmd.c_str()));
			vMsgData.push_back(sMsg.c_str());
		}
	}
	
	TextListBox( _("Help"), vMsgData, true );
}


void	CmdCommonImp::ReloadConfigChange( bool bCursesRefresh )
{
	if ( _pEditor->_bConfChg )
	{
		string sCfgHome, sDefault, sColor, sKeySet, sTmpDir, sTmpCopyDir;
		sCfgHome = g_tCfg.GetValue("Static", "CfgHome");
		sDefault = g_tCfg.GetValue("Static", "CfgFile");
		sColor = g_tCfg.GetValue("Static", "ColFile");
		sKeySet = g_tCfg.GetValue("Static", "KeyFile");
		sTmpDir = g_tCfg.GetValue( "Static", "TmpDir" );
		sTmpCopyDir = g_tCfg.GetValue("Static", "TmpCopyDir");
		
		g_tCfg.Clear();
		g_tColorCfg.Clear();
		g_tKeyCfg.Clear();
		
		LOG("sCfgHome :: %s", sCfgHome.c_str());
		LOG("sDefault :: %s", sDefault.c_str());
		LOG("sColor :: %s", sColor.c_str());
		LOG("sKeySet :: %s", sKeySet.c_str());

		g_tCfg.Load(sDefault.c_str());
		g_tColorCfg.Load(sColor.c_str());
		g_tKeyCfg.Load(sKeySet.c_str());

		g_tCfg.SetStaticValue("CfgHome", sCfgHome);
		g_tCfg.SetStaticValue("CfgFile", sDefault);
		g_tCfg.SetStaticValue("ColFile", sColor);
		g_tCfg.SetStaticValue("KeyFile", sKeySet);
		g_tCfg.SetStaticValue("TmpDir",  sTmpDir);
		g_tCfg.SetStaticValue("TmpCopyDir", sTmpCopyDir);

		_pEditor->_bConfChg = false;

		g_tMainFrame.Reload();

		if ( bCursesRefresh )
			CursesRestart( g_tCfg.GetBool("Default", "Transparency", false ) );
	}
}

void CmdCommonImp::BoxCodeChange()
{
	g_tMainFrame.LineCodeChange();
}

void CmdCommonImp::LangChange()
{
	if (e_nCurLang == US)
		Set_Locale(AUTO);
	else
		Set_Locale(US);
}
