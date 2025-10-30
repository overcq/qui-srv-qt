/*******************************************************************************
* ©overcq                on ‟Gentoo Linux 23.0” “x86_64”            2025‒10‒30 P
*******************************************************************************/
#ifndef SIGNALS_H
#define SIGNALS_H
#include <QObject>
#include <QQuickItem>
class Z_menu_item_X_triggered_Q : public QObject {
    Q_OBJECT
public slots:
    void handle();
};
class Z_button_X_clicked_Q : public QObject {
    Q_OBJECT
public slots:
    void handle();
};
class Z_check_button_X_clicked_Q : public QObject {
    Q_OBJECT
public slots:
    void handle();
};
class Z_combo_box_X_activated_Q : public QObject {
    Q_OBJECT
public slots:
    void handle();
};
class Z_text_field_X_text_changed_Q : public QObject {
    Q_OBJECT
public slots:
    void handle();
};
#endif
/******************************************************************************/
