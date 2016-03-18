# Requirements
- "exit" works
- background and foreground processes
- Error messages
- No zombies
- Do not exit shell if foreground job is canceled with ctrl-c

Termination of processes
- [CHECK] Polling status of child proccees (wait()/waitpid())
- [CHECK] Signalhandling

Foreground and Background:
- [CHECK] print pid on start
- [CHECK] echo execution time
- [CHECK] echo pid on exit (on next terminal input for background)

Built in commands:
- [CHECK] cd
- [CHECK] exit
- [CHECK] digenv

# Stuff left to do:
- [WON'T FIX: Mac only] Memmoryleak for unkown command
- [DONE] Print Env
- [DONE] Memoryleak while changing directories. cd .. loses 48bytes.
- Do not exit shell if foreground job is canceled with ctrl-c
    i.e. "sleep 2", then ^C