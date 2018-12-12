#-------------------------------------------------
#
# Project created by QtCreator 2018-09-04T18:10:27
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG+=c++11
TARGET = subway
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    login_dialog.cpp \
    account.cpp \
    subway.cpp \
    line.cpp \
    station.cpp \
    dialog_add_line.cpp \
    stt_nor.cpp \
    stt_adm.cpp \
    line_head.cpp \
    in_use_family.cpp \
    un_use_family.cpp \
    dialog_move.cpp \
    find.cpp \
    same_st_head.cpp \
    same_st_family.cpp \
    st_tr_info.cpp \
    tr_msg.cpp \
    st_com_info.cpp \
    plan_widgets.cpp \
    info_modify.cpp \
    whether_save.cpp \
    rename_st.cpp

HEADERS  += widget.h \
    login_dialog.h \
    account.h \
    subway.h \
    line.h \
    station.h \
    testhead.h \
    dialog_add_line.h \
    stt_nor.h \
    stt_adm.h \
    line_head.h \
    in_use_family.h \
    un_use_family.h \
    dialog_move.h \
    find.h \
    same_st_head.h \
    same_st_family.h \
    st_tr_info.h \
    tr_msg.h \
    st_com_info.h \
    plan_widgets.h \
    info_modify.h \
    whether_save.h \
    rename_st.h

FORMS    += widget.ui \
    dialog_add_line.ui \
    stt_nor.ui \
    stt_adm.ui \
    line_head.ui \
    in_use_family.ui \
    un_use_family.ui \
    dialog_move.ui \
    same_st_head.ui \
    same_st_family.ui \
    st_tr_info.ui \
    tr_msg.ui \
    st_com_info.ui \
    plan_widgets.ui \
    info_modify.ui \
    whether_save.ui \
    rename_st.ui

RESOURCES += \
    icons.qrc
