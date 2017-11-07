#include "define.h"
#include "configure.h"

using namespace std;

namespace MLSUTIL {

    Configure::Configure() {
    }

    Configure::~Configure() {
    }

    void Configure::Init() {
    }

    void Configure::Clear() {
        Init();
        _EnvMap.clear();
    }

    bool Configure::Load(const string &sFilename) {
        if (sFilename.size() == 0) return false;

        ifstream in(sFilename.c_str());
        if (!in) return false;

        Init();
        _sFilename = sFilename;

        string line, var, val, section, name;

        while (!getline(in, line).eof()) {
            if (line.empty()) continue;
            if (Tolower(line.substr(0, 10)) == "#!version ") {
                _sVersion = Tolower(chop(line.substr(10)));
                continue;
            }

            if (line[0] == '#') continue;
            if (line[0] == '[') {
                section = getbetween(line, '[', ']');
                continue;
            }

            string::size_type p = line.find('=');
            if (p == string::npos) continue;

            // . file the variable.
            var = chop(line.substr(0, p));
            val = chop(line.substr(p + 1));

            if (var == "version") {
                _sVersion = val;
                continue;
            }

            if (section != "")
                name = Tolower(section) + "." + Tolower(var);
            else
                name = Tolower(var);

            if (Parsing(section, var, val) == false) {
                _EnvMap[name] = Entry(var, val, section, false);
            }
        }
        return true;
    }

    bool Configure::Save(const string &sFilename) {// save the current file.
        string bakfile = _sFilename, srcfile;

        if (sFilename.size() == 0 || bakfile == sFilename) {
            bakfile = bakfile + ".bak";

            unlink(bakfile.c_str());
            if (rename(_sFilename.c_str(), bakfile.c_str()) == -1) {
                LOG("BackFile rename failure !!! - %s [%d][%s]",
                          bakfile.c_str(), errno, strerror(errno));
                return false;
            }
            srcfile = _sFilename;
        } else {

            srcfile = sFilename;
        }

        ifstream in(bakfile.c_str());
        ofstream out(srcfile.c_str());

        if (!out) {
            LOG("file write failure !!! - %s", srcfile.c_str());
            return false;
        }

        SaveParcing();

        // Modified
        MapType::iterator i = _EnvMap.begin(), end = _EnvMap.end(), j;
        MapType mod_list, dup_list;

        for (i = _EnvMap.begin(); i != end; ++i)
            if (i->second.modified)
                mod_list.insert(*i);

        string line, var, val, section, name, lower_name;
        bool bFirstSection = false;

        if (in) {
            while (!getline(in, line).eof()) {
                if (line.empty() || line[0] == '#' || line[0] == '$') {
                    out << line << endl;
                    continue;
                }

                if (line[0] == '[') {
                    section = getbetween(line, '[', ']');

                    bool bSection = false;
                    for (i = _EnvMap.begin(); i != end; ++i)
                        if (i->second.section == section) {
                            bSection = true;
                            break;
                        }

                    if (bSection)
                        out << line << endl;

                    bFirstSection = true;
                    //else
                    //	out << '#' << line << endl;
                    continue;
                }

                string::size_type p = line.find('=');
                if (p == string::npos) {
                    out << line << endl;
                    continue;
                }

                var = chop(line.substr(0, p));
                val = chop(line.substr(p + 1));
                if (section != "")
                    name = section + "." + var;
                else
                    name = var;

                lower_name = Tolower(name);
                i = mod_list.find(lower_name);
                j = _EnvMap.find(lower_name);

                if (i != mod_list.end()) {
                    if (!bFirstSection) {
                        out << "[" << i->second.section.c_str() << "]" << endl;
                        bFirstSection = true;
                    }
                    out << i->second.var.c_str() << " = " << i->second.val.c_str() << endl;
                    dup_list.insert(*i);
                    mod_list.erase(i);
                    continue;
                } else if ((i = dup_list.find(lower_name)) != dup_list.end()) {
                    out << '#' << line << endl;
                    continue;
                }

                if (j != _EnvMap.end())
                    out << line << endl;
            }
        }

        vector<Entry> vEntry;

        for (i = mod_list.begin(); i != mod_list.end(); i++) {
            vEntry.push_back(i->second);
        }

        sort(vEntry.begin(), vEntry.end(), sort_Entry());

        Entry tEntry;
        string sSection = "";

        for (int n = 0; n < (int) vEntry.size(); n++) {
            tEntry = vEntry[n];
            if (sSection != tEntry.section) {
                if (tEntry.section != "")
                    out << endl << "[" << tEntry.section.c_str() << "]" << endl;
            }
            sSection = tEntry.section;

            out << tEntry.var.c_str() << " = " << tEntry.val.c_str() << endl;

            LOG("Section [%s] [%s] [%s]", tEntry.section.c_str(), tEntry.var.c_str(), tEntry.val.c_str());
        }

        if (in) in.close();
        out.close();

        if (sFilename != bakfile)
            remove(bakfile.c_str());
        return true;
    }

    string Configure::GetValue(const string &section, const string &var, const string &def) {
        MapType::iterator i = _EnvMap.begin(), end = _EnvMap.end();
        MapType::iterator j;
        MapType sectionlist;
        string name;

        if (section != "") {
            for (; i != end; ++i)
                if (i->first.find('.') != string::npos)
                    if (i->first.substr(0, section.size()) == Tolower(section))
                        sectionlist.insert(*i);

            name = section + "." + var;
            j = sectionlist.find(Tolower(name));
            if (j == sectionlist.end()) {
                LOG("not find config var [%s] section [%s] ", var.c_str(), section.c_str());
                return def;
            }
        } else {
            j = _EnvMap.find(Tolower(var));
            if (j == _EnvMap.end()) {
                for (; i != end; ++i)
                    if (i->first.find('.') != string::npos)
                        if (i->first.substr(i->first.find('.') + 1) == Tolower(var))
                            return i->second.val;
                LOG("not find config var [%s]", var.c_str());
                return def;
            }
        }

        Entry tEntry = j->second;

        //cout << "GetValue :: " << tEntry.m_sVal.c_str() << endl;
        return tEntry.val;
    }

    void Configure::SetValue(const string &section,
                             const string &var,
                             const string &val,
                             bool bSave) {
        string v;
        if (section == "")
            v = Tolower(var);
        else
            v = Tolower(section) + "." + Tolower(var);

        MapType::iterator i = _EnvMap.find(v);

        LOG("SetValue :: [%s] [%s] [%s]", section.c_str(), var.c_str(), val.c_str());

        if (i == _EnvMap.end())
            _EnvMap.insert(MapType::value_type(v, Entry(var, val, section, bSave)));
        else {
            if (!i->second.modified)
                i->second.modified = (bSave && (i->second.val != val));
            i->second.val = val;
        }
    }

    bool Configure::GetBool(const std::string &section, const std::string &var, bool def) {
        string sValue = GetValue(section, var);
        if (sValue.size() == 0)
            return def;
        return (Tolower(sValue) == "on");
    }

};
