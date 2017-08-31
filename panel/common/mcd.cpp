/******************************************************************************
 *   Copyright (C) 2005 by la9527                                             *
 *                                                                            *
 *  This program is free software; you can redistribute it and/or modify      * 
 *  it under the terms of the GNU General Public License as published by      *
 *  the Free Software Foundation; either version 2 of the License, or         *
 *  (at your option) any later version.                                       *
 *                                                                            *
 *  This program is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 *  GNU General Public License for more details.                              *
 *                                                                            *
 *  You should have received a copy of the GNU General Public License         *
 *  along with this program; if not, write to the Free Software               *
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.*
 ******************************************************************************/

#include    "mcd.h"
#include    "cmdshell.h"

using namespace MLSUTIL;
using namespace MLS;

Mcd::Mcd(const string &sDir) {
    _pRoot = NULL;

    _nSRow = 0;
    _nSCol = 0;

    _sCode = "LINM0.7";

    _nCounter = 0;
    _pReader = NULL;
    _bMcdSort = true;
    _bHidden = false;

    if (sDir == "")
        _pRoot = new Dir("/", NULL);
    else
        _pRoot = new Dir(sDir, NULL);

    _pOrder.clear();
    SetOrder();
    SetCur("/");
}

Mcd::~Mcd() {
    if (_pRoot) {
        for (pDirIterator p = _pOrder.begin(); p != _pOrder.end(); ++p)
            delete *p;

        _pRoot = NULL;
    }
}

bool Mcd::Load(const char *sFile) {
    char sCheckVersion[10];
    int nPreDepth = 0, nRow = 0;
    Dir *preNode = NULL, *pNode = NULL;
    FILE *pFile = NULL;

    memset(&sCheckVersion, 0, sizeof(sCheckVersion));

    if (sFile)
        pFile = fopen(sFile, "rb");

    if (!pFile) return false;

    fread(sCheckVersion, _sCode.size(), 1, pFile);

    if (_sCode != sCheckVersion) return false;

    File tFile;

    while (!feof(pFile)) {
        int nDepth = fgetc(pFile);
        int nCheck = fgetc(pFile);
        int nLink = fgetc(pFile);

        int nSize = fgetc(pFile) * 256;
        nSize += fgetc(pFile);

        if (feof(pFile)) break;

        char sFullName[nSize + 1];
        fread(sFullName, nSize, 1, pFile);
        sFullName[nSize] = 0;

        // depth에 따른 노드 조정
        if (nDepth > nPreDepth) {
            pNode = preNode;
        } else {
            while (nDepth < nPreDepth) {
                pNode = pNode->pParentDir;
                nPreDepth--;
            }
            if (pNode) nRow++;
        }

        preNode = new Dir(sFullName, pNode, nCheck == 1 ? true : false);
        preNode->tFile.bLink = nLink == 1 ? true : false;
        preNode->nDepth = nDepth;
        preNode->nRow = nRow;

        if (pNode)
            pNode->vNode.push_back(preNode);
        else
            pNode = preNode;

        nPreDepth = nDepth;
    }

    fclose(pFile);

    if (pNode) {
        while (pNode->pParentDir)
            pNode = pNode->pParentDir;

        _pRoot = pNode;
    } else {
        return false;
    }

    SetOrder();
    return true;
}

bool Mcd::Save(const char *sFile) {
    FILE *pFile = NULL;

    if (sFile)
        pFile = fopen(sFile, "wb");

    if (!pFile) return false;

    fwrite(_sCode.c_str(), _sCode.size(), 1, pFile);

    for (pDirIterator i = _pOrder.begin(); i != _pOrder.end(); i++) {
        fputc((*i)->nDepth, pFile);
        fputc((*i)->bCheck == true ? 1 : 0, pFile);
        fputc((*i)->tFile.bLink == true ? 1 : 0, pFile);

        fputc((*i)->tFile.sFullName.size() / 256, pFile);
        fputc((*i)->tFile.sFullName.size() % 256, pFile);
        fputs((*i)->tFile.sFullName.c_str(), pFile);
    }

    fclose(pFile);
    return true;
}

///	\brief	tree 이하의 노드를 모두 지운다.
///	\param	tree		지우고자 하는 자식 노드
///	\param	bOrdering 	정렬 여부
void Mcd::Destroy(Dir *pTree, bool bOrdering) {// 비재귀
    if (pTree == NULL) return;
    if (_pOrder.empty()) return;

    int nDepth = pTree->nDepth;

    LOG_WRITE("_pOrder size [%d] [%d]", _pOrder.size(), nDepth);
    for (pDirIterator i = _pOrder.begin() + pTree->nIndex + 1; i != _pOrder.end(); i++) {
        LOG_WRITE("depth [%d] ndepth [%d] 1", (*i)->nDepth, nDepth);
        if (nDepth >= (*i)->nDepth) break;
        LOG_WRITE("depth [%d] ndepth [%d] 2", (*i)->nDepth, nDepth);
        delete *i;
    }

    LOG_WRITE("_pOrder size [%d] [%d] end", _pOrder.size(), nDepth);

    pTree->vNode.clear();
    pTree->bCheck = false;

    if (bOrdering) {
        SetOrder();
    }
}

///	\brief	str이 가르키는 directory로 현재 위치를 이동시킨다.
///	\param	str		현재 directory로 위치시키고자하는 directory명
void
Mcd::SetCur(const string &sPath) {
    //               01234567890123456789
    // m_pRoot->name = "/sdfsd/sdfsdsf/"
    //              "/sdfsd/sdfsdsf/p/3/"

    // str이 "" 일 경우 처음 위치로
    if (sPath.empty()) {
        _pCur = _pOrder.begin();
        return;
    }

    if (sPath == _pRoot->tFile.sFullName        // 루트일때
        || sPath.size() < _pRoot->tFile.sFullName.size())  // 루트사이즈보다 작을때
    {
        _pCur = _pOrder.begin();
        return;
    }

    // /
    // 012345678901234
    // /home/ioklo/ppp
    string sTmpPath = sPath;
    string sTmpPath2 = _pRoot->tFile.sFullName;
    if (sTmpPath.substr(0, 1) != "/") sTmpPath = "/" + sTmpPath;
    if (sTmpPath2.substr(0, 1) != "/") sTmpPath2 = "/" + sTmpPath2;

    //LOG_WRITE("MCD::SetCur [%s] [%s]", sPath.c_str(), sTmpPath.substr(sTmpPath2.size()).c_str());

    StringToken tStrToken(sTmpPath.substr(sTmpPath2.size()), "/");

    pDirIterator i;
    Dir *pNode = _pRoot;

    while (tStrToken.Next()) {
        const string &t = tStrToken.Get();

        // 자식들 중에 있는거 찾아낸다.
        for (i = pNode->vNode.begin(); i != pNode->vNode.end(); i++) {
            if ((*i)->tFile.sName == t) break;
        }

        if (i == pNode->vNode.end()) {
            _pCur = _pOrder.begin();
            return;
        }

        pNode = (*i);
    }

    _pCur = _pOrder.begin() + (*i)->nIndex;
}

///	\brief	mcd상에서 위 directory로 이동한다.
void
Mcd::Key_Up() {
    pDirIterator i;

    if ((*_pCur)->pParentDir) {
        if ((*_pCur)->nDepth != (*(_pCur - 1))->nDepth) {
            pDirIterator t;
            for (t = _pCur - 1; t != _pOrder.begin() - 1; t--)
                if ((*t)->nDepth == (*_pCur)->nDepth) break;

            if (t != _pOrder.begin() - 1) _pCur = t;
            else
                Key_Left();
        } else _pCur--;
    } else
        Key_Left();
}

///	\brief	mcd상에서 아래 directory로 이동한다.
void
Mcd::Key_Down() {
    // 아래부분에 뭔가 있을 경우
    if ((*_pCur)->pParentDir) {
        if (_pCur + 1 == _pOrder.end() || (*_pCur)->nDepth != (*(_pCur + 1))->nDepth) {
            pDirIterator t;
            for (t = _pCur + 1; t != _pOrder.end(); t++)
                if ((*t)->nDepth == (*_pCur)->nDepth) break;

            if (t != _pOrder.end())
                _pCur = t;
            else
                Key_Right();
        } else
            _pCur++;
    }
}

///	\brief	mcd상에서 왼쪽(상위 directory)로 이동한다.
void
Mcd::Key_Left() {
    if ((*_pCur)->pParentDir)
        _pCur = _pOrder.begin() + (*_pCur)->pParentDir->nIndex;
}

///	\brief	mcd상에서 오른쪽(하위 directory)로 이동한다.
void
Mcd::Key_Right() {
    if (!(*_pCur)->vNode.empty()) {
        _pCur = _pOrder.begin() + (*_pCur)->vNode.front()->nIndex;
        LOG_WRITE("Key_Right vNode empty [%s]", (*_pCur)->Path().c_str());
    } else {
        string p = (*_pCur)->Path();
        LOG_WRITE("Key_Right [%s]", p.c_str());
        Scan(*_pCur, 1);
        SetCur(p);
        if ((*_pCur)->vNode.empty()) {
            if (_pCur + 1 != _pOrder.end()) _pCur++;
        } else {
            if (!(*_pCur)->vNode.empty() && (*_pCur)->nDepth - _nSCol == 5) {
                _nSCol = _nSCol + 1;
            }
        }
    }
}

void
Mcd::Key_Home() {
    SetCur("/");
}

void
Mcd::Key_End() {
    while ((!(*_pCur)->vNode.empty())) {
        _pCur = _pOrder.begin() + (*_pCur)->vNode.front()->nIndex;
        LOG_WRITE("Key_Right vNode empty [%s]", (*_pCur)->Path().c_str());
    }
}

///	\brief	mcd 정렬을 한다.
void
Mcd::SetOrder() {
    vector<Dir *> vDir;
    Dir *pDirNode;
    int ind = 0;
    int row = 0, odepth = -1;

    _pOrder.clear();

    vDir.push_back(_pRoot);
    while (!vDir.empty()) {
        pDirNode = vDir.back();
        vDir.pop_back();

        if (pDirNode->nDepth <= odepth) row++;
        odepth = pDirNode->nDepth;

        pDirNode->nRow = row;
        pDirNode->nIndex = ind;

        ind++;
        _pOrder.push_back(pDirNode);
        copy(pDirNode->vNode.rbegin(), pDirNode->vNode.rend(), back_inserter(vDir));
    }

    _pCur = _pOrder.begin();
}

///	\brief	directory을 mcd에 추가한다.
///	\param	_dir	추가할 directory 명
bool Mcd::AddDirectory(const string &sDir) {
    if (!_pRoot) {
        MsgBox("Error", "Root directory is NULL");
        return false;
    }

    Dir *pDir = _pRoot;

    StringToken st(sDir, "/"); // /을 중심으로 계속 끊어줌

    while (st.Next()) {
        const string &sToken = st.Get();

        //LOG_WRITE("Token :: [%s]", sToken.c_str());

        pDirIterator i;

        for (i = pDir->vNode.begin(); i != pDir->vNode.end(); i++) {
            if ((*i)->tFile.sName == sToken) {
                Scan((*i), 1);
                break;
            }
        }

        if (i == pDir->vNode.end()) {// 못찾았을 경우
            //LOG_WRITE("Not Find...!!! [%s]", sToken.c_str());

            string::size_type p = sDir.find(sToken);
            string sPath;
            if (p == string::npos)
                sPath = sDir;
            else
                sPath = sDir.substr(0, p + sToken.size());

            Dir *pNewDir = new Dir(sPath, pDir);

            pDir->vNode.push_back(pNewDir);

            if (_bMcdSort)
                sort(pDir->vNode.begin(), pDir->vNode.end(), McdSort());

            // 디렉토리 많아질 수록 느려짐
            SetOrder();

            // 현재 디렉토리에 있는 디렉토리 검색
            Scan(pNewDir, 2);

            pDir = pNewDir;
        } else {// 찾았을 경우
            pDir = (*i);
        }
    }
    return true;
}

///	\brief	directory를 전체를 재 검색한다.
///	\return	0을 반환
int Mcd::Rescan(int nDepth) {
    if (_pRoot) {
        for (pDirIterator p = _pOrder.begin(); p != _pOrder.end(); ++p)
            delete *p;

        _pRoot = NULL;
    }

    File tFile;
    tFile.sFullName = "/";
    tFile.sName = "/";
    tFile.bLink = false;
    tFile.bDir = true;
    _pRoot = new Dir(tFile, NULL);
    return Scan(_pRoot, nDepth); // 루트를 중심으로 다시 스캔
}

///	\brief	tree로 넘어온 디렉토리에 서브디렉토리를 추가한다
///	\param	arg			directory 인자
///	\param	nDepth		디렉토리 검색 깊이
///	\return	0을 반환
int Mcd::Scan(Dir *pDir, int nDepth) {
    // 비재귀판
    pDirContainer vDir;
    Dir *pTree;

    int nDepthCount = 1;

    if (_pReader == NULL) {
        MsgBox("Error", "pReader is NULL");
        return -1;
    }

    string sBefPath = _pReader->GetPath();

    //Destroy(pDir, false);	// 구성요소들을 destroy.. 시킨다.

    vDir.push_back(pDir);

    bool bBreak = false;
    bool bNotSave = false;

    while (!vDir.empty()) {
        pTree = vDir.back();
        vDir.pop_back();

        // Linked 되어 있으면 그냥 넘긴다.
        //LOG_WRITE("Node Search [%d]", pTree->tFile.bLink);

        //if (pTree->tFile.bLink) continue;

        //LOG_WRITE("Node Search [%s]", pTree->Path().c_str());
        if (_pReader->Read(pTree->Path()) == false) {
            //LOG_WRITE("Node Search False [%s]", pTree->Path().c_str());
            continue;
        }

        while (_pReader->Next()) {
            File tFile;
            if (!_pReader->GetInfo(tFile)) continue;

            // ESC를 누르면 STOP 된다.
            if (GetChar(true) == 27) {
                bBreak = true;
                break;
            }

            if (tFile.sName == "." || tFile.sName == "..")
                continue;

            if (tFile.bDir && !tFile.bLink) {
                if (_bHidden == false)
                    if (tFile.sName.substr(0, 1) == ".")
                        continue;

                bNotSave = false;
                for (pDirIterator i = pTree->vNode.begin(); i != pTree->vNode.end(); ++i) {
                    if (tFile.sFullName == (*i)->tFile.sFullName) {
                        bNotSave = true;
                        break;
                    }
                }

                if (!bNotSave)
                    pTree->vNode.push_back(new Dir(tFile, pTree));
                //LOG_WRITE("Node Insert [%s]", tFile.sFullName.c_str());
            }
        }

        // 이 디렉토리를 체크 했는가?
        pTree->bCheck = true;

        // tree 를 정렬한다.
        if (_bMcdSort)
            sort(pTree->vNode.begin(), pTree->vNode.end(), McdSort());

        // 스택에 노드들을 채운다.. 순서는 반대로 한다.
        if (nDepth != nDepthCount) {
            nDepthCount++;
            copy(pTree->vNode.rbegin(), pTree->vNode.rend(), back_inserter(vDir));
        }

        if (bBreak == true) break;
    }

    SetOrder();
    // 함수를 실행하기 전 디렉토리로 이동.

    _pReader->Read(sBefPath);
    return 0;
}

///	\brief	sFullPath  같은 directory를 찾는다.
///	\return	-1	:	찾지 못했음
///			양수:	찾은 index
int
Mcd::Search(const string &sPath, int nNextNum) {
    pDirIterator i = _pOrder.begin(), end = _pOrder.end();
    string p = sPath;

    int nCount = 0;

    while (i != end) {
        Dir &rd = *(*i++);
        if (rd.tFile.sName.size() < sPath.size()) continue;
        if (rd.tFile.sName.substr(0, sPath.size()) == p) {
            if (nNextNum <= nCount) return rd.nIndex;
            nCount++;
        }
    }

    // 없으면
    return -1;
}

int Mcd::SearchProcess(KeyInfo &tKeyInfo) {
    int key = (int) tKeyInfo;
    if (!_bSearch && (isalnum(key) || key == '.')) _bSearch = true;
    if (_bSearch && ((32 <= key && key <= 126) || tKeyInfo.sKeyName == "BS" || key == '\t')) {
        // search mode;
        // 뒷부분에 key를 넣고
        if (tKeyInfo.sKeyName == "BS") {
            _sStrSearch.erase(_sStrSearch.size() - 1, 1);
            if (_sStrSearch.empty()) {
                _bSearch = false;
                return true;
            }
        } else if (key == '\t')
            _nSearchCnt++;
        else
            _sStrSearch += (char) key;

        int nTmp = 0;
        // m_sSearch로 시작하는 문자를 찾는다.
        if ((nTmp = Search(_sStrSearch, _nSearchCnt)) != -1)
            _pCur = _pOrder.begin() + nTmp;
        else
            _nSearchCnt = 0;

        return true;
    }

    _bSearch = false;
    _sStrSearch.erase();
    return false;
}

int Mcd::SubDirOneSearch() {
    string sCurPath = (*_pCur)->tFile.sFullName;
    Destroy(*_pCur, false);
    Scan(*_pCur, 1);
    SetCur(sCurPath);
    return 0;
}

int Mcd::SubDirAnySearch(int nDepth) {
    string sCurPath = (*_pCur)->tFile.sFullName;
    Destroy(*_pCur, false);
    Scan(*_pCur, nDepth);
    SetCur(sCurPath);
    return 0;
}

int Mcd::SubDirAllSearch() {
    void *pWait = MsgWaitBox(_("Wait"),
                             _("Please wait !!! - Cancel Key [ESC]"));
    string sCurPath = (*_pCur)->tFile.sFullName;
    Destroy(*_pCur, false);
    Scan(*_pCur, 0);
    SetCur(sCurPath);
    MsgWaitEnd(pWait);
    return 0;
}

int Mcd::SubDirClear() {
    string sCurPath = (*_pCur)->tFile.sFullName;
    Destroy(*_pCur);
    SetCur(sCurPath);
    return 0;
}

void Mcd::MountList() {
    vector<LineArgData> vLineArgData;
    vector<string> vViewList;
    vector<string> vMountList;

    if (_pReader == NULL || _pReader->GetReaderName() != "file") {
        MsgBox(_("Error"), _("mounted list local system use only."));
        return;
    }

    if (CmdShell::CmdExeArg("mount", 0, vLineArgData, false) == -1) {
        LOG_WRITE("CmdExeArg Error !!!");
        return;
    }

    String sView;

    for (uint n = 0; n < vLineArgData.size(); n++) {
        LineArgData vArgData = vLineArgData[n];

        if (vArgData.size() > 5) {
            if (vArgData[0].substr(0, 4) == "/dev") {
                sView.setEmpty();
                sView.appendBlank(10, "%s", vArgData[4].c_str());
                sView.appendBlank(14, "%s", vArgData[0].c_str());
                sView.append(" %s", vArgData[2].c_str());

                vViewList.push_back(sView.c_str());
                vMountList.push_back(vArgData[2]);
            }
        }
    }

    if (vViewList.size() == 0) return;

    int nCho = SelectBox(_("go to the mounted directory."), vViewList, 0);
    if (nCho == -1) return;

    if ((int) vMountList.size() > nCho) {
        AddDirectory(vMountList[nCho]);
        SetCur(vMountList[nCho]);
    }
}
