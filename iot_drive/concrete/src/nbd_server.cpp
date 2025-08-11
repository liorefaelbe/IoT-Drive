#include <sys/socket.h>             // socketpair
#include <sys/wait.h>               // waitpid
#include <sys/ioctl.h>              // ioctl
#include <fcntl.h>                  // O_RDWR
#include <signal.h>                 // SIGTERM
#include <netinet/in.h>             // sockaddr_in
#include <linux/nbd.h>              // NBD_SET_SIZE
#include <unistd.h>                 // read
#include <assert.h>                 // assert
#include <errno.h>                  // errno
#include <err.h>                    // worn
#include <string.h>                 // strerror
#include <iostream>                 // std::cerr

#include "colors.hpp"				// RESET
#include "nbd_server.hpp"           // NBDServer
#include "master_read_args.hpp"     // MasterReadArgs
#include "master_write_args.hpp"    // MasterWriteArgs

#ifndef BUSE_DEBUG
  #define BUSE_DEBUG (1)
#endif

#ifdef WORDS_BIGENDIAN
u_int64_t ntohll(u_int64_t a) 
{
  return a;
}
#else
u_int64_t ntohll(u_int64_t a) 
{
  uint32_t lo = a & 0xffffffff;
  uint32_t hi = a >> 32U;
  lo = ntohl(lo);
  hi = ntohl(hi);
  return ((u_int64_t) lo) << 32U | hi;
}
#endif
#define htonll ntohll

using namespace std;
namespace ilrd_166_7
{

static const uint32_t DRIVE_SIZE = 1024 * 1024 * 4 * 3;
// static char MINIONS[DRIVE_SIZE]{0};
// static int read_req_num = 1;
// static int write_req_num = 1;

/*************************** Forward Declaration ********************************/

static int g_nbd_dev_to_disconnect = -1;

static void ChildProcess(int socket_pair[], int nbd);
static void ParentProcess(int socket_pair[], int nbd);

static int read_all(int fd, char* buf, size_t count);
static int write_all(int fd, char* buf, size_t count);
static void disconnect_nbd(int signal);
static int set_sigaction(int sig, const struct sigaction * act);

/**************************** Implementations *********************************/
/**************************** Member Functions ********************************/

NBDServer::NBDServer()
{
	int err = socketpair(AF_UNIX, SOCK_STREAM, 0, m_sp);
	if (err == -1)
	{
		throw runtime_error("failed to create socketpair");
	}

	int nbd = open("/dev/nbd0", O_RDWR);
	if (nbd == -1) 
	{
		throw runtime_error("failed to open nbd");
	}
	
	err = ioctl(nbd, NBD_SET_SIZE, DRIVE_SIZE);
	assert(err != -1);

	err = ioctl(nbd, NBD_CLEAR_QUE);
	assert(err != -1);

	err = ioctl(nbd, NBD_CLEAR_SOCK);
	assert(err != -1);

	pid_t pid = fork();
	
	if (pid == 0)
	{
		ChildProcess(m_sp, nbd);
	}
	else
	{
		ParentProcess(m_sp, nbd);
	}
}

NBDServer::~NBDServer()
{
	if (close(m_sp[0]) != 0)
	{ 
		warn("problem closing server side nbd socket");
	}

	if (wait(NULL) == -1) 
	{
		warn("waitpid failed");
	}
}

shared_ptr<IKeyArgs> NBDServer::GetKeyArgs()
{
    struct nbd_request request {};
    
    ssize_t bytes_read = read(m_sp[0], &request, sizeof(request)); 
    if (bytes_read == -1) 
	{
        if (errno == EINTR) 
		{
            return nullptr;
        }
        throw runtime_error(string("error reading userside of nbd socket: ") + strerror(errno));
    }

    if (bytes_read == 0) 
	{
        return nullptr;
    }

    if (bytes_read != sizeof(request)) 
	{
        throw runtime_error("incomplete nbd request read");
    }

    char handle[8];
    memcpy(handle, request.handle, sizeof(request.handle));

    uint32_t len = ntohl(request.len);
    uint64_t from = ntohll(request.from);

    switch(ntohl(request.type)) 
	{
        case NBD_CMD_READ:
		{	
			// cout << GREEN << "NBD_CMD_READ - send #" << read_req_num << " | nbdUid: " << *(size_t*)handle << RESET << endl;
			// ++read_req_num;
			// Respond(handle, len, MINIONS + from);
			// return nullptr;
			
            return (make_shared<MasterReadArgs>(handle, from, len));
		}    
        case NBD_CMD_WRITE: 
		{	
			// cout << GREEN << "NBD_CMD_WRITE - send #" << write_req_num << " | nbdUid: " << *(size_t*)handle << RESET << endl;
			// ++write_req_num;

	  		char* chunk = (char*)malloc(len);

	  		read_all(m_sp[0], (char*)chunk, len);
			// memcpy(MINIONS + from, chunk, len);
			// Respond(handle, 0);
			shared_ptr<MasterWriteArgs> ptr = make_shared<MasterWriteArgs>(handle, from, len, chunk);
			free (chunk);
			// return nullptr;
			return (ptr);
		}
        case NBD_CMD_FLUSH:
        {    
			cout << "NBD_CMD_FLUSH - sending success" << endl;
            Respond(handle, 0);
            return nullptr;
		}    
        case NBD_CMD_TRIM:
        {    
			cout << "NBD_CMD_TRIM - sending success" << endl;
            Respond(handle, 0);
            return nullptr;
		}
        case NBD_CMD_DISC:
        {    
			cout << "NBD_CMD_DISC - disconnecting" << endl;
            Respond(handle, 0);
            return nullptr;
		}    
        default:
        {    
			cout << "Unknown NBD command: " << ntohl(request.type) << endl;
            Respond(handle, ENOTSUP);
            return nullptr;
		}
    }
}

int NBDServer::GetFD()
{
	return (m_sp[0]);
}

void NBDServer::Respond(char handle[8], uint32_t size, const char* buffer)
{
    if (SendStatus(handle, 0) != 0) 
	{
        return;
    }
    
    if (size > 0 && buffer) 
	{
        write_all(m_sp[0], const_cast<char*>(buffer), size);
    }
}

void NBDServer::Respond(char handle[8], uint8_t status)
{
 	SendStatus(handle, status);
}

int NBDServer::SendStatus(char handle[8], uint32_t status)
{
    struct nbd_reply reply;
    reply.magic = htonl(NBD_REPLY_MAGIC);
    reply.error = htonl(status);
    memcpy(reply.handle, handle, sizeof(reply.handle));
    
    return (write_all(m_sp[0], (char*)&reply, sizeof(struct nbd_reply)));
}

/**************************** Static Functions **********************************/

static void ChildProcess(int socket_pair[], int nbd)
{
	sigset_t sigset;
	if (sigfillset(&sigset) != 0 || sigprocmask(SIG_SETMASK, &sigset, NULL) != 0) 
	{
 	 	throw runtime_error("failed to block signals in child");
	}

	close(socket_pair[0]);

	if(ioctl(nbd, NBD_SET_SOCK, socket_pair[1]) == -1)
	{
  		throw runtime_error("ioctl(nbd, NBD_SET_SOCK, socket_pair[1]) failed.");
	}

	int flags;
	
	#if defined NBD_SET_FLAGS
	flags = 0;

	#if defined NBD_FLAG_SEND_TRIM
	flags |= NBD_FLAG_SEND_TRIM;
	#endif

	#if defined NBD_FLAG_SEND_FLUSH
	flags |= NBD_FLAG_SEND_FLUSH;
	#endif

	if (flags != 0 && ioctl(nbd, NBD_SET_FLAGS, flags) == -1)
	{
		throw runtime_error("ioctl(nbd, NBD_SET_FLAGS failed.");
	}
	#endif
	
	int err = ioctl(nbd, NBD_DO_IT);
	
	if (BUSE_DEBUG) 
	{
		fprintf(stderr, "nbd device terminated with code %d\n", err);
	}

	if (err == -1) 
	{
		warn("NBD_DO_IT terminated with error");
		exit(EXIT_FAILURE);
	}

	if (ioctl(nbd, NBD_CLEAR_QUE) == -1 || ioctl(nbd, NBD_CLEAR_SOCK) == -1) 
	{
  		warn("failed to perform nbd cleanup actions");
  		exit(EXIT_FAILURE);
	}

	exit(0);
}

static void ParentProcess(int socket_pair[], int nbd)
{
	assert(g_nbd_dev_to_disconnect == -1);
	g_nbd_dev_to_disconnect = nbd;

	struct sigaction act;
	act.sa_handler = disconnect_nbd;
	act.sa_flags = SA_RESTART;
	if (
		sigemptyset(&act.sa_mask) != 0 ||
		sigaddset(&act.sa_mask, SIGINT) != 0 ||
		sigaddset(&act.sa_mask, SIGTERM) != 0) 
	{
		throw runtime_error("failed to prepare signal mask in parent");
	}

	if (set_sigaction(SIGINT, &act) != 0 || set_sigaction(SIGTERM, &act) != 0) 
	{
		throw runtime_error("failed to register signal handlers in parent");
 	}

	close(socket_pair[1]);
}

static int read_all(int fd, char* buf, size_t count)
{
	int bytes_read;

	while (count > 0) 
	{
		bytes_read = read(fd, buf, count);
		assert(bytes_read > 0);
		buf += bytes_read;
		count -= bytes_read;
	}
	assert(count == 0);

	return (0);
}

static int write_all(int fd, char* buf, size_t count)
{
	int bytes_written;

	while (count > 0) 
	{
		bytes_written = write(fd, buf, count);
		assert(bytes_written > 0);
		buf += bytes_written;
		count -= bytes_written;
	}
  	assert(count == 0);

  	return 0;
}

static void disconnect_nbd(int signal) 
{
	(void)signal;
	if (g_nbd_dev_to_disconnect != -1) 
	{
		if(ioctl(g_nbd_dev_to_disconnect, NBD_DISCONNECT) == -1) 
		{
			warn("failed to request disconect on nbd device");
		} 
		else 
		{
			g_nbd_dev_to_disconnect = -1;
			fprintf(stderr, "sucessfuly requested disconnect on nbd device\n");
		}
	}
}

static int set_sigaction(int sig, const struct sigaction * act) 
{
	struct sigaction oact;
	int r = sigaction(sig, act, &oact);

	if (r == 0 && oact.sa_handler != SIG_DFL) 
	{
		warnx("overriden non-default signal handler (%d: %s)", sig, strsignal(sig));
	}
  	return r;
}
}