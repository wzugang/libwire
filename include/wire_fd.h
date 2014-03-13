#ifndef WIRE_FD_LIB_H
#define WIRE_FD_LIB_H

/** @file
 * XCoro file descriptor poll loop.
 */

/** @defgroup FDPoller FD poller
 *
 * This part of the library gives support for polling, sleeping and waiting
 * until a file descriptor is active and requires attention. It assumes that
 * the file descriptor is in a non-blocking state. It is expected to work for
 * pipes and sockets only and not for files as reading and writing to a real
 * file fd will result in a blocking action which should be avoided.
 */
/// @{

/** Initialize the XCoro file descriptor poller. This is currently global per app and should become local to an XCoro thread.
 */
void wire_fd_init(void);

/** One shot waiting for an FD to become readable. This is the less performant
 * cousing of the fd mode functions below. It will add and remove the fd from
 * the polling mechanism for each call.
 */
int wire_fd_wait_read(int fd);

/** Sleep in the poller for msecs time. The current coroutine is suspended and resumed back when the time passes. */
int wire_fd_wait_msec(int msecs);

/** File descriptor polling mode. When monitoring a file descriptor it can be
 * placed in either read, write or no monitoring. When in read or write
 * monitoring mode whenever the polling task sees the fd in this mode it will
 * resume the originating task assuming that it is suspended and waiting for a
 * wakeup call.
 */
typedef enum wire_fd_mode {
	FD_MODE_NONE,
	FD_MODE_READ,
	FD_MODE_WRITE,
} wire_fd_mode_e;

/** Internal tracking state. The waiting task allocates and maintains this
 * struct but it is only ever changed by the wire_fd_mode_* functions.
 */
typedef struct wire_fd_state {
	int fd;
	wire_fd_mode_e state;
} wire_fd_state_t;

/** Initialize the wire_fd_state_t struct, set the file descriptor to be monitored and sets the mode in FD_MODE_NONE by default. */
void wire_fd_mode_init(wire_fd_state_t *state, int fd);

/** Sets the mode to be FD_MODE_READ. This will also change the internal fd polling state. */
int wire_fd_mode_read(wire_fd_state_t *fd_state);
/** Sets the mode to be FD_MODE_WRITE. This will also change the internal fd polling state. */
int wire_fd_mode_write(wire_fd_state_t *fd_state);
/** Sets the mode to be FD_MODE_NONE. This will also change the internal fd polling state to disable the monitoring, no more wakeups will happen for this FD. */
int wire_fd_mode_none(wire_fd_state_t *fd_state);

/** Go to sleep waiting for the notification from the polling task. */
void wire_fd_wait(wire_fd_state_t *fd_state);

/// @}

#endif
