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
//==============================================================================
static pid_t process_id;
static volatile int shm_id = ~0;
static char *next_commands;
static size_t next_commands_l;
static QTimer *Z_signal_I_timeout_S;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static QMetaObject::Connection Q_application_X_activate_S;
static QQuickWindow *Z_gtk_Q_main_window;
//==============================================================================
static
void
Z_signal_I_timeout( void
){  if( ~shm_id )
    {   Z_signal_I_timeout_S->start(0);
        return;
    }
    shm_id = shmget( IPC_PRIVATE, next_commands_l, 0600 | IPC_CREAT | IPC_EXCL );
    if( !~shm_id )
        exit( EXIT_FAILURE );
    void *p = shmat( shm_id, 0, 0 );
    memcpy( p, next_commands, next_commands_l );
    shmdt(p);
    free( next_commands );
    next_commands = 0;
    union sigval sv;
    sv.sival_int = shm_id;
    sigqueue( process_id, SIGUSR1, sv );
}
static
void
Z_signal_I_process_call_req_Z_void(
  char *id
){  size_t l_1 = strlen(id) + 1;
    size_t l = 0x1000 - 1;
    l = ( sizeof( uint64_t ) + l_1 + sizeof( uint64_t ) + l ) & ~l;
    char *p = ( char * )realloc( next_commands, next_commands_l + l );
    if( !p )
        exit( EXIT_FAILURE );
    next_commands = p;
    p += next_commands_l;
    next_commands_l += l;
    *( uint64_t * )p = 2;
    strcpy( p + sizeof( uint64_t ), id );
    *( uint64_t * )( p + sizeof( uint64_t ) + l_1 ) = 0;
    if( !Z_signal_I_timeout_S->isActive() )
        Z_signal_I_timeout_S->start(0);
}
static
void
Z_signal_I_process_call_req_Z_unsigned(
  char *id
, unsigned v
){  size_t l_1 = strlen(id) + 1;
    size_t l = 0x1000 - 1;
    l = ( sizeof( uint64_t ) + l_1 + sizeof(unsigned) + sizeof( uint64_t ) + l ) & ~l;
    char *p = ( char * )realloc( next_commands, next_commands_l + l );
    if( !p )
        exit( EXIT_FAILURE );
    next_commands = p;
    p += next_commands_l;
    next_commands_l += l;
    *( uint64_t * )p = 2;
    strcpy( p + sizeof( uint64_t ), id );
    *( unsigned * )( p + sizeof( uint64_t ) + l_1 ) = v;
    *( uint64_t * )( p + sizeof( uint64_t ) + l_1 + sizeof(unsigned) ) = 0;
    if( !Z_signal_I_timeout_S->isActive() )
        Z_signal_I_timeout_S->start(0);
}
static
void
Z_signal_I_process_call_req_Z_string(
  char *id
, char *s
){  size_t l_1 = strlen(id) + 1;
    size_t l_2 = strlen(s) + 1;
    size_t l = 0x1000 - 1;
    l = ( sizeof( uint64_t ) + l_1 + l_2 + sizeof( uint64_t ) + l ) & ~l;
    char *p = ( char * )realloc( next_commands, next_commands_l + l );
    if( !p )
        exit( EXIT_FAILURE );
    next_commands = p;
    p += next_commands_l;
    next_commands_l += l;
    *( uint64_t * )p = 2;
    strcpy( p + sizeof( uint64_t ), id );
    strcpy( p + sizeof( uint64_t ) + l_1, s );
    *( uint64_t * )( p + sizeof( uint64_t ) + l_1 + l_2 ) = 0;
    if( !Z_signal_I_timeout_S->isActive() )
        Z_signal_I_timeout_S->start(0);
}
/*static
void
Z_action_X_activate( GSimpleAction *action
, GVariant *parameter
, void *data
){  Z_signal_I_process_call_req_Z_void( gtk_buildable_get_buildable_id(( void *)action ));
}
static
void
Z_button_X_clicked( GtkButton *button
, void *data
){  Z_signal_I_process_call_req_Z_void( gtk_buildable_get_buildable_id(( void *)button ));
}
static
void
Z_checkbutton_X_toggled( GtkCheckButton *checkbutton
, void *data
){  Z_signal_I_process_call_req_Z_void( gtk_buildable_get_buildable_id(( void *)checkbutton ));
}
static
void
Z_dropdown_X_selected( GtkDropDown *dropdown
, GParamSpec *parameter
, void *data
){  Z_signal_I_process_call_req_Z_unsigned( gtk_buildable_get_buildable_id(( void *)dropdown ), gtk_drop_down_get_selected(dropdown) );
}
static
gboolean
Z_entry_X_changed_I_timeout( void *data
){  GtkEntry *entry = data;
    GtkEntryBuffer *buffer = gtk_entry_get_buffer(entry);
    Z_signal_I_process_call_req_Z_string( gtk_buildable_get_buildable_id(data), gtk_entry_buffer_get_text(buffer) );
    Z_entry_X_changed_I_timeout_S = 0;
    return G_SOURCE_REMOVE;
}
static
void
Z_entry_X_changed( GtkEntry *entry
, void *data
){  if( Z_entry_X_changed_I_timeout_S )
        g_source_remove( Z_entry_X_changed_I_timeout_S );
    Z_entry_X_changed_I_timeout_S = g_timeout_add( 183, Z_entry_X_changed_I_timeout, entry );
}*/
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
                            ;
                        else if( widget->inherits( "QQuickCheckButton" ))
                            ;
                        else if( widget->inherits( "QQuickComboBox" ))
                            ;
                        else if( widget->inherits( "QQuickTextField" ))
                            ;
                        else if( widget->inherits( "QQuickMenu" ))
                            ;
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
                        {   widget->setProperty( "value", *( double * )p );
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
Q_application_X_activate( void
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
    process_id = getppid();
    QApplication app( argc, argv );
    Z_signal_I_timeout_S = new QTimer();
    QObject::connect( Z_signal_I_timeout_S, &QTimer::timeout, &Z_signal_I_timeout );
    QQmlEngine engine;
    QQmlComponent component( &engine );
    QString s = R"(
import QtQuick
import QtQuick.Controls
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
