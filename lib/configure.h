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

/// @brief    Entry 으로 비교하는 class
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

        void setValue(const string &section,
                      const string &var,
                      const string &val,
                      bool bSave = true);

        void SetValue(const string &var,
                      const string &val,
                      bool /*bSave*/ = true) {
            setValue("", var, val, true);
        }

        void setStaticValue(const std::string &section,
                            const std::string &var,
                            const std::string &val) {
            setValue(section, var, val, false);
        }

        void setStaticValue(const std::string &var, const std::string &val) {
            setValue("Static", var, val, false);
        }

        string getValue(const string &section,
                        const string &var,
                        const string &def = "");

        int GetValueNum(const string &section,
                        const string &var,
                        int nDef = 0) {
            string str = getValue(section, var);
            if (str != "")
                return strtoint(str);
            return nDef;
        }

        bool getBool(const string &section, const string &var, bool def = false);

        void setBool(const std::string &section,
                     const std::string &var,
                     bool value,
                     bool bSave = true) {
            if (value) setValue(section, var, "On", bSave);
            else setValue(section, var, "Off", bSave);
        }

        void setBool(const string &var, bool bBool, bool bSave = false) {
            setBool("", var, bBool, bSave);
        }
    };

};

#endif
