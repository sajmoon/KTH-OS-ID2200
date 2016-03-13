# Requirements
- "exit" works
- background and foreground processes
- Error messages
- No zombies
- Do not exit shell if foreground job is canceled with ctrl-c

Termination of processes
- Polling status of child proccees (wait()/waitpid())
- Signalhandling

Foreground and Background:
- print pid on start
- echo execution time
- echo pid on exit (on next terminal input for background)

Built in commands:
- cd
- exit
- digenv

# Stuff left to do:
- [WON'T FIX: Mac only] Memmoryleak for unkown command
- Print Env
- [DONE] Memoryleak while changing directories. cd .. loses 48bytes.
