/***************************************************************************
 *   Copyright (C) 2004 by Byoungyoung, La                                 *
 *   la9527@yahoo.co.kr                                                    *
 ***************************************************************************/

#include "editorcmd.h"
#include "editor.h"

namespace MLS {
    EditorCmd *g_pEditorCmd;

    void EditorCmd::Init(Editor *pEditor) {
        _pEditor = pEditor;

        _func["left"] = &Editor::Key_Left;
        _func["right"] = &Editor::Key_Right;
        _func["up"] = &Editor::Key_Up;
        _func["down"] = &Editor::Key_Down;

        _func["shift_left"] = &Editor::Key_ShiftLeft;
        _func["shift_right"] = &Editor::Key_ShiftRight;
        _func["shift_up"] = &Editor::Key_ShiftUp;
        _func["shift_down"] = &Editor::Key_ShiftDown;

        _func["home"] = &Editor::Key_Home;
        _func["end"] = &Editor::Key_End;
        _func["pgup"] = &Editor::Key_PgUp;
        _func["pgdn"] = &Editor::Key_PgDn;

        _func["ins"] = &Editor::Key_Ins;
        _func["del"] = &Editor::Key_Del;
        _func["bs"] = &Editor::Key_BS;
        _func["tab"] = &Editor::Key_Tab;

        _func["enter"] = &Editor::Key_Enter;

        _func["select"] = &Editor::Select;
        _func["selectall"] = &Editor::SelectAll;
        _func["copy"] = &Editor::Copy;
        _func["cut"] = &Editor::Cut;
        _func["paste"] = &Editor::Paste;
        _func["undo"] = &Editor::Undo;
        _func["esc"] = &Editor::Key_ESC;

        _func["indent"] = &Editor::Key_Tab;
        _func["unindent"] = &Editor::Key_Untab;
        _func["indentmode"] = &Editor::IndentMode;

        _func["refresh"] = &Editor::Refresh;

        _func["mouse"] = &Editor::Key_Mouse;
        _func["menu"] = &Editor::Menu;

        _func["gotoline"] = &Editor::GotoLine;
        _func["gotofirst"] = &Editor::GotoFirst;
        _func["gotoend"] = &Editor::GotoEnd;

        _func["find"] = &Editor::Find;
        _func["findnext"] = &Editor::FindNext;
        _func["findprevious"] = &Editor::FindPrevious;

        _func["linenumberview"] = &Editor::LineNumberView;

        _func["filenew"] = &Editor::FileNew;
        _func["filesave"] = &Editor::FileSave;
        _func["filesaveas"] = &Editor::FileSaveAs;

        _func["help"] = &Editor::Help;
        _func["about"] = &Editor::About;
        _func["exit"] = &Editor::Exit;
    }

    void EditorMenu::MenuRefresh() {
        g_pEditor->Refresh();
    }

    void EditorMenu::MenuAddCatecory() {
        MenuCategory File(gettext("File"));
        File.AddItem(gettext("New"), "<filenew>");
        File.AddItem("", "");
        File.AddItem(gettext("Save"), "<filesave>");
        File.AddItem(gettext("Save As"), "<filesaveas>");
        File.AddItem("", "");
        File.AddItem(gettext("Quit"), "<exit>");
        AddItem(File);

        MenuCategory Edit(gettext("Edit"));
        Edit.AddItem(gettext("Undo"), "<undo>");
        Edit.AddItem("", "");
        Edit.AddItem(gettext("Cut"), "<cut>");
        Edit.AddItem(gettext("Copy"), "<copy>");
        Edit.AddItem(gettext("Paste"), "<paste>");
        Edit.AddItem("", "");
        Edit.AddItem(gettext("Indent"), "<indent>");
        Edit.AddItem(gettext("Unindent"), "<unindent>");
        Edit.AddItem("", "");
        if (g_pEditor->GetIndentMode())
            Edit.AddItem(gettext("Indent Off"), "<indentmode>");
        else
            Edit.AddItem(gettext("Indent On"), "<indentmode>");
        Edit.AddItem("", "");
        Edit.AddItem(gettext("Select All"), "<selectall>");
        Edit.AddItem("", "");
        Edit.AddItem(gettext("Select Mode"), "<select>");
        AddItem(Edit);

        MenuCategory Find(gettext("Find"));
        Find.AddItem(gettext("Find"), "<find>");
        Find.AddItem("", "");
        Find.AddItem(gettext("Find Next"), "<findnext>");
        Find.AddItem(gettext("Find Previous"), "<findprevious>");
        AddItem(Find);

        MenuCategory View(gettext("View"));
        View.AddItem(gettext("Refresh"), "<refresh>");
        View.AddItem("", "");
        View.AddItem(gettext("Show line numbers"), "<linenumberview>");
        View.AddItem("", "");
        View.AddItem(gettext("Goto Line"), "<gotoline>");
        View.AddItem("", "");
        View.AddItem(gettext("Goto First"), "<gotofirst>");
        View.AddItem(gettext("Goto End"), "<gotoend>");
        AddItem(View);

        MenuCategory Help(gettext("Help"));
        Help.AddItem(gettext("About"), "<about>");
        Help.AddItem("", "");
        Help.AddItem(gettext("Help"), "<help>");
        AddItem(Help);
    }

};
