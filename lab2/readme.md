# Docker stuff:
docker build -t os-lab2 . && docker run -it --rm --name oslab os-lab2

# Requirements

- "exit" works
- background and foreground processes
- Error messages
- No zombies
- Do not exit shell if foreground job is canceled with ctrl-c

Termination of processes
- Polling status of child proccees (wait()/waitpid())
- Signalhandling

Foreground:
- print pid on start
- print pid on completion
- echo execution time
Background:
- echo pid on create
- echo pid on exit (on next terminal input)

Built in commands:
- cd
- exit
