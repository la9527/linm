SET(intl_STAT_HEADERS
    gmo.h
    gettextP.h
    hash-string.h
    loadinfo.h
    plural-exp.h
    eval-plural.h
    localcharset.h
    lock.h
    relocatable.h
    tsearch.h tsearch.c
    verify.h
    xsize.h
    printf-args.h printf-args.c
    printf-parse.h wprintf-parse.h printf-parse.c
    vasnprintf.h vasnwprintf.h vasnprintf.c
    os2compat.h
    libgnuintl.in.h)

SET(intl_STAT_LIBS
    bindtextdom.c
    dcgettext.c
    dgettext.c
    gettext.c
    finddomain.c
    hash-string.c
    loadmsgcat.c
    localealias.c
    textdomain.c
    l10nflist.c
    explodename.c
    dcigettext.c
    dcngettext.c
    dngettext.c
    ngettext.c
    plural.y
    plural-exp.c
    localcharset.c
    threadlib.c
    lock.c
    relocatable.c
    langprefs.c
    localename.c
    log.c
    printf.c
    setlocale.c
    version.c
    xsize.c
    osdep.c
    os2compat.c
    intl-exports.c
    intl-compat.c)

add_library(intl STATIC ${intl_STAT_LIBS} ${intl_STAT_HEADERS})
