#include "define.h"
#include "mlsdialog.h"
#include "file.h"
#include "reader.h"
#include "selection.h"
#include "readerctl.h"
#include "mpool.h"

using namespace MLSUTIL;
using namespace MLS;

ClipBoard::ClipBoard()
{
	nState = CLIP_NONE;
}


ClipBoard::~ClipBoard()
{
}

void ClipBoard::Set(ClipState cs)
{
	nState = cs;
}

ClipState ClipBoard::GetState( )
{
	return nState;
}

ullong Selection::CalcSize(Reader* pReader)
{
	ullong uSize = 0;

	if (pReader) ExpandDir(pReader, true);
		
	for (uint t=0; t<_vFile.size(); t++)
		if (_vFile[t]->bDir == false)
			uSize = uSize + _vFile[t]->uSize;
	return uSize;
}

class	DirList
{
	public:
		string	_sFullName;
		bool	_bDel;
	
		DirList(const string& sFullName, bool Del = false) 
		{ 
			_sFullName = sFullName; _bDel = Del; 
		}
};

int Selection::ExpandDir(Reader* pReader, bool bCtlBreak)
{
	if (!pReader)
	{
		LOG_WRITE("ExtendDir Error");
		return ERROR;
	}

	if (!_bUseExpandDir) return ERROR;

	vector<DirList*>	vDirs;
	
	// vDirs 를 채워 넣는다.
	for (int n = 0; n < (int)_vFile.size(); n++)
		if (_vFile[n]->bDir && !_vFile[n]->bLink)
			vDirs.push_back(new DirList(_vFile[n]->sFullName));

	string sBefPath = pReader->GetPath();
		
	pReader->SetErrMsgShow(false);
	
	LOG_WRITE("ExpandDir First Dirsize [%d]", vDirs.size());

	string	sDirName;

	for(;;)
	{
		DirList* pDirList = NULL;
	
		bool bAllCheck = false;
		for (int n = 0; n < (int)vDirs.size(); n++)
		{
			pDirList = vDirs[n];
			if (pDirList->_bDel == false)
			{
				bAllCheck = false;
				break;
			}
			bAllCheck = true;
		}

		if (bAllCheck == true || pDirList == NULL) break;
		pDirList->_bDel = true;
		
		sDirName = pDirList->_sFullName;
		
		if (pReader->Read(sDirName) == false) continue;

		bool bBreak = false;
		
		while(pReader->Next())
		{
			File*	pFile = new File;
			if (!pReader->GetInfo(*pFile)) continue;

			// ESC를 누르면 STOP 된다.
			if (bCtlBreak && GetChar(true)==27) 
			{
				bBreak = true;
				break;
			}
			
			if (pFile->sName == "." || pFile->sName == "..")	continue;

			if (pFile->bDir)
			{
				bool bPush = true;
				for (int n = 0; n < (int)vDirs.size(); n++)
				{
					if (vDirs[n]->_sFullName == pFile->sFullName)
					{
						bPush = false;
						break;
					}
				}
				
				if (bPush == true && !pFile->bLink )
					vDirs.push_back(new DirList(pFile->sFullName));

				_vFile.push_back(pFile);
			}
			else
			{
				LOG_WRITE("ExpandDir Insert File [%s] [%s]", pFile->sName.c_str(), pFile->sFullName.c_str());
				_vFile.push_back(pFile);
			}
		}

		if (bBreak == true) break;
	}

	for (int n = 0; n < (int)vDirs.size(); n++)
		delete vDirs[n];

	pReader->Read(sBefPath);
	_bUseExpandDir = true;
	return SUCCESS;
}
