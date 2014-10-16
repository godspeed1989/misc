#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void
sigHandler(int sig)
{
    if (sig == SIGINT) {
        printf("Caught SIGINT (%s)\n", strsignal(sig));
        return;
    } else if (sig == SIGQUIT) {
        printf("Caught SIGQUIT (%s)\n", strsignal(sig));
        exit(EXIT_SUCCESS);
    } else {
        printf("Caught (%s)\n", strsignal(sig));
    }
}

void /* Print list of signals within a signal set */
printSigset(FILE *of, const char *prefix, const sigset_t *sigset)
{
    int sig, cnt;
    cnt = 0;
    for (sig = 1; sig < NSIG; sig++) {
        if (sigismember(sigset, sig)) {
            cnt++;
            fprintf(of, "%s%d (%s)\n", prefix, sig, strsignal(sig));
        }
    }
    if (cnt == 0)
        fprintf(of, "%s<empty signal set>\n", prefix);
}

int
main(int argc, char *argv[])
{
    int n;
    sigset_t pendingMask, blockingMask, emptyMask;

    for (n = 1; n < NSIG; n++)          /* Same handler for all signals */
        signal(n, sigHandler);          /* Ignore errors */

    sigfillset(&blockingMask);          /* Block all signals */
    if (sigprocmask(SIG_SETMASK, &blockingMask, NULL) == -1)
        perror("sigprocmask");

    printf("sleep 5s for pending...\n");
    sleep(5);
    printf("\n");
    if (sigpending(&pendingMask) == -1)
        perror("sigpending");
    printSigset(stdout, "pending: ", &pendingMask);
        
    sigemptyset(&emptyMask);        /* Unblock all signals */
    if (sigprocmask(SIG_SETMASK, &emptyMask, NULL) == -1)
    perror("sigprocmask");

    for(;;)
    {
        pause();
    }
}

