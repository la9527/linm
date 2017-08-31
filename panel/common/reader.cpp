/******************************************************************************
 *   Copyright (C) 2008 by la9527                                             *
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

#include "file.h"
#include "reader.h"
#include "selection.h"
#include "strutil.h"
#include "sortfile.h"

using namespace MLS;
using namespace MLSUTIL;

bool Reader::Copy(Selection &tSelection, const string &strTargetPath, Selection *pSelection) {
    vector<File *> vFiles;

    ullong uFileSize = tSelection.CalcSize();
    string sSize = toregular(uFileSize), sRestSize;

    vFiles = tSelection.GetData();

    if (strTargetPath.size() == 0) {
        LOG_WRITE("target path is null");
        return false;
    }

    string strTargetPathTmp = strTargetPath;

    if (strTargetPathTmp.substr(strTargetPathTmp.size() - 1, 1) != "/")
        strTargetPathTmp = strTargetPathTmp + "/";

    _pProgress->init();
    _pProgress->setTitle("Copy to " + strTargetPath);

    if (pSelection != NULL) {
        pSelection->Clear();
        pSelection->SetSelectPath(strTargetPathTmp);
    }

    _pProgress->showProgBar();

    string strSourcePath = tSelection.GetSelectPath();

    string strTargetName;
    bool bCopyRt = true;
    RtStateInfo tStateInfo;

    // 파일 복사
    for (int n = 0; n < (int) vFiles.size(); n++) {
        File *pFile = vFiles[n];

        // 파일 이름이 없을경우 continue
        if (_pProgress->isExit()) {
            _pProgress->setExitInit();
            _pProgress->hideProgBar();

            if (YNBox(_("Warning"), _("Do you want to stop copy operation?"), false) == true)
                break;

            _pProgress->showProgBar();
        }

        //LOG( "copyDir fullname [%s] sourcepath [%d] [%s]", tFile.strFullName.c_str(), strSourcePath.size(), strSourcePath.c_str() );
        strTargetName = strTargetPathTmp + pFile->sFullName.substr(strSourcePath.size());

        string strTargetNewName;

        _pProgress->setLeftStr(pFile->sName);
        _pProgress->setRightStr(toregular(n + 1) + "/" + toregular(vFiles.size()));

        LOG_WRITE("Copy :: [%s]", pFile->sFullName.c_str());

        tStateInfo.nCount = n + 1;

        File tFileNew;
        if (this->Copy(*pFile, strTargetName, &tFileNew, &tStateInfo) >= 0) {
            if (pSelection != NULL) {
                File *pFileNew = new File;
                *pFileNew = tFileNew;
                pSelection->Add(pFileNew);
            }
        } else {
            LOG_WRITE("this->copy failure !!!  -- 1");
            bCopyRt = false;
        }

        usleep(10);

        if (tStateInfo.nStatInfo == OPER_SKIP_ALL) {
            LOG_WRITE("this->copy failure !!!  -- 2");
            bCopyRt = false;
            break;
        }
    }

    if (pSelection)
        pSelection->SetSelectPath(strTargetPathTmp);

    _pProgress->hideProgBar();
    return bCopyRt;
}


bool Reader::Move(Selection &tSelection, const string &sTargetPath, Selection *pSelection) {
    bool bFilesCopyRemove = false;

    string strTargetName;
    vector<File *> vFiles;

    ullong uFileSize = tSelection.CalcSize();
    string sSize = toregular(uFileSize), sRestSize;

    vFiles = tSelection.GetData();

    if (sTargetPath.size() == 0) {
        LOG_WRITE("target path is null");
        return false;
    }

    string strTargetPathTmp = sTargetPath;

    if (strTargetPathTmp.substr(strTargetPathTmp.size() - 1, 1) != "/")
        strTargetPathTmp = strTargetPathTmp + "/";

    _pProgress->init();
    _pProgress->setTitle(_("Move to ") + strTargetPathTmp);

    if (pSelection != NULL) {
        pSelection->Clear();
        pSelection->SetSelectPath(strTargetPathTmp);
    }

    _pProgress->showProgBar();

    string strSourcePath = tSelection.GetSelectPath();

    vector<File *>::iterator begin = vFiles.begin(), end = vFiles.end();
    sort(begin, end, sort_dir_adaptor<sort_fullname_length, sort_fullname_length>());

    RtStateInfo tStateInfo;
    tStateInfo.llAllBytes = uFileSize;
    // 파일 이동
    for (int n = 0; n < (int) vFiles.size(); n++) {
        File *pFile = vFiles[n];

        usleep(10);

        // 하부 파일들은 이동하지 않는다.
        string sTmpName = pFile->sFullName.substr(strSourcePath.size());
        string::size_type p = sTmpName.find("/");
        if (p != string::npos)
            if (p != sTmpName.length() - 1)
                continue;

        //LOG_WRITE( "Move [%s]", tFile.sFullName.c_str() );

        strTargetName = strTargetPathTmp + pFile->sFullName.substr(strSourcePath.size());

        string strTargetNewName;

        _pProgress->setOneValue(0);

        File tTargetFile;

        int nRt = this->Move(*pFile, strTargetName, &tTargetFile, &tStateInfo);

        if (nRt == 1 && pSelection != NULL) {
            File *pFileNew = new File;
            *pFileNew = tTargetFile;
            pSelection->Add(pFileNew);
        }

        _pProgress->setOneValue(100);

        _pProgress->setLeftStr(pFile->sName);
        _pProgress->setRightStr(toregular(n + 1) + "/" + toregular(vFiles.size()));
        _pProgress->setRightStr2(toregular(tStateInfo.llCurBytes) + "/" + toregular(tStateInfo.llAllBytes));

        if (tStateInfo.llCurBytes && tStateInfo.llAllBytes)
            _pProgress->setTwoValue(tStateInfo.llCurBytes * 100 / tStateInfo.llAllBytes);

        if (nRt == 0) {
            bFilesCopyRemove = true;
            break;
        }

        if (tStateInfo.nStatInfo == OPER_SKIP_ALL) break;
    }

    if (bFilesCopyRemove) {
        // EXDEV에 대해서 복사/삭제 오퍼레이션으로 전환.
        if (this->Copy(tSelection, strTargetPathTmp))
            this->Remove(tSelection);
    }

    if (pSelection)
        pSelection->SetSelectPath(strTargetPathTmp);

    _pProgress->hideProgBar();
    return true;
}

bool Reader::Remove(Selection &tSelection, bool bMsgShow, bool bIgnore) {
    uint uSize = tSelection.CalcSize();
    string sTargetName;

    vector<string> dir_stack;
    vector<File *> vFiles;

    vFiles = tSelection.GetData();

    _pProgress->init();
    _pProgress->setTitle("Remove files");

    vector<File *> vLink, vFile, vDirs;

    // 파일 이동
    for (int t = 0; t < (int) vFiles.size(); t++) {
        File *pFile = vFiles[t];

        if (!pFile->bDir || pFile->bLink)
            vFile.push_back(pFile);
        else
            vDirs.push_back(pFile);
    }

    int nCount = 1;

    _pProgress->setRightStr2("");
    if (nCount && uSize)
        _pProgress->setOneValue(nCount * 100 / vFiles.size());

    // 파일 긴것부터 삭제.
    vector<File *>::iterator begin = vFile.begin(), end = vFile.end();
    sort(begin, end, sort_dir_adaptor<sort_fullname_length, sort_fullname_length>());
    begin = vDirs.begin();
    end = vDirs.end();
    sort(begin, end, sort_dir_adaptor<sort_fullname_length, sort_fullname_length>());

    RtStateInfo tStateInfo;

    if (bIgnore)
        tStateInfo.nStatInfo = OPER_OVERWRITE_ALL;

    for (int n = 0; n < (int) vFile.size(); n++) {
        //LOG_WRITE( "File Remove [%s]", vFile[n].strFullName.c_str() );

        usleep(10);

        if (this->Remove(*vFile[n], &tStateInfo) == -1)
            return false;

        nCount++;

        string strCount = toregular(nCount) + "/" + toregular(vFiles.size());

        _pProgress->setLeftStr(vFile[n]->sName);
        _pProgress->setRightStr(strCount);
        if (nCount && uSize)
            _pProgress->setOneValue(nCount * 100 / (int) vFiles.size());
    }

    for (int n = 0; n < (int) vDirs.size(); n++) {
        //LOG( "Dir Remove [%s]", vDirs[n].strFullName.c_str() );

        usleep(10);

        if (this->Remove(*vDirs[n], &tStateInfo) == -1)
            return false;

        nCount++;
        string strCount = toregular(nCount) + "/" + toregular(vFiles.size());

        _pProgress->setLeftStr(vDirs[n]->sName);
        _pProgress->setRightStr(strCount);
        if (nCount && uSize)
            _pProgress->setOneValue(nCount * 100 / (int) vFiles.size());
    }
    return true;
}

