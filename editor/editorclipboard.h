/***************************************************************************
 *   Copyright (C) 2004 by Byoungyoung, La                                 *
 *   la9527@yahoo.co.kr                                                    *
 *                                                                         *
 ***************************************************************************/

#ifndef __EditorClipBoard_h__
#define __EditorClipBoard_h__

#include "define.h"
#include "editorclipboard.h"

using namespace std;

namespace MLS {

    enum EditClip {
        Edit_ClipCopy,
        Edit_ClipCut,
        Edit_ClipNone
    };

    class EditorClipboard {
        vector<wstring> _vClip;
        EditClip _ClipStatus;

    public:
        EditorClipboard() : _ClipStatus(Edit_ClipNone) {}

        void Set(vector<wstring> &sSet, EditClip ClipStatus = Edit_ClipNone) {
            _vClip = sSet;
            _ClipStatus = ClipStatus;
        }

        vector<wstring> Get() {
            return _vClip;
        }

        static EditorClipboard &GetInstance() {
            static EditorClipboard tEditorClipboard;
            return tEditorClipboard;
        }
    };

#define g_tEditorClip    EditorClipboard::GetInstance()

/// Undo, Redo information save class
    class DoInfo {
    public:
        DoInfo(int y, int x, vector<wstring> Data, int DelSize = 0) :
                nLine(y), nCulumn(x), vData(Data), nDelSize(DelSize) {}

        DoInfo(int y, int x, int nDel, wstring sWStr) :
                nLine(y), nCulumn(x), nDelSize(nDel) {
            vData.push_back(sWStr);
        }

        DoInfo(int y, int x, wstring sWStr, wstring sWStr2) :
                nLine(y), nCulumn(x), nDelSize(0) {
            vData.push_back(sWStr);
            vData.push_back(sWStr2);
        }

        DoInfo(int y, int x, wstring sWStr) :
                nLine(y), nCulumn(x), nDelSize(0) {
            vData.push_back(sWStr);
        }

    public:
        int nLine;        /// line position
        int nCulumn;    /// culumn poistion
        vector<wstring> vData;        /// line data
        int nDelSize;    ///	delete size. if nDelSize is 0, not insert lines.
    };

};

#endif
