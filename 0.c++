/*******************************************************************************
* ©overcq                on ‟Gentoo Linux 23.0” “x86_64”            2025‒10‒29 O
*******************************************************************************/
#include <sys/shm.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <QApplication>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QQuickItem>
#include <QQuickWindow>
#include <QString>
#include <QTimer>
#include <QWidget>
#include "signals.h"
//==============================================================================
static sigset_t sigset_req;
static pid_t process_id;
static volatile int shm_id = ~0;
static char *next_commands;
static size_t next_commands_l;
static QTimer *Z_signal_I_timeout_S;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static QMetaObject::Connection Q_application_X_activate_S;
static QQuickWindow *Z_gtk_Q_main_window;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static Z_menu_item_X_triggered_Q Z_menu_item_X_triggered_S;
static Z_button_X_clicked_Q Z_button_X_clicked_S;
static Z_check_button_X_clicked_Q Z_check_button_X_clicked_S;
static Z_combo_box_X_activated_Q Z_combo_box_X_activated_S;
static Z_text_field_X_text_changed_Q Z_text_field_X_text_changed_S;
//==============================================================================
void
Z_signal_I_timeout(
){  if( ~shm_id
    || !next_commands_l
    )
        return;
    sigprocmask( SIG_BLOCK, 0, &sigset_req );
    shm_id = shmget( IPC_PRIVATE, next_commands_l, 0600 | IPC_CREAT | IPC_EXCL );
    if( !~shm_id )
        exit( EXIT_FAILURE );
    void *p = shmat( shm_id, 0, 0 );
    memcpy( p, next_commands, next_commands_l );
    next_commands_l = 0;
    shmdt(p);
    free( next_commands );
    next_commands = 0;
    sigprocmask( SIG_UNBLOCK, 0, &sigset_req );
    union sigval sv;
    sv.sival_int = shm_id;
    sigqueue( process_id, SIGUSR1, sv );
}
void
Z_signal_I_process_call_req_Z_void(
  char *id
){  size_t l_1 = strlen(id) + 1;
    size_t l = 0x1000 - 1;
    l = ( next_commands_l + sizeof( uint64_t ) + l_1 + sizeof( uint64_t ) + l ) & ~l;
    char *p = static_cast<char*>( realloc( next_commands, l ));
    if( !p )
        exit( EXIT_FAILURE );
    next_commands = p;
    p += next_commands_l;
    next_commands_l = l;
    *( uint64_t * )p = 2;
    strcpy( p + sizeof( uint64_t ), id );
    *( uint64_t * )( p + sizeof( uint64_t ) + l_1 ) = 0;
}
void
Z_signal_I_process_call_req_Z_unsigned(
  char *id
, unsigned v
){  size_t l_1 = strlen(id) + 1;
    size_t l = 0x1000 - 1;
    l = ( next_commands_l + sizeof( uint64_t ) + l_1 + sizeof(unsigned) + sizeof( uint64_t ) + l ) & ~l;
    char *p = static_cast<char*>( realloc( next_commands, l ));
    if( !p )
        exit( EXIT_FAILURE );
    next_commands = p;
    p += next_commands_l;
    next_commands_l = l;
    *( uint64_t * )p = 2;
    strcpy( p + sizeof( uint64_t ), id );
    *( unsigned * )( p + sizeof( uint64_t ) + l_1 ) = v;
    *( uint64_t * )( p + sizeof( uint64_t ) + l_1 + sizeof(unsigned) ) = 0;
}
void
Z_signal_I_process_call_req_Z_string(
  char *id
, char *s
){  size_t l_1 = strlen(id) + 1;
    size_t l_2 = strlen(s) + 1;
    size_t l = 0x1000 - 1;
    l = ( next_commands_l + sizeof( uint64_t ) + l_1 + l_2 + sizeof( uint64_t ) + l ) & ~l;
    char *p = static_cast<char*>( realloc( next_commands, l ));
    if( !p )
        exit( EXIT_FAILURE );
    next_commands = p;
    p += next_commands_l;
    next_commands_l = l;
    *( uint64_t * )p = 2;
    strcpy( p + sizeof( uint64_t ), id );
    strcpy( p + sizeof( uint64_t ) + l_1, s );
    *( uint64_t * )( p + sizeof( uint64_t ) + l_1 + l_2 ) = 0;
}
void
Z_menu_item_X_triggered_Q::handle(
){  QQuickItem* widget = qobject_cast<QQuickItem*>( sender() );
    Z_signal_I_process_call_req_Z_void( widget->objectName().toUtf8().data() );
}
void
Z_button_X_clicked_Q::handle(
){  QQuickItem* widget = qobject_cast<QQuickItem*>( sender() );
    for( auto widget_ : widget->findChildren<QQuickItem*>() )
        if( widget_->inherits( "QQuickPopupItem" ))
        {   widget_->setProperty( "visible", !widget_->property( "visible" ).toBool() );
            return;
        }
    Z_signal_I_process_call_req_Z_void( widget->objectName().toUtf8().data() );
}
void
Z_check_button_X_clicked_Q::handle(
){  QQuickItem* widget = qobject_cast<QQuickItem*>( sender() );
    Z_signal_I_process_call_req_Z_void( widget->objectName().toUtf8().data() );
}
void
Z_combo_box_X_activated_Q::handle(
){  QQuickItem* widget = qobject_cast<QQuickItem*>( sender() );
    Z_signal_I_process_call_req_Z_unsigned( widget->objectName().toUtf8().data(), static_cast<unsigned>( widget->property( "currentIndex" ).toInt() ));
}
void
Z_text_field_X_text_changed_Q::handle(
){  QQuickItem* widget = qobject_cast<QQuickItem*>( sender() );
    Z_signal_I_process_call_req_Z_string( widget->objectName().toUtf8().data(), widget->property( "text" ).toString().toUtf8().data() );
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static
void
Z_signal_V_process_call_req( int uid
, siginfo_t *siginfo
, void *data
){  if( siginfo->si_code != SI_QUEUE )
        return;
    void *shm = shmat( siginfo->si_value.sival_int, 0, 0 );
    if( !~( unsigned long )shm )
        return;
    char *p = ( char * )shm;
    uint64_t command = *( uint64_t * )p;
    while(command)
    {   p += sizeof( uint64_t );
        bool invalid = false;
        switch(command)
        { case 1:
            {   for( auto window : QApplication::allWindows() )
                {   window->close();
                    window->deleteLater();
                }
                break;
            }
          case 2:
            {   if( !Z_gtk_Q_main_window )
                {   invalid = true;
                    break;
                }
                Z_gtk_Q_main_window->close();
                Z_gtk_Q_main_window->deleteLater();
                Z_gtk_Q_main_window = 0;
                QQmlEngine engine;
                QQmlComponent component( &engine );
                QString s{p};
                QStringList a = s.split( "ApplicationWindow{" );
                for( auto i = 1; i < a.size(); i++ )
                {   QString s{ a[0] };
                    s.append( "ApplicationWindow{" ).append( a[i] );
                    component.setData( s.toUtf8(), QUrl() );
                    if( !component.isReady() )
                        exit( EXIT_FAILURE );
                    QQuickWindow* window = qobject_cast<QQuickWindow*>( component.create() );
                    for( auto widget : window->findChildren<QQuickItem*>() )
                    {   if( widget->inherits( "QQuickButton" ))
                            QObject::connect( widget, SIGNAL( clicked() ), &Z_button_X_clicked_S, SLOT( handle() ));
                        else if( widget->inherits( "QQuickCheckButton" ))
                            QObject::connect( widget, SIGNAL( clicked() ), &Z_check_button_X_clicked_S, SLOT( handle() ));
                        else if( widget->inherits( "QQuickComboBox" ))
                            QObject::connect( widget, SIGNAL( activated(int) ), &Z_combo_box_X_activated_S, SLOT( handle() ));
                        else if( widget->inherits( "QQuickTextField" ))
                            QObject::connect( widget, SIGNAL( textChanged() ), &Z_text_field_X_text_changed_S, SLOT( handle() ));
                        else if( widget->inherits( "QQuickMenuItem" ))
                            QObject::connect( widget, SIGNAL( triggered() ), &Z_menu_item_X_triggered_S, SLOT( handle() ));
                    }
                    window->show();
                }
                p += strlen(p) + 1;
                break;
            }
          case 3:
            {   QString id{p};
                p += strlen(p) + 1;
                for( auto window : QApplication::allWindows() )
                {   auto list = window->findChildren<QQuickItem*>(id);
                    if( !list.isEmpty() )
                    {   auto widget = list.first();
                        if( widget->inherits( "QQuickTextField" ))
                        {   QString s{p};
                            widget->property( "text" ) = s;
                            p += strlen(p) + 1;
                        }else if( widget->inherits( "QQuickProgressBar" ))
                        {   widget->setProperty( "value", *reinterpret_cast<double *>(p) );
                            p += sizeof(double);
                        }else if( widget->inherits( "QQuickBusyIndicator" ))
                        {   widget->property( "running" ) = !widget->property( "running" ).toBool();
                            p += sizeof(double);
                        }
                        break;
                    }
                }
                break;
            }
          default:
                invalid = true;
                break;
        }
        if(invalid)
            break;
        command = *( uint64_t * )p;
    }
    shmdt(shm);
    union sigval sv;
    sigqueue( siginfo->si_pid, SIGUSR2, sv );
}
static
void
Z_signal_V_process_call_reply( int uid
, siginfo_t *siginfo
, void *data
){  shmctl( shm_id, IPC_RMID, 0 );
    shm_id = ~0;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static
void
Q_application_X_activate(
){  size_t l = 0x1000 - 1;
    l = ( 2 * sizeof( uint64_t ) + l ) & ~l;
    shm_id = shmget( IPC_PRIVATE, l, 0600 | IPC_CREAT | IPC_EXCL );
    uint64_t *n = ( uint64_t * )shmat( shm_id, 0, 0 );
    n[0] = 1;
    n[1] = 0;
    shmdt(n);
    union sigval sv;
    sv.sival_int = shm_id;
    sigqueue( process_id, SIGUSR1, sv );
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int
main( int argc
, char *argv[]
){  struct sigaction sa;
    sigfillset( &sa.sa_mask );
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = Z_signal_V_process_call_req;
    sigaction( SIGUSR1, &sa, 0 );
    sa.sa_sigaction = Z_signal_V_process_call_reply;
    sigaction( SIGUSR2, &sa, 0 );
    sa.sa_flags = 0;
    sa.sa_handler = SIG_IGN;
    sigaction( SIGVTALRM, &sa, 0 );
    sigemptyset( &sigset_req );
    sigaddset( &sigset_req, SIGUSR1 );
    process_id = getppid();
    QApplication app( argc, argv );
    Z_signal_I_timeout_S = new QTimer();
    Z_signal_I_timeout_S->start(183);
    QObject::connect( Z_signal_I_timeout_S, &QTimer::timeout, &Z_signal_I_timeout );
    QQmlEngine engine;
    QQmlComponent component( &engine );
    QString s = R"(
import QtQuick.Controls
import QtQuick.Window
ApplicationWindow{visible:true}
    )";
    component.setData( s.toUtf8(), QUrl() );
    if( component.isReady() )
    {   Z_gtk_Q_main_window = qobject_cast<QQuickWindow*>( component.create() );
        Q_application_X_activate_S = QObject::connect( Z_gtk_Q_main_window
        , &QQuickWindow::afterRendering
        , []()
        {   QObject::disconnect( Q_application_X_activate_S );
            QTimer::singleShot( 0, &Q_application_X_activate );
        });
        return app.exec();
    }
    return EXIT_FAILURE;
}
/******************************************************************************/
