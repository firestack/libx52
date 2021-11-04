/*
 * Saitek X52 Pro MFD & LED driver - Daemon controller
 *
 * Copyright (C) 2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

/**
@page x52ctl Command Line controller to X52 daemon

\htmlonly
<b>x52ctl</b> - Command line controller to X52 daemon
\endhtmlonly

# SYNOPSIS
<tt>\b x52ctl [\a -i] [\a -s socket-path] [command] </tt>

# DESCRIPTION

x52ctl is a program that can be used to communicate with the X52 daemon. It can
be used either as a one-shot program that can be run from another program or
script, or it can be run interactively.

Commands are sent to the running daemon, and responses are written to standard
output.

If not running interactively, then you must specify a command, or the program
will exit with a failure exit code. If running interactively, the program will
request input and send that to the daemon, until the user either enters the
string "quit", or terminates input by using Ctrl+D.
*/

#include "config.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include "x52d_const.h"
#include "x52dcomm.h"

#define APP_NAME "x52ctl"
#if HAVE_FUNC_ATTRIBUTE_NORETURN
__attribute__((noreturn))
#endif
static void usage(int exit_code)
{
    fprintf(stderr, _("Usage: %s [-i] [-s socket-path] [command]\n"), APP_NAME);
    exit(exit_code);
}

static int send_command(int sock_fd, char *buffer, size_t buflen)
{
    int rc;
    rc = x52d_send_command(sock_fd, buffer, buflen);
    if (rc >= 0) {
        if (write(STDOUT_FILENO, buffer, rc) < 0) {
            perror("write");
            return -1;
        }
    } else {
        perror("x52d_send_command");
        return -1;
    }

    fputc('\n', stdout);
    return 0;
}

int main(int argc, char **argv)
{
    bool interactive = false;
    char *socket_path = NULL;
    int opt;
    int i;
    int sock_fd;
    int rc = EXIT_SUCCESS;

    char buffer[1024];
    size_t buflen;

    /*
     * Parse command line arguments
     *
     * -i   Interactive
     * -s   Socket path
     */
    while ((opt = getopt(argc, argv, "is:h")) != -1) {
        switch (opt) {
        case 'i':
            interactive = true;
            break;

        case 's':
            socket_path = optarg;
            break;

        case 'h':
            usage(EXIT_SUCCESS);
            break;

        default:
            usage(EXIT_FAILURE);
            break;
        }
    }

    if (!interactive && optind >= argc) {
        usage(EXIT_FAILURE);
    }

    /* Connect to the socket */
    sock_fd = x52d_dial_command(socket_path);
    if (sock_fd < 0) {
        perror("x52d_dial_command");
        return EXIT_FAILURE;
    }

    if (interactive) {
        if (optind < argc) {
            fprintf(stderr,
                    _("Running in interactive mode, ignoring extra arguments\n"));
        }

        fputs("> ", stdout);
        while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            if (strcasecmp(buffer, "quit\n") == 0) {
                break;
            }

            if (send_command(sock_fd, buffer, strlen(buffer))) {
                rc = EXIT_FAILURE;
                goto cleanup;
            }

            fputs("> ", stdout);
        }

    } else {
        memset(buffer, 0, sizeof(buffer));
        buflen = 0;
        for (i = optind; i < argc; i++) {
            buflen += snprintf(&buffer[buflen], sizeof(buffer) - buflen,
                               "%s ", argv[i]);

            if (buflen >= sizeof(buffer)) {
                fprintf(stderr, _("Argument length too long\n"));
                rc = EXIT_FAILURE;
                goto cleanup;
            }
        }

        if (buflen > sizeof(buffer)) {
            buflen = sizeof(buffer);
        }
        buflen--;
        buffer[buflen] = '\0';

        if (send_command(sock_fd, buffer, buflen)) {
            rc = EXIT_FAILURE;
            goto cleanup;
        }
    }


cleanup:
    close(sock_fd);
    return rc;
}
