#include "us_world.h"
#include "us_daemon.h"
#include "us_logger.h"
#include <pwd.h>

#if defined(US_CONFIG_POSIX)

static char us_daemon_pid_file_name[1024];

void us_daemon_drop_root(
    const char * uid_name
)
{
    uid_t new_uid=0;
    uid_t new_gid=0;
    setsid(); /* new process group */
    if( getuid()==0 )
    {
        /* lookup uid and gid by user name */
        struct passwd *pw = getpwnam( uid_name );
        if( pw )
        {
            new_uid = pw->pw_uid;
            new_gid = pw->pw_gid;
        }
        else
        {
            /* that failed, maybe uid is actually a number */
            new_uid = strtol( uid_name, 0, 10 );
            pw = getpwuid( new_uid );
            if( pw )
                new_gid = pw->pw_gid;
        }
        if( setegid(new_gid)<0 ||
                seteuid(new_uid)<0 )
        {
            perror("error setting euid/egid");
            exit(1);
        }
    }
}

void us_daemon_daemonize(
    bool real_daemon,
    const char * identity,
    const char * home_dir,
    const char * pid_file,
    const char * lock_file,
    const char * new_uid
)
{
    /* remember the pid file name */
    us_strncpy( us_daemon_pid_file_name, pid_file, 1024 );
    if( strlen(home_dir)>0 )
    {
        mkdir(home_dir,0750);
        chdir(home_dir);
    }
    if( real_daemon )
    {
        switch(fork())
        {
        case -1:
            perror( "fork() failed" );
            exit(1);
        case 0:
            break;
        default:
            _exit(0);
            break;
        };
    }
    if( strlen(new_uid)>0 )
    {
        us_daemon_drop_root(new_uid);
    }
    umask(027);
    if( strlen(us_daemon_pid_file_name)>0 )
    {
        int pid_fd = open( us_daemon_pid_file_name, O_CREAT | O_TRUNC | O_WRONLY | O_NOFOLLOW, 0640 );
        if( pid_fd>=0 )
        {
            char tmpbuf[64];
            sprintf( tmpbuf, "%ld\n", (long)getpid() );
            int len = strlen(tmpbuf);
            if( write( pid_fd, tmpbuf, len)!=len )
            {
                us_log_error( "Error writing pid file: %s", us_daemon_pid_file_name );
                abort();
            }
            close( pid_fd );
            atexit( us_daemon_end );
        }
        else
        {
            us_log_error("Error creating pid file: %s", us_daemon_pid_file_name );
            abort();
        }
    }
    if( lockf(pid_fd,F_TLOCK,0)<0)
    {
        us_log_error( "Unable to lock pid file: %s", us_daemon_pid_file_name );
        abort();
    }
    if( real_daemon )
    {
        int fd;
        for( fd=0; fd<getdtablesize(); ++fd )
        {
            close(fd);
        }
        fd = open("/dev/null", O_RDWR );
        dup2(fd, STDIN_FILENO);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
    }
    us_daemon_prepare_child_start();
}


void us_daemon_prepare_child_start(void)
{
}

pid_t us_daemon_fork( void )
{
    pid_t i = fork();
    if( i == 0 )
    {
        // in child, clean up some things
        us_daemon_prepare_child_start();
    }
    if( i<0 )
    {
        perror("fork failed");
        abort();
    }
    return i;
}

void us_daemon_end(void)
{
    if( strlen(us_daemon_pid_file_name)>0 )
    {
        if( unlink( us_daemon_pid_file_name )<0 )
        {
            us_log_error("remove pid file failed: %s", strerror(errno));
        }
    }
}

#endif

