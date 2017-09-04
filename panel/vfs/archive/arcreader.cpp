#include "arcreader.h"
#include "mlslocale.h"

using namespace MLSUTIL;
using namespace MLS;

bool	ArcReader::Init(const string& sInitFile)
{ 
	_sCurPath = "";
	void*	pWait = MsgWaitBox(	_("Wait"),
								_("Please wait !!! - Cancel Key [Ctrl+C]"));
	
	if (_pArchive) delete _pArchive; _pArchive = NULL;
	_pArchive = new Archive(sInitFile);
	
	SetKeyBreakUse( true );

	if (_pArchive->FileListRead() == SUCCESS)
	{
		_sCurPath = "/";
		_sInitTypeName = "archive://" + sInitFile;
		SetKeyBreakUse( false );
		MsgWaitEnd(pWait);
		_bConnected = true;
		return true;
	}
	
	SetKeyBreakUse( false );

	if (_pArchive) delete _pArchive; _pArchive = NULL;
	MsgWaitEnd(pWait);
	MsgBox(_("Error"), "Archive file view failure. !!!");
	return false;
}

void	ArcReader::Destroy()
{
	if (_pArchive) delete _pArchive; _pArchive = NULL;

	_sCurPath = "";
	_sInitTypeName = "";
	_bConnected = false;
}

string ArcReader::GetViewPath() const 
{
	if (_sCurPath.size() > 0 && _sCurPath[0] == '/')
		return _sInitTypeName + ChgCurLocale(_sCurPath);
	else
		return _sInitTypeName + "/" + ChgCurLocale(_sCurPath);
}

string ArcReader::GetRealPath(const string& str) const
{
	return _sCurPath;
}

bool ArcReader::Read(const string &sPath)
{
	if (_pArchive == NULL)
	{
		LOG_WRITE("_pArchive is NULL");
		return false;
	}
	
	string sRealPath;
	if (sPath == "") 
		sRealPath = "/";
	else
		sRealPath = sPath;
	
	if (_pArchive->GetDir_Files(sRealPath, &_vFileList) == SUCCESS)
	{
		_uCur = 0;
		_sCurPath = sRealPath;
		LOG_WRITE("ArcReader Read Ok !!! - Path [%s] [%d]", sRealPath.c_str(), _vFileList.size());
		return true;
	}
	else
	{
		LOG_WRITE("ArcReader Read false !!!");
		return false;
	}
}

bool ArcReader::Next()
{
	if (_uCur < _vFileList.size())
	{
		_uCur++;
		return true;
	}
	return false;
}

bool ArcReader::GetInfo(File &tFile)
{
	File*	pFile = NULL;

	// . 현재 선택된 파일이 유효하면
	if (_uCur-1 < _vFileList.size())
	{
		// .. filelist에서 파일 정보를 가져옴
		pFile = _vFileList[_uCur-1];
		tFile.Clear();

		tFile.sType = _sInitTypeName;
		
		// .. filename얻기
		if (_sCurPath == pFile->sFullName)
		{
			tFile.sName = "..";

			string 				sParentPath;
			string::size_type	tSizeType;

			if (_sCurPath.substr(_sCurPath.length()-1) == "/")
			{
				sParentPath = _sCurPath.substr(0, _sCurPath.size()-1);
			}

			tSizeType = sParentPath.rfind("/");
			if (tSizeType != string::npos) 
			{
				tFile.sFullName = sParentPath.substr(0, tSizeType+1);
			}
			else
			{
				tFile.sFullName = "/";
			}
		}
		else
		{
			tFile.sName = ChgCurLocale(pFile->sName);
			tFile.sFullName = pFile->sFullName;
		}

		tFile.sTmp = pFile->sTmp;
		tFile.bDir = pFile->bDir;
		tFile.bLink = false;
		tFile.tColor = pFile->tColor;
		tFile.sDate = pFile->sDate;
		tFile.sTime = pFile->sTime;
		tFile.sAttr = pFile->sAttr;
		tFile.bSelected = pFile->bSelected;
		tFile.uSize = pFile->uSize;
		/*
		LOG_WRITE("%s, %s, %d, %s, %s, %s, %ld", 	tFile.sName.c_str(), 
													tFile.sFullName.c_str(),
													tFile.bDir,
													tFile.sDate,
													tFile.sTime,
													tFile.sAttr,
													tFile.uSize);
		*/
	}
	else
		return false;
	return true;
}

bool ArcReader::isRoot()
{
	if (_sCurPath.length() == 0) return true;
	return false;
}

///	\brief	파일을 보기 위한..
///	\param	tFile	볼 파일.
///	\return	압축 파일을 tmp 에 복사 해놓고 파일 위치 리턴
bool  ArcReader::View(const File* pFileOriginal, File* pFileChange)
{ 
	if (!_pArchive) return false;
	void*	pWait = MsgWaitBox(	_("Wait"),
								_("Please wait !!! - Cancel Key [Ctrl+C]"));

	SetKeyBreakUse( true );

	if (_pArchive->Uncompress(pFileOriginal, _sTmpDir.c_str()) == ERROR)
	{
		SetKeyBreakUse( false );
		MsgWaitEnd(pWait);
		MsgBox(_("Error"), _("Uncompress failure !!!"));
		return false;
	}

	*pFileChange = *pFileOriginal;
	pFileChange->sFullName = _sTmpDir + pFileOriginal->sTmp;
	pFileChange->sTmp2 = pFileOriginal->sFullName;
	pFileChange->sName = pFileOriginal->sName;
	
	SetKeyBreakUse( false );
	MsgWaitEnd(pWait);
	return true;
}

///	\brief	파일 복사
///	\return	복사 여부
bool	ArcReader::Copy(Selection&	tSelection, const string& sTargetPath, Selection* pSelection)
{
	if (!_pArchive) return false;
	
	void*	pWait = MsgWaitBox(	_("Wait"),
								_("Please wait !!! - Cancel Key [Ctrl+C]"));

	SetKeyBreakUse( true );

	vector<File*>	tFileList = tSelection.GetData();

	if ( tFileList.size() )
	{	
		// Flie All Extract.	
		if (_pArchive->Uncompress(sTargetPath) == ERROR)
		{
			MsgWaitEnd(pWait);
			SetKeyBreakUse( false );
			MsgBox(_("Error"), _("Uncompress failure !!!"));
			return false;
		}
	}	

	SetKeyBreakUse( false );

	LOG_WRITE("ArcReader Copy :: sTargetPath [%s]", sTargetPath.c_str());

	if (pSelection != NULL)
	{
		pSelection->Clear();

		string sTargetPathTmp = sTargetPath;
		if (sTargetPathTmp.substr(sTargetPathTmp.length()-1, 1) != "/")
			sTargetPathTmp = sTargetPathTmp + "/";
		
		for (uint n = 0; n < tFileList.size(); n++)
		{
			File* pFile = tFileList[n];
			File	tFile = *pFile;
			//LOG_WRITE("ArcReader::Copy Path [%s]", tFile.sFullName.c_str());
			if (pFile->sFullName.size() > 0	&& pFile->sFullName.substr(0, 1) == "/")
				tFile.sFullName = sTargetPathTmp + pFile->sFullName.substr(1, pFile->sFullName.length()-1);
			else
				tFile.sFullName = sTargetPathTmp + pFile->sFullName;
			LOG_WRITE("ArcReader::Copy Name [%s]", tFile.sFullName.c_str());
			pSelection->Add(&tFile);
		}

		string sSaveName;

		if (tSelection.GetSelectPath().size() > 0 && tSelection.GetSelectPath().substr(0,1) == "/")
			sSaveName = sTargetPathTmp + tSelection.GetSelectPath().substr(1);
		else
			sSaveName = sTargetPathTmp + tSelection.GetSelectPath();
		
		pSelection->SetSelectPath(sSaveName);
	}

	MsgWaitEnd(pWait);
	return true;
}

///	\brief	파일 이동
///	\return	이동 여부
bool	ArcReader::Move(Selection&	tSelection, const string& sTargetPath, Selection* pSelection)
{
	MsgBox(_("Error"), _("Compress move failure !!!"));
	return false;
}

///	\brief	파일 삭제
///	\return	삭제 여부
bool	ArcReader::Remove(Selection&	tSelection, bool bMsgShow)
{
	if (!_pArchive) return false;

	void*	pWait = MsgWaitBox(	_("Wait"),
								_("Please wait !!! - Cancel Key [Ctrl+C]"));

	SetKeyBreakUse( true );

	vector<File*>	tFileList = tSelection.GetData();
	if (_pArchive->Compress(tFileList, TAR_DELETE) == ERROR)
	{
		MsgWaitEnd(pWait);
		SetKeyBreakUse( false );
		MsgBox(_("Error"), _("Uncompress failure !!!"));
		return false;
	}
	SetKeyBreakUse( false );
	MsgWaitEnd(pWait);
	return true;
}

///	\brief	파일 붙여넣기 할때 압축파일에 파일 넣을 때 사용
///	\return	성공여부
bool	ArcReader::Paste(Selection& tSelection)
{
	if (!_pArchive) return false;

	void*	pWait = MsgWaitBox(	_("Wait"),
								_("Please wait !!! - Cancel Key [Ctrl+C]"));

	vector<File*>	tFileList = tSelection.GetData();

	switch(_pArchive->GetCurZipType())
	{
		case TAR_GZ:
		case TAR_BZ:
		case TAR:
		case ZIP:
		{
			// /tmp/ + targetdir 로 해야 하는데 나중에 수정해야 함.
			for (int n = 0; n < tFileList.size(); n++)
			{
				File* pFile = tFileList[n];
				pFile->sTmp = pFile->sFullName.substr(_sTmpDir.length());
			}
			SetKeyBreakUse( true );
			if (_pArchive->Compress(tFileList, TAR_APPEND, _sTmpDir) == ERROR)
			{
				MsgWaitEnd(pWait);
				SetKeyBreakUse( false );
				MsgBox(_("Error"), _("Uncompress failure !!!"));
				return false;
			}
			break;
		}
		
		default:
			SetKeyBreakUse( false );
			MsgWaitEnd(pWait);
			return false;
	}
	SetKeyBreakUse( false );
	MsgWaitEnd(pWait);
	return true;
}

