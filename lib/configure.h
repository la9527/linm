#ifndef MLSCONFIGURE_H
#define MLSCONFIGURE_H

#include "define.h"
#include "exception.h"
#include "strutil.h"

namespace MLSUTIL {

    class Entry {
    public:
        Entry() : modified(false), deleted(false) {}

        Entry(const std::string &_var,
              const std::string &_val,
              const std::string &_section,
              bool b)
                : var(_var), val(_val), section(_section), modified(b), deleted(false) {}

        std::string var;
        std::string val;
        std::string section;
        bool modified;
        bool deleted;
    };

/// @brief    Entry compare class
    class sort_Entry {
    public:
        bool operator()(Entry a, Entry b) {
            return a.section < b.section;
        }
    };

    class Configure {
    private:
        typedef std::map<std::string, Entry> MapType;

        std::string _sVersion;
        std::string _sFilename;

    protected:
        MapType _EnvMap;

    protected:
        virtual bool Parsing(const string &section,
                             const string &var,
                             const string &val) = 0;

        virtual bool SaveParcing() { return true; }

    public:
        Configure();

        virtual ~Configure();

        virtual void Init();

        virtual void Clear();

        bool Load(const string &sFile);

        bool Save(const string &sFile = "");

        const string &getVersion() const { return _sVersion; }

        void SetValue(const string &section,
                      const string &var,
                      const string &val,
                      bool bSave = true);

        void SetValue(const string &var,
                      const string &val,
                      bool /*bSave*/ = true) {
            SetValue("", var, val, true);
        }

        void SetStaticValue(const std::string &section,
                            const std::string &var,
                            const std::string &val) {
            SetValue(section, var, val, false);
        }

        void SetStaticValue(const std::string &var, const std::string &val) {
            SetValue("Static", var, val, false);
        }

        string GetValue(const string &section,
                        const string &var,
                        const string &def = "");

        int GetValueNum(const string &section,
                        const string &var,
                        int nDef = 0) {
            string str = GetValue(section, var);
            if (str != "")
                return strtoint(str);
            return nDef;
        }

        bool GetBool(const string &section, const string &var, bool def = false);

        void SetBool(const std::string &section,
                     const std::string &var,
                     bool value,
                     bool bSave = true) {
            if (value) SetValue(section, var, "On", bSave);
            else SetValue(section, var, "Off", bSave);
        }

        void SetBool(const string &var, bool bBool, bool bSave = false) {
            SetBool("", var, bBool, bSave);
        }
    };

};

#endif
